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
#include "Light.h"

#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"

#include "SampleScene.h"

using namespace std;
using namespace Graphics;
using namespace Application;
using namespace Resources;

namespace Game
{
    Shader* g_SampleShader;
    Material* g_SampleMaterial[2];
    Texture2D* g_SampleTexture[2];
    Mesh* g_SampleMesh[2];

    GameObject* g_SampleCameraObject;
    Camera* g_SampleCamera;
    GameObject* g_SampleLightObject;
    DirectionalLight* g_SampleLight;
    GameObject* g_SampleModelObject[2];

    RenderTargetTexture g_RenderTexture;
    DepthStencilTexture g_ShadowMapTexture;

    void SampleScene::Initialize()
    {
        m_Name = _T("SampleScene");

        auto* graphicsCommandList = CommandListPool::Request(D3D12_COMMAND_LIST_TYPE_DIRECT);
        graphicsCommandList->Reset();

        bool isUseBundle = false; // TODO 完善捆绑包

        {
            auto texPath = Application::GetAssetPath();
            texPath.append("Shimarin.png");

            TextureLoader texData;
            texData.LoadTexture2D(texPath);

            g_SampleTexture[0] = &AddGameResource(unique_ptr<Texture2D>(new Texture2D()));
            g_SampleTexture[0]->PlacedCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
            g_SampleTexture[0]->DispatchCopyTextureData(*graphicsCommandList, texData.GetDataPointer());
            g_SampleTexture[0]->DispatchTransitionStates(graphicsCommandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            g_SampleTexture[0]->SetName(L"志摩 凛");

            texPath = Application::GetAssetPath();
            texPath.append(L"云堇.jpg");
            texData.LoadTexture2D(texPath);

            g_SampleTexture[1] = &AddGameResource(unique_ptr<Texture2D>(new Texture2D()));
            g_SampleTexture[1]->PlacedCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
            g_SampleTexture[1]->DispatchCopyTextureData(*graphicsCommandList, texData.GetDataPointer());
            g_SampleTexture[1]->DispatchTransitionStates(graphicsCommandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            g_SampleTexture[1]->SetName(L"云堇");
        }

        {
            ShaderDesc shaderDesc{};
            shaderDesc.m_SemanticFlags = (1 << (int)VertexSemantic::Position) | (1 << (int)VertexSemantic::Normal) | (1 << (int)VertexSemantic::Texcoord);
            shaderDesc.m_ShaderFilePaths[static_cast<int>(ShaderType::VertexShader)] = Application::GetShaderPath().append("SampleTexture_vs.cso");
            shaderDesc.m_ShaderFilePaths[static_cast<int>(ShaderType::PixelShader)] = Application::GetShaderPath().append("SampleTexture_ps.cso");
            shaderDesc.m_CbvCount = 3;
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
            meshRenderer.BindResource(g_SampleMesh[1], g_SampleMaterial[0], isUseBundle);
            g_SampleModelObject[0]->GetTransform().m_LocalScale = Vector3::One * 2.0f;
            g_SampleModelObject[0]->GetTransform().LookAt(Vector3::One);

            g_SampleModelObject[1] = &AddGameObject(unique_ptr<GameObject>(new GameObject(L"Test Object2")));
            auto& meshRenderer2 = g_SampleModelObject[1]->AddComponent<MeshRenderer>();
            meshRenderer2.BindResource(g_SampleMesh[0], g_SampleMaterial[1], isUseBundle);
            g_SampleModelObject[1]->GetTransform().m_LocalScale = Vector3::One * 0.5f;
            g_SampleModelObject[1]->GetTransform().m_LocalPosition = Vector3::Up * 0.75f;
            g_SampleModelObject[1]->GetTransform().m_LocalRotation = Quaternion::CreateFromEulerAngles(10, 23, 30);
            g_SampleModelObject[1]->GetTransform().SetParent(&g_SampleModelObject[0]->GetTransform(), false);

            g_SampleCameraObject = &AddGameObject(unique_ptr<GameObject>(new GameObject(L"Main Camera")));
            auto& camera = g_SampleCameraObject->AddComponent<Camera>();
            g_SampleCameraObject->GetTransform().m_LocalPosition = Vector3(0.0f, 5.0f, -5.0f);
            g_SampleCameraObject->GetTransform().LookAt(g_SampleModelObject[0]->GetTransform().GetPosition());
        }

        {
            auto& quad = AddGameResource<Mesh>();
            quad = Mesh::CreateQuad();
            quad.SetName(L"Quad");

            auto& groundObj = AddGameObject(unique_ptr<GameObject>(new GameObject(L"Ground")));
            auto& groundMeshRenderer = groundObj.AddComponent<MeshRenderer>();
            groundMeshRenderer.BindResource(&quad, g_SampleMaterial[0], isUseBundle);
            groundObj.GetTransform().m_LocalScale = Vector3::One * 20.0f;
            groundObj.GetTransform().Rotate(Vector3(90.0f, 0.0f, 0.0f));
        }

        // 创建渲染目标贴图
        {
            g_RenderTexture.PlacedCreate(DXGI_FORMAT_R8G8B8A8_UNORM, 2048, 2048, Color(0.0f, 0.2f, 0.4f, 1.0f));
            g_RenderTexture.SetName(L"ShadowMapRenderTexture");

            g_ShadowMapTexture.PlacedCreate(DXGI_FORMAT_D32_FLOAT, 2048, 2048);
            g_ShadowMapTexture.SetName(L"ShadowMap");
        }

        {
            g_SampleLightObject = &AddGameObject(unique_ptr<GameObject>(new GameObject(L"DirectionalLight")));
            g_SampleLight = &g_SampleLightObject->AddComponent<DirectionalLight>();
            g_SampleLight->GetTransform().LookAt(-Vector3::One);
        }

        GraphicsManager::GetGraphicsCommandQueue()->ExecuteCommandLists(&graphicsCommandList);


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
            g_SampleCameraObject->GetTransform().Translate(Time::GetDeltaTime() * 10.0f * pos);

            pos = Vector3::Zero;
            if (Input::KeyState(KeyCode::Space))
                pos.y += 1.0f;
            if (Input::KeyState(KeyCode::C))
                pos.y -= 1.0f;
            g_SampleCameraObject->GetTransform().Translate(Time::GetDeltaTime() * 10.0f * pos, true);

            Vector3 rot{};
            if (Input::MouseButtonState(MouseButtonType::LeftButton))
            {
                Vector2 deltaPos = Input::GetMouseDeltaPos();
                rot.y = deltaPos.x;
                rot.x = -deltaPos.y;
                rot.y *= -1.0f;
                rot *= 0.1f;
                auto cameraRot = g_SampleCameraObject->GetTransform().GetEulerAngles();
                cameraRot += rot;
                g_SampleCameraObject->GetTransform().SetEulerAngles(cameraRot);
            }

            if (Input::KeyDown(KeyCode::R))
            {
                g_SampleCameraObject->GetTransform().m_LocalPosition = Vector3(0.0f, 0.0f, -10.0f);
                g_SampleCameraObject->GetTransform().SetRotation(Quaternion::Identity);
                g_SampleCameraObject->GetTransform().m_LocalScale = Vector3::One;
            }

            int rotAxis = -1;
            const Vector3 dirs[] = { Vector3::Right ,Vector3::Up ,Vector3::Forward };
            if (Input::KeyState(KeyCode::NumPad1))
                rotAxis = 0;
            if (Input::KeyState(KeyCode::NumPad2))
                rotAxis = 1;
            if (Input::KeyState(KeyCode::NumPad3))
                rotAxis = 2;

            if (rotAxis == -1)
            {
                g_SampleModelObject[1]->GetTransform().m_LocalRotation = Quaternion::CreateFromEulerAngles(45, 0.0f, 0.0f);
                g_SampleModelObject[1]->GetTransform().m_LocalPosition = Vector3::Up * 0.75f;
            }
            else
            {
                //g_SampleModelObject[1]->GetTransform().Rotate(dirs[rotAxis], Time::GetDeltaTime() * 90.0f, Input::KeyState(KeyCode::LeftAlt));
                g_SampleModelObject[1]->GetTransform().Rotate(dirs[rotAxis] * Time::GetDeltaTime() * 360.0f, Input::KeyState(KeyCode::LeftAlt));
                //g_SampleModelObject[1]->GetTransform().RotateAround(Vector3::Zero, dirs[rotAxis], Time::GetDeltaTime() * 360.0f);
                //g_SampleModelObject[1]->GetTransform().Translate(dirs[rotAxis] * Time::GetDeltaTime() * 10.0f, Input::KeyState(KeyCode::LeftAlt));
            }

            g_SampleModelObject[0]->GetTransform().Rotate(Vector3(0.0f, Time::GetDeltaTime() * 90.0f, 0.0f), true);

            if (Input::KeyDown(KeyCode::Back))
            {
                TRACE("Input::KeyDown(KeyCode::Back)");
                //g_SampleModelObject[0]->Destroy();
                //g_SampleModelObject[0] = nullptr;
                g_SampleModelObject[0]->RemoveComponent<MeshRenderer>();
            }
        }

        {
            g_SampleLight->GetTransform().Rotate(Vector3::Up, Time::GetDeltaTime() * 45.0f, true);
            auto speed = 1.0f;
            g_SampleLight->m_LightColor = Vector4(
                (Math::Cos(Time::GetRunTime() * speed) + 1.0f) * 0.5f,
                (Math::Cos(Time::GetRunTime() * speed - (Math::TAU / 3.0f)) + 1.0f) * 0.5f,
                (Math::Cos(Time::GetRunTime() * speed + (Math::TAU / 3.0f)) + 1.0f) * 0.5f,
                1.0f
            );
        }
    }
}