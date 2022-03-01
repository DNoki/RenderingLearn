#include "pch.h"

#include "AppMain.h"
#include "DescriptorHeap.h"
#include "GraphicsBuffer.h"
#include "TextureLoader.h"
#include "Texture2D.h"
#include "GraphicsMemory.h"
#include "GraphicsCommon.h"
#include "GraphicsCore.h"
#include "PipelineState.h"
#include "CommandList.h"

#include "Display.h"
#include "Input.h"
#include "GameTime.h"
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "MeshRenderer.h"

#include "SampleResource.h"

using namespace std;
using namespace winrt;
using namespace Game;
using namespace Application;

namespace Graphics
{
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

    Camera g_Camera;
    Transform g_ModelTrans;

    Mesh g_SampleMesh;
    Material g_SampleMaterial;
    MeshRenderer g_SampleRenderer;


    void InitShader()
    {
        g_SampleShader.Create();
        g_SampleMaterial.Create(&g_SampleShader);
    }
    void InitTexture2D()
    {
        auto texPath = Application::GetAssetPath();
        texPath.append("Shimarin.png");

        // 创建Checker贴图
        //t_DefaultTexture.GenerateChecker(t_SampleResDescHeap.GetDescriptorHandle(0), 256, 256);

        TextureLoader texData;
        texData.LoadTexture2D(texPath);

        auto& t1 = t_DefaultTexture[0];
        //g_TestTex2D.DirectCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
        t1.PlacedCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
        t1.DispatchCopyTextureData(g_GraphicsCommandList, texData.GetDataPointer());

        texPath = Application::GetAssetPath();
        texPath.append(L"云堇.jpg");
        texData.LoadTexture2D(texPath);

        t_DefaultTexture[1].PlacedCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
        t_DefaultTexture[1].DispatchCopyTextureData(g_GraphicsCommandList, texData.GetDataPointer());

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
    }
    void InitMesh()
    {
        g_SampleMesh = (Mesh::CreateCube());

        g_ModelTrans = Transform();
        g_ModelTrans.LocalScale = Vector3::One * 2.0f;

        g_Camera = Camera();
        g_Camera.m_ProjectionMode = ProjectionMode::Orthographic;
        g_Camera.m_FieldOfView = 60.0f;
        g_Camera.m_Transform.LocalPosition = Vector3(0.0f, 0.0f, -10.0f);
        g_Camera.m_Transform.LocalEulerAngles = Vector3(0.0f, 0.0f, 0.0f) * Math::Deg2Rad;
    }

    void InitCommandListBundle()
    {
        g_SampleRenderer.Create(&g_SampleMesh, &g_SampleMaterial);
    }


    void SampleDraw()
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
                    pos.y += 1.0f;
                if (Input::KeyState(KeyCode::S))
                    pos.y -= 1.0f;
                if (Input::KeyState(KeyCode::A))
                    pos.x -= 1.0f;
                if (Input::KeyState(KeyCode::D))
                    pos.x += 1.0f;
                pos.z += Input::GetMouseDeltaScrollWheel();
                g_Camera.m_Transform.LocalPosition += pos * Time::GetDeltaTime() * 10.0f;

                Vector3 rot{};
                if (Input::MouseButtonState(MouseButtonType::LeftButton))
                {
                    Vector2 deltaPos = Input::GetMouseDeltaPos();
                    rot.y = deltaPos.x;
                    rot.x = -deltaPos.y;
                    rot.y *= -1.0f;
                }
                g_Camera.m_Transform.LocalEulerAngles += rot * 10.0f * Time::GetDeltaTime() * Math::Deg2Rad;
                //TRACE(L"%f, %f\n", g_CameraTrans.LocalEulerAngles.x, g_CameraTrans.LocalEulerAngles.y);

                if (Input::KeyDown(KeyCode::R))
                {
                    g_Camera.m_Transform.LocalPosition = Vector3(0.0f, 0.0f, -10.0f);
                    g_Camera.m_Transform.LocalEulerAngles = Vector3::Zero;
                    g_Camera.m_Transform.LocalScale = Vector3::One;
                }

                g_ModelTrans.LocalEulerAngles.y += Time::GetDeltaTime() * 90.0f * Math::Deg2Rad;
            }

            g_Camera.m_ProjectionMode = ProjectionMode::Perspective;
            Matrix4x4 pers = g_Camera.GetProjectionMatrix();

            Matrix4x4 view = g_Camera.GetViewMatrix();

            Matrix4x4 model = g_ModelTrans.GetLocalToWorldMatrix();

            g_MVPBuffer->m_P = pers;
            g_MVPBuffer->m_V = view;
            g_MVPBuffer->m_M = model;
            g_MVPBuffer->m_IT_M = model.Invert().Transpose();
#ifdef USE_COLUMN_MAJOR
            g_MVPBuffer->m_MVP = pers * view * model;
#else
            g_MVPBuffer->m_MVP = model * view * pers;
#endif // USE_COLUMN_MAJOR

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

        g_SampleRenderer.ExecuteDraw(&g_GraphicsCommandList);
    }

}