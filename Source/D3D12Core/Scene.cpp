#include "pch.h"

#include "AppMain.h"
#include "CommandList.h"
#include "GraphicsManager.h"
#include "CommandQueue.h"
#include "GraphicsCommon.h"
#include "SwapChain.h"
#include "GameTime.h"
#include "Input.h"

#include "TextureLoader.h"

#include "PipelineState.h"
#include "GraphicsBuffer.h"
#include "Texture2D.h"
#include "RenderTexture.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "ConstansBuffer.h"

#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "Light.h"

#include "Scene.h"

using namespace std;
using namespace Graphics;
using namespace Application;
using namespace Resources;

namespace Game
{
    GameObject& Scene::AddGameObject(std::unique_ptr<GameObject>&& gameObj)
    {
        auto result = gameObj.get();
        m_HierarchyObject.push_back(VersionedGameObject());
        m_HierarchyObject.back().m_GameObject = move(gameObj);
        m_HierarchyObject.back().m_Version = 0;
        m_IsComponentChanged = true;
        return *result;
    }
    GameObject* Scene::FindGameObject(const std::wstring& name) const
    {
        for (auto& verObj : m_HierarchyObject)
        {
            auto& obj = verObj.m_GameObject;
            if (obj->GetTransform().GetParent() || !obj->GetActive()) continue;
            if (obj->m_Name == name)
                return obj.get();
        }
        return nullptr;
    }
    bool Scene::RemoveGameObject(const GameObject* obj)
    {
        auto result = false;
        auto it = m_HierarchyObject.begin();
        for (; it != m_HierarchyObject.end(); ++it)
        {
            if (it->m_GameObject.get() == obj)
            {
                m_HierarchyObject.erase(it);
                result = true;
                m_IsComponentChanged = true;
                break;
            }
        }
        return result;
    }

    void Scene::ExecuteScene()
    {
        ExecuteUpdate();
        ExecutePrevRender();
        ExecuteRender();
    }
    void Scene::CloseScene()
    {
        m_HierarchyObject.clear();
        m_BakedComponents.clear();
        m_ResourceMap.clear();
    }

    void Scene::ExecutePrevRender()
    {
        // 检测是否需要重新烘焙组件
        if (!m_IsComponentChanged)
        {
            for (auto& verObj : m_HierarchyObject)
            {
                auto version = verObj.m_GameObject->GetVersion();
                if (verObj.m_Version != version || version == 0)
                {
                    m_IsComponentChanged = true;
                    break;
                }
            }
        }

        if (m_IsComponentChanged)
        {
            // 重新刷新烘焙组件
            m_IsComponentChanged = false;
            m_BakedComponents.clear();

            auto objIt = m_HierarchyObject.begin();
            while (objIt != m_HierarchyObject.end())
            {
                if (objIt->m_GameObject->GetVersion() == 0)
                {
                    // 移除已删除元素
                    objIt = m_HierarchyObject.erase(objIt);
                    continue;
                }
                // 记录组件指针
                objIt->m_Version = objIt->m_GameObject->GetVersion();
                for (auto& componentList : objIt->m_GameObject->GetRawComponents())
                {
                    auto& bakedComponents = m_BakedComponents[componentList.first];
                    for (auto& component : componentList.second)
                    {
                        bakedComponents.push_back(component.get());
                    }
                }
                ++objIt;
            }
        }
    }

    void Scene::ExecuteRender()
    {
        vector<CommandList*> commandListArray{};
        RenderPass1(commandListArray);

        GraphicsManager::GetGraphicsCommandQueue()->ExecuteCommandLists(commandListArray.data(), static_cast<UINT>(commandListArray.size()));
        commandListArray.clear();
        GraphicsManager::GetGraphicsCommandQueue()->WaitForQueueCompleted();

        RenderPass2(commandListArray);

        GraphicsManager::GetGraphicsCommandQueue()->ExecuteCommandLists(commandListArray.data(), static_cast<UINT>(commandListArray.size()));
        commandListArray.clear();
        GraphicsManager::GetGraphicsCommandQueue()->WaitForQueueCompleted();

        RenderPass3(commandListArray);

        GraphicsManager::GetGraphicsCommandQueue()->ExecuteCommandLists(commandListArray.data(), static_cast<UINT>(commandListArray.size()));
        commandListArray.clear();
        GraphicsManager::GetGraphicsCommandQueue()->WaitForQueueCompleted();

        // 呈现帧。
        CHECK_HRESULT(GraphicsManager::GetSwapChain()->GetD3D12SwapChain()->Present(0, 0));

        TimeSystem::AddFrameCompleted(); // 帧完成信号+1
        TimeSystem::AddSwapFrameCompleted(); // 已交换帧数量+1

        SetWindowTitle(Format(g_TitleFormat.c_str(), WINDOW_TITLE, g_TitleGPU.c_str(), Time::GetAverageFPS()));
    }

    void Scene::RenderPass1(vector<CommandList*>& commandListArray)
    {
        static vector<ConstansBuffer<ShaderCommon::CameraBuffer>*> usedCameraBuffers{};
        static vector<ConstansBuffer<ShaderCommon::DirLightBuffer>*> usedDirLightBuffers{};
        static vector<ConstansBuffer<ShaderCommon::ModelBuffer>*> usedModelBuffers{};
        usedCameraBuffers.clear();
        usedDirLightBuffers.clear();
        usedModelBuffers.clear();

        // 渲染阶段 1
        // 渲染阴影贴图、几何信息

        // 阴影贴图
        MultiRenderTargets shadowMapTargets{};
        CommandList* shadowMapCommandList;
        {
            // 渲染到阴影贴图
            {
                shadowMapTargets.SetDepthStencil(&g_ShadowMapTexture);
            }

            // 获取一个命令列表
            shadowMapCommandList = CommandListPool::Request(D3D12_COMMAND_LIST_TYPE_DIRECT);
            shadowMapCommandList->Reset();

            // 使阴影贴图为渲染目标
            shadowMapTargets.DispatchTransitionStates(shadowMapCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_DEPTH_WRITE);

            // 设置渲染目标
            shadowMapCommandList->OMSetRenderTargets(&shadowMapTargets);
            shadowMapTargets.DispatchViewportsAndScissor(shadowMapCommandList);

            // 清除渲染目标贴图
            shadowMapTargets.DispatchClear(shadowMapCommandList);
        }

        // 深度贴图
        MultiRenderTargets deferredTargets{};
        CommandList* deferredCommandList;
        {
            deferredTargets.SetDepthStencil(&g_DepthTexture);

            // 获取一个命令列表
            deferredCommandList = CommandListPool::Request(D3D12_COMMAND_LIST_TYPE_DIRECT);
            deferredCommandList->Reset();

            // 使阴影贴图为渲染目标
            deferredTargets.DispatchTransitionStates(deferredCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_DEPTH_WRITE);

            // 设置渲染目标
            deferredCommandList->OMSetRenderTargets(&deferredTargets);
            deferredTargets.DispatchViewportsAndScissor(deferredCommandList);

            // 清除渲染目标贴图
            deferredTargets.DispatchClear(deferredCommandList);
        }

        // 多线程渲染 TODO
        {
            auto cameraList = m_BakedComponents.find(typeid(Camera).hash_code());
            auto dirLightList = m_BakedComponents.find(typeid(DirectionalLight).hash_code());
            auto rendererList = m_BakedComponents.find(typeid(MeshRenderer).hash_code());
            if (cameraList != m_BakedComponents.end() &&
                dirLightList != m_BakedComponents.end() &&
                rendererList != m_BakedComponents.end() &&
                cameraList->second.size() > 0 &&
                dirLightList->second.size() > 0)
            {
                Camera* camera = static_cast<Camera*>(cameraList->second.front());
                DirectionalLight* dirLight = static_cast<DirectionalLight*>(dirLightList->second.front());

                auto camera_WorldToClip = camera->GetProjectionMatrix() * camera->GetViewMatrix();
                auto dirLight_WorldToClip = dirLight->GetLightProjectionMatrix() * dirLight->GetLightViewMatrix();

                for (auto* comRenderer : rendererList->second)
                {
                    MeshRenderer* renderer = static_cast<MeshRenderer*>(comRenderer);

                    auto* modelBuffer = ConstansBufferPool::Request<ShaderCommon::ModelBuffer>();
                    usedModelBuffers.push_back(modelBuffer);

                    renderer->GetTransform().FillModelBuffer(modelBuffer->GetMappingBuffer(), dirLight_WorldToClip);

                    //TODO
                    g_GenDepthMaterial.BindBuffer(0, *(modelBuffer->GetResourceBuffer()));
                    g_GenDepthMaterial.BindBuffer(1, *(modelBuffer->GetResourceBuffer()));

                    renderer->GetMesh()->DispatchDraw(shadowMapCommandList, &g_GenDepthMaterial);


                    modelBuffer = ConstansBufferPool::Request<ShaderCommon::ModelBuffer>();
                    usedModelBuffers.push_back(modelBuffer);

                    renderer->GetTransform().FillModelBuffer(modelBuffer->GetMappingBuffer(), camera_WorldToClip);

                    //TODO
                    g_GenDepthMaterial.BindBuffer(0, *(modelBuffer->GetResourceBuffer()));
                    g_GenDepthMaterial.BindBuffer(1, *(modelBuffer->GetResourceBuffer()));

                    renderer->GetMesh()->DispatchDraw(deferredCommandList, &g_GenDepthMaterial);
                }
            }
        }

        // 将渲染贴图改为着色器资源
        shadowMapTargets.DispatchTransitionStates(shadowMapCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        deferredTargets.DispatchTransitionStates(deferredCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

        shadowMapCommandList->AddOnCompletedEvent([]()
            {
                for (int i = 0; i < usedCameraBuffers.size(); i++)
                    ConstansBufferPool::Restore(&usedCameraBuffers[i]);
                for (int i = 0; i < usedDirLightBuffers.size(); i++)
                    ConstansBufferPool::Restore(&usedDirLightBuffers[i]);
                for (int i = 0; i < usedModelBuffers.size(); i++)
                    ConstansBufferPool::Restore(&usedModelBuffers[i]);
            });

        commandListArray.push_back(shadowMapCommandList);
        commandListArray.push_back(deferredCommandList);
    }
    void Scene::RenderPass2(vector<CommandList*>& commandListArray)
    {
        static vector<ConstansBuffer<ShaderCommon::CameraBuffer>*> usedCameraBuffers{};
        static vector<ConstansBuffer<ShaderCommon::DirLightBuffer>*> usedDirLightBuffers{};
        static vector<ConstansBuffer<ShaderCommon::ModelBuffer>*> usedModelBuffers{};
        usedCameraBuffers.clear();
        usedDirLightBuffers.clear();
        usedModelBuffers.clear();

        // 渲染阶段 2
        // 使用阴影贴图、几何信息

        // 渲染到交换链
        MultiRenderTargets currentRenderTargets{};
        {
            currentRenderTargets.SetRenderTarget(0, &g_RenderRtvTexture);
            currentRenderTargets.SetDepthStencil(&g_DepthTexture);
        }

        // 重置命令列表
        auto* graphicsCommandList = CommandListPool::Request(D3D12_COMMAND_LIST_TYPE_DIRECT);
        graphicsCommandList->Reset();

        // 指示后台缓冲区将用作渲染目标
        currentRenderTargets.DispatchTransitionStates(graphicsCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_DEPTH_WRITE);

        // 设置渲染目标
        graphicsCommandList->OMSetRenderTargets(&currentRenderTargets);
        currentRenderTargets.DispatchViewportsAndScissor(graphicsCommandList);

        // 清除渲染目标贴图
        currentRenderTargets.DispatchClear(graphicsCommandList, true, false);

        // 使用阴影贴图渲染
        {
            auto cameraList = m_BakedComponents.find(typeid(Camera).hash_code());
            auto dirLightList = m_BakedComponents.find(typeid(DirectionalLight).hash_code());
            auto rendererList = m_BakedComponents.find(typeid(MeshRenderer).hash_code());
            if (cameraList != m_BakedComponents.end() && rendererList != m_BakedComponents.end())
            {
                DirectionalLight* dirLight = static_cast<DirectionalLight*>(dirLightList->second.front());

                auto* dirLightBuffer = ConstansBufferPool::Request<ShaderCommon::DirLightBuffer>();
                usedDirLightBuffers.push_back(dirLightBuffer);

                dirLight->FillDirLightBuffer(dirLightBuffer->GetMappingBuffer());

                for (auto* comCamera : cameraList->second)
                {
                    Camera* camera = static_cast<Camera*>(comCamera);

                    auto* cameraBuffer = ConstansBufferPool::Request<ShaderCommon::CameraBuffer>();
                    usedCameraBuffers.push_back(cameraBuffer);

                    auto* cameraMappingBuffer = cameraBuffer->GetMappingBuffer();
                    camera->FillCameraBuffer(cameraMappingBuffer);

                    auto worldSpaceToClipSpace = cameraMappingBuffer->_Camera_Project * cameraMappingBuffer->_Camera_View;

                    for (auto* comRenderer : rendererList->second)
                    {
                        MeshRenderer* renderer = static_cast<MeshRenderer*>(comRenderer);

                        auto* modelBuffer = ConstansBufferPool::Request<ShaderCommon::ModelBuffer>();
                        usedModelBuffers.push_back(modelBuffer);

                        renderer->GetTransform().FillModelBuffer(modelBuffer->GetMappingBuffer(), worldSpaceToClipSpace);

                        renderer->GetMaterial()->BindBuffer(0, *(cameraBuffer->GetResourceBuffer()));
                        renderer->GetMaterial()->BindBuffer(1, *(modelBuffer->GetResourceBuffer()));
                        renderer->GetMaterial()->BindBuffer(2, *(dirLightBuffer->GetResourceBuffer()));

                        renderer->DispatchDraw(graphicsCommandList);
                    }
                }
            }
        }

        // 指示现在将使用后台缓冲区来呈现。
        currentRenderTargets.DispatchTransitionStates(graphicsCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_READ);

        graphicsCommandList->AddOnCompletedEvent([]()
            {
                for (int i = 0; i < usedCameraBuffers.size(); i++)
                    ConstansBufferPool::Restore(&usedCameraBuffers[i]);
                for (int i = 0; i < usedDirLightBuffers.size(); i++)
                    ConstansBufferPool::Restore(&usedDirLightBuffers[i]);
                for (int i = 0; i < usedModelBuffers.size(); i++)
                    ConstansBufferPool::Restore(&usedModelBuffers[i]);
            });

        commandListArray.push_back(graphicsCommandList);
    }
    void Scene::RenderPass3(vector<CommandList*>& commandListArray)
    {
        // 渲染阶段 3

        // 渲染到交换链
        MultiRenderTargets currentRenderTargets{};
        {
            auto& swapChain = *GraphicsManager::GetSwapChain();
            UINT cbbi = swapChain.GetCurrentBackBufferIndex();
            currentRenderTargets.SetRenderTarget(0, swapChain.GetRenderTarget(cbbi));
        }

        // 重置命令列表
        auto* graphicsCommandList = CommandListPool::Request(D3D12_COMMAND_LIST_TYPE_DIRECT);
        graphicsCommandList->Reset();

        // 指示后台缓冲区将用作渲染目标
        currentRenderTargets.DispatchTransitionStates(graphicsCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_DEPTH_WRITE);

        // 设置渲染目标
        graphicsCommandList->OMSetRenderTargets(&currentRenderTargets);
        currentRenderTargets.DispatchViewportsAndScissor(graphicsCommandList);

        // 清除渲染目标贴图
        currentRenderTargets.DispatchClear(graphicsCommandList);

        // 位块传输
        {
            g_BlitMaterial.BindTexture(0, g_RenderRtvTexture);
            g_BlitQuad.DispatchDraw(graphicsCommandList, &g_BlitMaterial);
        }

        // 指示现在将使用后台缓冲区来呈现。
        currentRenderTargets.DispatchTransitionStates(graphicsCommandList, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_DEPTH_READ);

        commandListArray.push_back(graphicsCommandList);
    }
}