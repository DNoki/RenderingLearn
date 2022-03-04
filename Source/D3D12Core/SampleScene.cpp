#include "pch.h"

#include "AppMain.h"
#include "CommandList.h"
#include "GraphicsCore.h"
#include "GraphicsCommon.h"
#include "GameTime.h"
#include "Input.h"

#include "TextureLoader.h"

#include "PipelineState.h"
#include "GraphicsBuffer.h"
#include "ConstansBuffer.h"
#include "Texture2D.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"

#include "SampleScene.h"

using namespace std;
using namespace Graphics;
using namespace Application;

namespace Game
{
    Texture2D t_DefaultTexture[2];
    Shader g_SampleShader;
    Material g_SampleMaterial;
    Mesh g_SampleMesh;

    struct MVPBuffer
    {
        Matrix4x4 m_P;
        Matrix4x4 m_V;
        Matrix4x4 m_M;
        Matrix4x4 m_IT_M;
        Matrix4x4 m_MVP;
    };
    ConstansBuffer<MVPBuffer> g_SampleConstansBuffer;

    void SampleScene::Initialize()
    {
        m_Name = "SampleScene";

        auto& g_GraphicsCommandList = *CommandListPool::Request(D3D12_COMMAND_LIST_TYPE_DIRECT);
        g_GraphicsCommandList.Reset();

        {
            auto texPath = Application::GetAssetPath();
            texPath.append("Shimarin.png");

            TextureLoader texData;
            texData.LoadTexture2D(texPath);

            t_DefaultTexture[0].PlacedCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
            t_DefaultTexture[0].DispatchCopyTextureData(g_GraphicsCommandList, texData.GetDataPointer());
            t_DefaultTexture[0].DispatchTransitionStates(&g_GraphicsCommandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

            texPath = Application::GetAssetPath();
            texPath.append(L"云堇.jpg");
            texData.LoadTexture2D(texPath);

            t_DefaultTexture[1].PlacedCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
            t_DefaultTexture[1].DispatchCopyTextureData(g_GraphicsCommandList, texData.GetDataPointer());
            t_DefaultTexture[1].DispatchTransitionStates(&g_GraphicsCommandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }

        {
            g_SampleShader.Create();
            g_SampleMaterial.Create(&g_SampleShader);

            g_SampleMaterial.BindTexture(0, t_DefaultTexture[1]);
            g_SampleMaterial.BindSampler(&g_SamplerPointMirror);
        }

        {
            g_SampleConstansBuffer.PlacedCreate();
            g_SampleMaterial.BindBuffer(1, *g_SampleConstansBuffer.GetResourceBuffer());
        }

        g_SampleMesh = (Mesh::CreateCube());

        {
            auto& sampleObj = AddGameObject(unique_ptr<GameObject>(new GameObject("Test Object")));
            auto& meshRenderer = sampleObj.AddComponent<MeshRenderer>();
            meshRenderer.Create(&g_SampleMesh, &g_SampleMaterial, false);
            sampleObj.GetTransform().LocalScale = Vector3::One * 2.0f;

            auto& cameraObj = AddGameObject(unique_ptr<GameObject>(new GameObject("Main Camera")));
            auto& camera = cameraObj.AddComponent<Camera>();
            cameraObj.GetTransform().LocalPosition = Vector3(0.0f, 5.0f, -5.0f);
            cameraObj.GetTransform().LookAt(sampleObj.GetTransform().GetPosition());
        }

        GraphicsManager::GetGraphicsCommandQueue()->ExecuteCommandLists(&g_GraphicsCommandList);


        // 等待命令列表执行。 
        GraphicsManager::GetGraphicsCommandQueue()->WaitForQueueCompleted();
        GraphicsManager::GetComputeCommandQueue()->WaitForQueueCompleted();
        GraphicsManager::GetCopyCommandQueue()->WaitForQueueCompleted();
    }

    void SampleScene::ExecuteUpdate()
    {
        auto* sampleObj = FindGameObject("Test Object");
        auto* cameraObj = FindGameObject("Main Camera");

        static DescriptorHandle* samplers[] =
        {
            &g_SamplerPointBorder,
            &g_SamplerLinearBorder,
            &g_SamplerPointClamp,
            &g_SamplerLinearClamp,
            &g_SamplerPointWarp,
            &g_SamplerLinearWarp,
            &g_SamplerPointMirror,
            &g_SamplerLinearMirror,
        };
        static int useSamplerIndex = 0;
        if (Input::KeyDown(KeyCode::Space))
        {
            useSamplerIndex = Math::Repeat(useSamplerIndex + 1, 0, _countof(samplers));
            g_SampleMaterial.BindSampler(samplers[useSamplerIndex]);
        }

        {
            Vector3 pos{};
            if (Input::KeyState(KeyCode::W))
                pos.z += 1.0f;
            if (Input::KeyState(KeyCode::S))
                pos.z -= 1.0f;
            if (Input::KeyState(KeyCode::A))
                pos.x -= 1.0f;
            if (Input::KeyState(KeyCode::D))
                pos.x += 1.0f;
            pos = cameraObj->GetTransform().GetRotation() * pos;

            if (Input::KeyState(KeyCode::Space))
                pos.y += 1.0f;
            if (Input::KeyState(KeyCode::C))
                pos.y -= 1.0f;
            pos *= Time::GetDeltaTime() * 10.0f;
            cameraObj->GetTransform().LocalPosition += pos;

            Vector3 rot{};
            if (Input::MouseButtonState(MouseButtonType::LeftButton))
            {
                Vector2 deltaPos = Input::GetMouseDeltaPos();
                rot.y = deltaPos.x;
                rot.x = -deltaPos.y;
                rot.y *= -1.0f;
            }
            cameraObj->GetTransform().LocalEulerAngles += rot * 0.1f * Math::Deg2Rad;
            //TRACE(L"%f, %f\n", g_CameraTrans.LocalEulerAngles.x, g_CameraTrans.LocalEulerAngles.y);

            if (Input::KeyDown(KeyCode::R))
            {
                cameraObj->GetTransform().LocalPosition = Vector3(0.0f, 0.0f, -10.0f);
                cameraObj->GetTransform().LocalEulerAngles = Vector3::Zero;
                cameraObj->GetTransform().LocalScale = Vector3::One;
            }

            sampleObj->GetTransform().LocalEulerAngles.y += Time::GetDeltaTime() * 90.0f * Math::Deg2Rad;
        }

        if (Input::KeyDown(KeyCode::Enter))
        {
            static int fillMode = 1;
            g_SampleMaterial.SetFillMode((D3D12_FILL_MODE)Math::Repeat(fillMode++, 2, 4));
        }
    }
    void SampleScene::ExecuteRender()
    {
        auto* sampleObj = FindGameObject("Test Object");
        auto* cameraObj = FindGameObject("Main Camera");

        auto* graphicsCommandQueue = GraphicsManager::GetGraphicsCommandQueue();
        auto& swapChain = *GraphicsManager::GetSwapChain();


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
                graphicsCommandList->ClearRenderTargetView(swapChain.GetRtvDescHandle(cbbi), clearColor, 0, nullptr);
                graphicsCommandList->ClearDepthStencilView(swapChain.GetDsvDescHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
            }

            //SampleDraw(graphicsCommandList);
            {
                auto& camera = *cameraObj->GetComponent<Camera>();
                Matrix4x4 pers = camera.GetProjectionMatrix();

                Matrix4x4 view = camera.GetViewMatrix();

                Matrix4x4 model = sampleObj->GetTransform().GetLocalToWorldMatrix();

                g_SampleConstansBuffer.GetMappingBuffer()->m_P = pers;
                g_SampleConstansBuffer.GetMappingBuffer()->m_V = view;
                g_SampleConstansBuffer.GetMappingBuffer()->m_M = model;
                g_SampleConstansBuffer.GetMappingBuffer()->m_IT_M = model.Inverse().Transpose();
                g_SampleConstansBuffer.GetMappingBuffer()->m_MVP = pers * view * model;

                sampleObj->GetComponent<MeshRenderer>()->DispatchDraw(graphicsCommandList);
            }

            // 指示现在将使用后台缓冲区来呈现。
            for (UINT i = 0; i < currentRenderTargets.GetRenderTargetCount(); i++)
            {
                currentRenderTargets.GetRenderTargets(i)->DispatchTransitionStates(graphicsCommandList, D3D12_RESOURCE_STATE_PRESENT);
            }
        }
        // 执行命令列表
        graphicsCommandQueue->ExecuteCommandLists(graphicsCommandList);

        // 呈现帧。
        CHECK_HRESULT(swapChain.GetD3D12SwapChain()->Present(1, 0));

        graphicsCommandQueue->WaitForQueueCompleted();

        TimeSystem::AddFrameCompleted(); // 帧完成信号+1
        TimeSystem::AddSwapFrameCompleted(); // 已交换帧数量+1

        SetWindowTitle(Format(g_TitleFormat.c_str(), WINDOW_TITLE, g_TitleGPU.c_str(), Time::GetAverageFPS()));
    }

#if 0
    Shader g_SampleShader;

    Texture2D t_DefaultTexture[2];

    struct MVPBuffer
    {
        DirectX::XMFLOAT4X4 m_P;
        DirectX::XMFLOAT4X4 m_V;
        DirectX::XMFLOAT4X4 m_M;
        DirectX::XMFLOAT4X4 m_IT_M;
        DirectX::XMFLOAT4X4 m_MVP;
    };
    UploadBuffer g_MvpBufferRes;
    MVPBuffer* g_MVPBuffer;

    unique_ptr<GameObject> g_SampleObj;
    unique_ptr<GameObject> g_CameraObj;

    Mesh g_SampleMesh;
    Material g_SampleMaterial;


    void InitShader()
    {
        g_SampleShader.Create();
        g_SampleMaterial.Create(&g_SampleShader);
    }
    void InitTexture2D()
    {
        auto& g_GraphicsCommandList = *CommandListPool::Request(D3D12_COMMAND_LIST_TYPE_DIRECT);
        g_GraphicsCommandList.Reset();

        auto texPath = Application::GetAssetPath();
        texPath.append("Shimarin.png");

        // 创建Checker贴图
        //t_DefaultTexture.GenerateChecker(t_SampleResDescHeap.GetDescriptorHandle(0), 256, 256);

        TextureLoader texData;
        texData.LoadTexture2D(texPath);

        //g_TestTex2D.DirectCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
        t_DefaultTexture[0].PlacedCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
        t_DefaultTexture[0].DispatchCopyTextureData(g_GraphicsCommandList, texData.GetDataPointer());
        t_DefaultTexture[0].DispatchTransitionStates(&g_GraphicsCommandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        texPath = Application::GetAssetPath();
        texPath.append(L"云堇.jpg");
        texData.LoadTexture2D(texPath);

        t_DefaultTexture[1].PlacedCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
        t_DefaultTexture[1].DispatchCopyTextureData(g_GraphicsCommandList, texData.GetDataPointer());
        t_DefaultTexture[1].DispatchTransitionStates(&g_GraphicsCommandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        g_SampleMaterial.BindTexture(0, t_DefaultTexture[1]);
        g_SampleMaterial.BindSampler(&g_SamplerPointMirror);


        auto constantBufferSize = UINT_UPPER(sizeof(MVPBuffer), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
        //auto constantBufferSize = sizeof(MVPBuffer);
        {
            g_MvpBufferRes.PlacedCreate(constantBufferSize);
            g_MvpBufferRes.Map(0, reinterpret_cast<void**>(&g_MVPBuffer));
            g_MVPBuffer->m_MVP = Matrix4x4::Identity;
            //g_MvpBufferRes.DispatchCopyBuffer(g_GraphicsCommandList, &mvp);
            g_SampleMaterial.BindBuffer(1, g_MvpBufferRes);
        }

        GraphicsManager::GetGraphicsCommandQueue()->ExecuteCommandLists(&g_GraphicsCommandList);
    }
    void InitMesh()
    {
        g_SampleMesh = (Mesh::CreateCube());
    }

    void InitCommandListBundle()
    {
        g_SampleObj.reset(new GameObject("Test Object"));
        auto& meshRenderer = g_SampleObj->AddComponent<MeshRenderer>();
        meshRenderer.Create(&g_SampleMesh, &g_SampleMaterial, false);
        g_SampleObj->GetTransform().LocalScale = Vector3::One * 2.0f;

        g_CameraObj.reset(new GameObject("Main Camera"));
        auto& camera = g_CameraObj->AddComponent<Camera>();
        g_CameraObj->GetTransform().LocalPosition = Vector3(0.0f, 5.0f, -5.0f);
        g_CameraObj->GetTransform().LookAt(g_SampleObj->GetTransform().GetPosition());
    }


    void SampleDraw(const CommandList* g_GraphicsCommandList)
    {
        static DescriptorHandle* samplers[] =
        {
            &g_SamplerPointBorder,
            &g_SamplerLinearBorder,
            &g_SamplerPointClamp,
            &g_SamplerLinearClamp,
            &g_SamplerPointWarp,
            &g_SamplerLinearWarp,
            &g_SamplerPointMirror,
            &g_SamplerLinearMirror,
        };
        static int useSamplerIndex = 0;
        if (Input::KeyDown(KeyCode::Space))
        {
            useSamplerIndex = Math::Repeat(useSamplerIndex + 1, 0, _countof(samplers));
            g_SampleMaterial.BindSampler(samplers[useSamplerIndex]);
        }

        {
            {
                Vector3 pos{};
                if (Input::KeyState(KeyCode::W))
                    pos.z += 1.0f;
                if (Input::KeyState(KeyCode::S))
                    pos.z -= 1.0f;
                if (Input::KeyState(KeyCode::A))
                    pos.x -= 1.0f;
                if (Input::KeyState(KeyCode::D))
                    pos.x += 1.0f;
                pos = g_CameraObj->GetTransform().GetRotation() * pos;

                if (Input::KeyState(KeyCode::Space))
                    pos.y += 1.0f;
                if (Input::KeyState(KeyCode::C))
                    pos.y -= 1.0f;
                pos *= Time::GetDeltaTime() * 10.0f;
                g_CameraObj->GetTransform().LocalPosition += pos;

                Vector3 rot{};
                if (Input::MouseButtonState(MouseButtonType::LeftButton))
                {
                    Vector2 deltaPos = Input::GetMouseDeltaPos();
                    rot.y = deltaPos.x;
                    rot.x = -deltaPos.y;
                    rot.y *= -1.0f;
                }
                g_CameraObj->GetTransform().LocalEulerAngles += rot * 0.1f * Math::Deg2Rad;
                //TRACE(L"%f, %f\n", g_CameraTrans.LocalEulerAngles.x, g_CameraTrans.LocalEulerAngles.y);

                if (Input::KeyDown(KeyCode::R))
                {
                    g_CameraObj->GetTransform().LocalPosition = Vector3(0.0f, 0.0f, -10.0f);
                    g_CameraObj->GetTransform().LocalEulerAngles = Vector3::Zero;
                    g_CameraObj->GetTransform().LocalScale = Vector3::One;
                }

                g_SampleObj->GetTransform().LocalEulerAngles.y += Time::GetDeltaTime() * 90.0f * Math::Deg2Rad;
            }

            auto& camera = *g_CameraObj->GetComponent<Camera>();
            Matrix4x4 pers = camera.GetProjectionMatrix();

            Matrix4x4 view = camera.GetViewMatrix();

            Matrix4x4 model = g_SampleObj->GetTransform().GetLocalToWorldMatrix();

            g_MVPBuffer->m_P = pers;
            g_MVPBuffer->m_V = view;
            g_MVPBuffer->m_M = model;
            g_MVPBuffer->m_IT_M = model.Inverse().Transpose();
            g_MVPBuffer->m_MVP = pers * view * model;
        }

        // 使用三角形带渲染，这是最快的绘制矩形的方式，是渲染UI的核心方法
        //{
        //    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        //    commandList->IASetVertexBuffers(0, 1, g_SampleVBV.GetVBV());
        //    commandList->DrawInstanced(4, 1, 0, 0);
        //}


        if (Input::KeyDown(KeyCode::Enter))
        {
            static int fillMode = 1;
            g_SampleMaterial.SetFillMode((D3D12_FILL_MODE)Math::Repeat(fillMode++, 2, 4));
        }

        //g_SampleRenderer.DispatchDraw(g_GraphicsCommandList);
        g_SampleObj->GetComponent<MeshRenderer>()->DispatchDraw(g_GraphicsCommandList);
    }
#endif
}