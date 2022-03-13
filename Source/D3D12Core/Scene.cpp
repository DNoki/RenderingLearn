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

#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"

#include "Scene.h"

using namespace std;
using namespace Graphics;
using namespace Application;

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
        auto& swapChain = *GraphicsManager::GetSwapChain();

#if 1
        // 获取渲染目标
        MultiRenderTargets currentRenderTargets{};
        {
            UINT cbbi = swapChain.GetCurrentBackBufferIndex();
            currentRenderTargets.SetRenderTarget(0, swapChain.GetRenderTarget(cbbi), swapChain.GetRtvDescHandle(cbbi));
            currentRenderTargets.SetDepthStencil(swapChain.GetDepthStencil(), swapChain.GetDsvDescHandle());
        }

        vector<CommandList*> commandListArray{};

        // 渲染前处理
        {
            // 获取一个命令列表
            auto* graphicsCommandList = CommandListPool::Request(D3D12_COMMAND_LIST_TYPE_DIRECT);
            graphicsCommandList->Reset();

            // 指示后台缓冲区将用作渲染目标。
            for (UINT i = 0; i < currentRenderTargets.GetRenderTargetCount(); i++)
            {
                currentRenderTargets.GetRenderTargets(i)->DispatchTransitionStates(graphicsCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
            }
            // 设置渲染目标
            graphicsCommandList->OMSetRenderTargets(&currentRenderTargets);

            // 设置必要的状态。
            {
                // 设置视口大小
                auto viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(currentRenderTargets.GetWidth()), static_cast<float>(currentRenderTargets.GetHeight()));
                graphicsCommandList->RSSetViewports(1, &viewport);
                // 设置剪切大小
                auto scissorRect = CD3DX12_RECT(0, 0, currentRenderTargets.GetWidth(), currentRenderTargets.GetHeight());
                graphicsCommandList->RSSetScissorRects(1, &scissorRect);
            }

            // 清除渲染目标贴图
            {
                const Color clearColor = Color(0.0f, 0.2f, 0.4f, 1.0f);
                for (UINT i = 0; i < currentRenderTargets.GetRenderTargetCount(); i++)
                {
                    graphicsCommandList->ClearRenderTargetView(currentRenderTargets.GetRtvDescriptors()[i], clearColor, 0, nullptr);
                }
                graphicsCommandList->ClearDepthStencilView(swapChain.GetDsvDescHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
            }

            commandListArray.push_back(graphicsCommandList);
        }

        // 多线程渲染 TODO
        {
            auto cameraList = m_BakedComponents.find(typeid(Camera).hash_code());
            auto rendererList = m_BakedComponents.find(typeid(MeshRenderer).hash_code());
            if (cameraList != m_BakedComponents.end())
            {
                for (auto* comCamera : cameraList->second)
                {
                    Camera* camera = static_cast<Camera*>(comCamera);
                    camera->RefleshCameraBuffer();
                    auto* cameraBuffer = camera->GetCameraBuffer();
                    auto* cameraMappingBuffer = cameraBuffer->GetMappingBuffer();

                    if (rendererList != m_BakedComponents.end())
                    {
                        for (auto* comRenderer : rendererList->second)
                        {
                            MeshRenderer* renderer = static_cast<MeshRenderer*>(comRenderer);
                            renderer->GetTransform().RefleshTransformBuffer(cameraMappingBuffer->m_Project * cameraMappingBuffer->m_View);

                            renderer->GetMaterial()->BindBuffer(0, *(cameraBuffer->GetResourceBuffer()));
                            renderer->GetMaterial()->BindBuffer(1, *(renderer->GetTransform().GetTransformBuffer()->GetResourceBuffer()));

                            {
                                // 获取一个命令列表
                                auto* graphicsCommandList = CommandListPool::Request(D3D12_COMMAND_LIST_TYPE_DIRECT);
                                graphicsCommandList->Reset();

                                // 设置渲染目标
                                graphicsCommandList->OMSetRenderTargets(&currentRenderTargets);

                                // 设置必要的状态。
                                {
                                    // 设置视口大小
                                    auto viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(currentRenderTargets.GetWidth()), static_cast<float>(currentRenderTargets.GetHeight()));
                                    graphicsCommandList->RSSetViewports(1, &viewport);
                                    // 设置剪切大小
                                    auto scissorRect = CD3DX12_RECT(0, 0, currentRenderTargets.GetWidth(), currentRenderTargets.GetHeight());
                                    graphicsCommandList->RSSetScissorRects(1, &scissorRect);
                                }

                                renderer->DispatchDraw(graphicsCommandList);

                                commandListArray.push_back(graphicsCommandList);
                            }
                        }
                    }
                }
            }
        }

        // 渲染后处理
        {
            // 获取一个命令列表
            auto* graphicsCommandList = CommandListPool::Request(D3D12_COMMAND_LIST_TYPE_DIRECT);
            graphicsCommandList->Reset();

            // 指示现在将使用后台缓冲区来呈现。
            for (UINT i = 0; i < currentRenderTargets.GetRenderTargetCount(); i++)
            {
                currentRenderTargets.GetRenderTargets(i)->DispatchTransitionStates(graphicsCommandList, D3D12_RESOURCE_STATE_PRESENT);
            }

            commandListArray.push_back(graphicsCommandList);
        }

        // 执行命令列表
        GraphicsManager::GetGraphicsCommandQueue()->ExecuteCommandLists(commandListArray.data(), static_cast<UINT>(commandListArray.size()));

#else
        // 重置命令列表
        auto* graphicsCommandList = CommandListPool::Request(D3D12_COMMAND_LIST_TYPE_DIRECT);
        graphicsCommandList->Reset();

        // 渲染
        // 填充命令列表
        {
            // 获取当前后台缓冲索引
            UINT cbbi = swapChain.GetCurrentBackBufferIndex();

            // 以交换链为渲染目标
            MultiRenderTargets currentRenderTargets{};
            currentRenderTargets.SetRenderTarget(0, swapChain.GetRenderTarget(cbbi), swapChain.GetRtvDescHandle(cbbi));
            currentRenderTargets.SetDepthStencil(swapChain.GetDepthStencil(), swapChain.GetDsvDescHandle());

            // 设置必要的状态。
            {
                // 设置视口大小
                auto viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(currentRenderTargets.GetWidth()), static_cast<float>(currentRenderTargets.GetHeight()));
                graphicsCommandList->RSSetViewports(1, &viewport);
                // 设置剪切大小
                auto scissorRect = CD3DX12_RECT(0, 0, currentRenderTargets.GetWidth(), currentRenderTargets.GetHeight());
                graphicsCommandList->RSSetScissorRects(1, &scissorRect);
            }

            // 指示后台缓冲区将用作渲染目标。
            for (UINT i = 0; i < currentRenderTargets.GetRenderTargetCount(); i++)
            {
                currentRenderTargets.GetRenderTargets(i)->DispatchTransitionStates(graphicsCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
            }
            // 设置渲染目标
            graphicsCommandList->OMSetRenderTargets(&currentRenderTargets);

            // 清除渲染目标贴图
            {
                const Color clearColor = Color(0.0f, 0.2f, 0.4f, 1.0f);
                for (UINT i = 0; i < currentRenderTargets.GetRenderTargetCount(); i++)
                {
                    graphicsCommandList->ClearRenderTargetView(currentRenderTargets.GetRtvDescriptors()[i], clearColor, 0, nullptr);
                }
                graphicsCommandList->ClearDepthStencilView(swapChain.GetDsvDescHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
            }

            // SampleDraw
            {
                auto cameraList = m_BakedComponents.find(typeid(Camera).hash_code());
                auto rendererList = m_BakedComponents.find(typeid(MeshRenderer).hash_code());
                if (cameraList != m_BakedComponents.end())
                {
                    for (auto* comCamera : cameraList->second)
                    {
                        Camera* camera = static_cast<Camera*>(comCamera);
                        camera->RefleshCameraBuffer();
                        auto* cameraBuffer = camera->GetCameraBuffer();
                        auto* cameraMappingBuffer = cameraBuffer->GetMappingBuffer();

                        if (rendererList != m_BakedComponents.end())
                        {
                            for (auto* comRenderer : rendererList->second)
                            {
                                MeshRenderer* renderer = static_cast<MeshRenderer*>(comRenderer);
                                renderer->GetTransform().RefleshTransformBuffer(cameraMappingBuffer->m_Project * cameraMappingBuffer->m_View);

                                renderer->GetMaterial()->BindBuffer(0, *(cameraBuffer->GetResourceBuffer()));
                                renderer->GetMaterial()->BindBuffer(1, *(renderer->GetTransform().GetTransformBuffer()->GetResourceBuffer()));

                                renderer->DispatchDraw(graphicsCommandList);
                            }
                        }
                    }
                }
            }

            // 指示现在将使用后台缓冲区来呈现。
            for (UINT i = 0; i < currentRenderTargets.GetRenderTargetCount(); i++)
            {
                currentRenderTargets.GetRenderTargets(i)->DispatchTransitionStates(graphicsCommandList, D3D12_RESOURCE_STATE_PRESENT);
            }
        }
        // 执行命令列表
        GraphicsManager::GetGraphicsCommandQueue()->ExecuteCommandLists(graphicsCommandList);
#endif

        // 呈现帧。
        CHECK_HRESULT(swapChain.GetD3D12SwapChain()->Present(1, 0));

        GraphicsManager::GetGraphicsCommandQueue()->WaitForQueueCompleted();

        TimeSystem::AddFrameCompleted(); // 帧完成信号+1
        TimeSystem::AddSwapFrameCompleted(); // 已交换帧数量+1

        SetWindowTitle(Format(g_TitleFormat.c_str(), WINDOW_TITLE, g_TitleGPU.c_str(), Time::GetAverageFPS()));
    }
}