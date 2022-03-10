#include "pch.h"

#include "AppMain.h"
#include "CommandList.h"
#include "GraphicsManager.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "GraphicsCommon.h"
#include "GameTime.h"
#include "Input.h"

#include "TextureLoader.h"

#include "PipelineState.h"
#include "GraphicsBuffer.h"
#include "ConstansBuffer.h"
#include "Texture2D.h"
#include "RenderTexture.h"
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
    Shader* g_SampleShader;
    Material* g_SampleMaterial[2];
    Texture2D* g_SampleTexture[2];
    Mesh* g_SampleMesh[2];

    GameObject* g_SampleCameraObject;
    Camera* g_SampleCamera;
    GameObject* g_SampleModelObject[2];

    void SampleScene::Initialize()
    {
        m_Name = _T("SampleScene");

        auto& g_GraphicsCommandList = *CommandListPool::Request(D3D12_COMMAND_LIST_TYPE_DIRECT);
        g_GraphicsCommandList.Reset();

        {
            auto texPath = Application::GetAssetPath();
            texPath.append("Shimarin.png");

            TextureLoader texData;
            texData.LoadTexture2D(texPath);

            g_SampleTexture[0] = &AddGameResource(unique_ptr<Texture2D>(new Texture2D()));
            g_SampleTexture[0]->PlacedCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
            g_SampleTexture[0]->DispatchCopyTextureData(g_GraphicsCommandList, texData.GetDataPointer());
            g_SampleTexture[0]->DispatchTransitionStates(&g_GraphicsCommandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            g_SampleTexture[0]->SetName(L"志摩 凛");

            texPath = Application::GetAssetPath();
            texPath.append(L"云堇.jpg");
            texData.LoadTexture2D(texPath);

            g_SampleTexture[1] = &AddGameResource(unique_ptr<Texture2D>(new Texture2D()));
            g_SampleTexture[1]->PlacedCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
            g_SampleTexture[1]->DispatchCopyTextureData(g_GraphicsCommandList, texData.GetDataPointer());
            g_SampleTexture[1]->DispatchTransitionStates(&g_GraphicsCommandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            g_SampleTexture[1]->SetName(L"云堇");
        }

        {
            ShaderDesc shaderDesc{};
            shaderDesc.m_SemanticFlags = (1 << (int)VertexSemantic::Position) | (1 << (int)VertexSemantic::Normal) | (1 << (int)VertexSemantic::Texcoord);
            shaderDesc.m_ShaderFilePaths[static_cast<int>(ShaderType::VertexShader)] = Application::GetShaderPath().append("SampleTexture_vs.cso");
            shaderDesc.m_ShaderFilePaths[static_cast<int>(ShaderType::PixelShader)] = Application::GetShaderPath().append("SampleTexture_ps.cso");
            shaderDesc.m_CbvCount = 2;
            shaderDesc.m_SrvCount = 2;
            shaderDesc.m_SamplerCount = 2;

            g_SampleShader = &AddGameResource<Shader>();
            g_SampleShader->Create(&shaderDesc);
            g_SampleShader->SetName(L"示例着色器");

            for (int i = 0; i < _countof(g_SampleMaterial); i++)
            {
                g_SampleMaterial[i] = &AddGameResource<Material>();
                g_SampleMaterial[i]->Create(g_SampleShader);
                g_SampleMaterial[i]->SetName(L"示例材质");

                g_SampleMaterial[i]->BindTexture(0, *g_SampleTexture[0]);
                g_SampleMaterial[i]->BindTexture(1, *g_SampleTexture[1]);
                g_SampleMaterial[i]->BindSampler(0, g_SamplerPointMirror);
                g_SampleMaterial[i]->BindSampler(1, g_SamplerLinearBorder);
            }
        }

        {
            g_SampleMesh[0] = &AddGameResource<Mesh>();
            *g_SampleMesh[0] = Mesh::CreateCube();
            g_SampleMesh[0]->SetName(L"Cube");

            g_SampleMesh[1] = &AddGameResource<Mesh>();
            *g_SampleMesh[1] = Mesh::CreateSphere();
            g_SampleMesh[1]->SetName(L"Sphere");
        }

        {
            g_SampleModelObject[0] = &AddGameObject(unique_ptr<GameObject>(new GameObject(L"Test Object")));
            auto& meshRenderer = g_SampleModelObject[0]->AddComponent<MeshRenderer>();
            meshRenderer.BindResource(g_SampleMesh[1], g_SampleMaterial[0], false);
            g_SampleModelObject[0]->GetTransform().LocalScale = Vector3::One * 2.0f;

            g_SampleModelObject[1] = &AddGameObject(unique_ptr<GameObject>(new GameObject(L"Test Object2")));
            auto& meshRenderer2 = g_SampleModelObject[1]->AddComponent<MeshRenderer>();
            meshRenderer2.BindResource(g_SampleMesh[1], g_SampleMaterial[1], false);
            g_SampleModelObject[1]->GetTransform().LocalScale = Vector3::One;
            g_SampleModelObject[1]->GetTransform().LocalPosition = Vector3::Up * 1.5f;

            g_SampleCameraObject = &AddGameObject(unique_ptr<GameObject>(new GameObject(L"Main Camera")));
            auto& camera = g_SampleCameraObject->AddComponent<Camera>();
            g_SampleCameraObject->GetTransform().LocalPosition = Vector3(0.0f, 5.0f, -5.0f);
            g_SampleCameraObject->GetTransform().LookAt(g_SampleModelObject[0]->GetTransform().GetPosition());
        }

        GraphicsManager::GetGraphicsCommandQueue()->ExecuteCommandLists(&g_GraphicsCommandList);


        // 等待命令列表执行。 
        GraphicsManager::GetGraphicsCommandQueue()->WaitForQueueCompleted();
        GraphicsManager::GetComputeCommandQueue()->WaitForQueueCompleted();
        GraphicsManager::GetCopyCommandQueue()->WaitForQueueCompleted();
    }

    void SampleScene::ExecuteUpdate()
    {
        int changeIndex = 0;
        if (Input::KeyState(KeyCode::LeftControl))
        {
            changeIndex = 1;
        }

        // 测试切换采样器
        if (Input::KeyDown(KeyCode::D1))
        {
            static D3D12_SAMPLER_DESC* samplers[] =
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
            useSamplerIndex = Math::Repeat(useSamplerIndex + 1, 0, _countof(samplers));
            g_SampleMaterial[changeIndex]->BindSampler(0, *samplers[useSamplerIndex]);
        }
        // 测试切换贴图
        if (Input::KeyDown(KeyCode::D2))
        {
            static int useIndex = 0;
            useIndex = Math::Repeat(useIndex + 1, 0, _countof(g_SampleTexture));
            g_SampleMaterial[changeIndex]->BindTexture(0, *g_SampleTexture[useIndex]);
        }
        // 测试切换模型
        if (Input::KeyDown(KeyCode::D3))
        {
            static int useIndex = 0;
            useIndex = Math::Repeat(useIndex + 1, 0, _countof(g_SampleMesh));
            auto* meshRenderer = g_SampleModelObject[changeIndex]->GetComponent<MeshRenderer>();
            meshRenderer->BindResource(g_SampleMesh[useIndex], meshRenderer->GetMaterial());
        }
        // 测试切换渲染模式
        if (Input::KeyDown(KeyCode::D4))
        {
            static int fillMode = 1;
            g_SampleMaterial[changeIndex]->SetFillMode((D3D12_FILL_MODE)Math::Repeat(fillMode++, 2, 4));
        }

        // 测试模型变换
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
            pos = g_SampleCameraObject->GetTransform().GetRotation() * pos;

            if (Input::KeyState(KeyCode::Space))
                pos.y += 1.0f;
            if (Input::KeyState(KeyCode::C))
                pos.y -= 1.0f;
            pos *= Time::GetDeltaTime() * 10.0f;
            g_SampleCameraObject->GetTransform().LocalPosition += pos;

            Vector3 rot{};
            if (Input::MouseButtonState(MouseButtonType::LeftButton))
            {
                Vector2 deltaPos = Input::GetMouseDeltaPos();
                rot.y = deltaPos.x;
                rot.x = -deltaPos.y;
                rot.y *= -1.0f;
            }
            auto cameraEulerAngles = g_SampleCameraObject->GetTransform().GetRotation().GetEulerAngles();
            cameraEulerAngles += rot * 0.1f;

            if (Input::KeyDown(KeyCode::R))
            {
                g_SampleCameraObject->GetTransform().LocalPosition = Vector3(0.0f, 0.0f, -10.0f);
                cameraEulerAngles = Vector3::Zero;
                g_SampleCameraObject->GetTransform().LocalScale = Vector3::One;
            }
            g_SampleCameraObject->GetTransform().SetEulerAngles(cameraEulerAngles);

            auto modelEulerAngles = g_SampleModelObject[0]->GetTransform().GetEulerAngles();
            modelEulerAngles.y += Time::GetDeltaTime() * 90.0f;
            g_SampleModelObject[0]->GetTransform().SetEulerAngles(modelEulerAngles);
            if (Input::KeyDown(KeyCode::Back))
            {
                TRACE("Input::KeyDown(KeyCode::Back)");
                //g_SampleModelObject[0]->Destroy();
                //g_SampleModelObject[0] = nullptr;
                g_SampleModelObject[0]->RemoveComponent<MeshRenderer>();
            }
        }
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