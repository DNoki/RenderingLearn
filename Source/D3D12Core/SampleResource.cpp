#include "pch.h"

#include "AppMain.h"
#include "DescriptorHeap.h"
#include "GraphicsBuffer.h"
#include "TextureLoader.h"
#include "Texture2D.h"
#include "GpuPlacedHeap.h"
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
#include "Renderer.h"

#include "SampleResource.h"

using namespace std;
using namespace winrt;
using namespace Game;
using namespace Application;

namespace Graphics
{
    Shader g_SampleShader;

    Texture2D t_DefaultTexture[2];

    GpuPlacedHeap g_TexPlacedHeap;
    GpuPlacedHeap g_VertexPlacedHeap;
    GpuPlacedHeap g_UploadPlacedHeap; // TODO 上传堆可以改成全局管理分配模式，按需索取

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
    Renderer g_SampleRenderer;


    void InitShader()
    {
        g_SampleShader.Create();
        g_SampleMaterial.Create(&g_SampleShader);
    }
    void InitPlacedHeap()
    {
        const UINT64 sampleSize = 65536ull * 100u;

        // 纹理放置堆
        g_TexPlacedHeap = GpuPlacedHeap();
        g_TexPlacedHeap.Create(D3D12_HEAP_TYPE_DEFAULT, sampleSize, D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS);

        // 顶点放置堆
        g_VertexPlacedHeap = GpuPlacedHeap();
        g_VertexPlacedHeap.Create(D3D12_HEAP_TYPE_DEFAULT, sampleSize, D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS); // 默认顶点堆选项必须为缓冲区

        // 通用上传放置堆
        g_UploadPlacedHeap = GpuPlacedHeap();
        g_UploadPlacedHeap.Create(D3D12_HEAP_TYPE_UPLOAD, sampleSize, D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS); // 上传堆允许存放任何类型数据
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
        t1.PlacedCreate(g_TexPlacedHeap, texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
        t1.DispatchCopyTextureData(g_GraphicsCommandList, texData.GetDataPointer());

        texPath = Application::GetAssetPath();
        texPath.append(L"云堇.jpg");
        texData.LoadTexture2D(texPath);

        t_DefaultTexture[1].DirectCreate(texData.GetFormat(), texData.GetWidth(), texData.GetHeight());
        t_DefaultTexture[1].DispatchCopyTextureData(g_GraphicsCommandList, texData.GetDataPointer());

        g_SampleMaterial.GetResourceDescHeap()->BindShaderResourceView(0, t_DefaultTexture[1]);


        auto constantBufferSize = UINT_UPPER(sizeof(MVPBuffer), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
        //auto constantBufferSize = sizeof(MVPBuffer);
        {
            g_MvpBufferRes.DirectCreate(constantBufferSize);
            g_MvpBufferRes.Map(0, reinterpret_cast<void**>(&g_MVPBuffer));
            g_MVPBuffer->m_MVP = Matrix4x4::Identity;
            //g_MvpBufferRes.DispatchCopyBuffer(g_GraphicsCommandList, &mvp);
            g_SampleMaterial.GetResourceDescHeap()->BindConstantBufferView(1, g_MvpBufferRes);
        }
    }
    void InitMesh()
    {
        // 创建顶点缓冲
        struct Vertex
        {
            Vector3 position;
            Vector4 color;
            Vector2 uv;
        };

        auto m_aspectRatio = g_SwapChain.GetScreenAspect();
        auto meshSize = 0.5f;
        Vertex vertices[] =
        {
            //{ { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f } },
            //{ { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }  },
            //{ { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }  }

            // 定义三角形条带
            { { -1.0f * meshSize, -1.0f * m_aspectRatio * meshSize, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { -1.0f, -1.0f } }, // Bottom left.
            { { -1.0f * meshSize, 1.0f * m_aspectRatio * meshSize, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { -1.0f, 2.0f }  }, // Top left.
            { { 1.0f * meshSize, -1.0f * m_aspectRatio * meshSize, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 2.0f, -1.0f }  }, // Bottom right.
            { { 1.0f * meshSize, 1.0f * m_aspectRatio * meshSize, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 2.0f, 2.0f }  }, // Top right.
        };
        const UINT vertexBufferSize = sizeof(vertices);


        g_SampleMesh=(Mesh::CreateCube());

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
        useSamplerIndex += Input::KeyDown(KeyCode::Space) ? 1 : 0;
        useSamplerIndex = Math::Repeat(useSamplerIndex, 0, _countof(samplers));

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
                pos.z += Input::GetMouseDeltaScrollWheel() * 10.0f;
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