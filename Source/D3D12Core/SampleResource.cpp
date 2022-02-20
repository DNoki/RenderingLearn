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

#include "SampleResource.h"

using namespace std;
using namespace winrt;
using namespace Game;
using namespace Application;

namespace Graphics
{
    //RootSignature g_RootSignature;
    //GraphicsPipelineState g_PipelineState;
    Shader g_SampleShader;

    vector<Mesh> g_SampleMeshs;
    GraphicsBuffer g_SampleVBV;

    //DescriptorHeap t_SampleResDescHeap;
    Texture2D t_DefaultTexture[2];

    GpuPlacedHeap g_TexPlacedHeap;
    GpuPlacedHeap g_VertexPlacedHeap;
    GpuPlacedHeap g_UploadPlacedHeap; // TODO 上传堆可以改成全局管理分配模式，按需索取

    UploadBuffer g_MvpBufferRes;

    struct MVPBuffer
    {
        DirectX::XMFLOAT4X4 m_P;
        DirectX::XMFLOAT4X4 m_V;
        DirectX::XMFLOAT4X4 m_M;
        DirectX::XMFLOAT4X4 m_MVP;
    };
    MVPBuffer* g_MVPBuffer;


    Camera g_Camera;
    Transform g_ModelTrans;

    CommandList g_BundleCommandList;
    //com_ptr<ID3D12CommandAllocator> g_SampleBundleCommandAllocator;
    //com_ptr<ID3D12GraphicsCommandList5> g_SampleBundleGraphicsCommandList;

#if 0
    void InitRootSignature()
    {
        // 创建根签名
        g_RootSignature = RootSignature();

        // 使用静态采样器
        //{
        //    g_RootSignature.Reset(1, 1);
        //
        //    // 创建一块描述符表
        //    CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        //    ranges[0].Init(
        //        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, // 描述符表类型为着色器资源视图
        //        1, // 表中的描述符数量
        //        0, // 基准注册位置
        //        0, // 寄存器空间，通常可以为 0
        //        D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC); // 指定描述符和它们引用的数据的易失性
        //    g_RootSignature[0].InitAsDescriptorTable(1, ranges);
        //
        //    // 静态采样器将直接写入根签名
        //    g_RootSignature.GetStaticSamplerDesc(0).Init(
        //        0, // 指定着色器注册位置
        //        D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        //        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        //        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        //        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        //        0, 0, D3D12_COMPARISON_FUNC_NEVER
        //    );
        //}

        // 使用动态采样器
        CD3DX12_DESCRIPTOR_RANGE1 ranges[3]{};
        {
            g_RootSignature.Reset(3, 0);

            ranges[0].Init(
                D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                1,
                0,
                0,
                D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
            ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
            ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

            for (int i = 0; i < _countof(ranges); i++)
                g_RootSignature[i].InitAsDescriptorTable(1, &ranges[i]);
        }

        g_RootSignature.Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    }
    void InitPipelineState()
    {
        // 创建图形管线状态
        g_PipelineState = GraphicsPipelineState();

        // 编译Shader
        winrt::com_ptr<ID3DBlob> vertexShader;
        winrt::com_ptr<ID3DBlob> pixelShader;
        Path vsShaderPath = Application::GetShaderPath().append("SampleTexture_vs.cso");
        Path psShaderPath = Application::GetShaderPath().append("SampleTexture_ps.cso");
        ShaderUtility::ReadVSFromFile(vsShaderPath, vertexShader.put());
        ShaderUtility::ReadPSFromFile(psShaderPath, pixelShader.put());


        // 定义顶点输入层
        const D3D12_INPUT_ELEMENT_DESC SAMPLE_INPUT_ELEMENT_DESCS[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        };

        g_PipelineState.SetInputLayout(_countof(SAMPLE_INPUT_ELEMENT_DESCS), SAMPLE_INPUT_ELEMENT_DESCS);
        g_PipelineState.SetRootSignature(&g_RootSignature);
        g_PipelineState.SetVertexShader(vertexShader.get());
        g_PipelineState.SetPixelShader(pixelShader.get());
        //g_PipelineState.GetDepthStencilState().DepthEnable = FALSE;
        g_PipelineState.GetRasterizerState().FrontCounterClockwise = FALSE; // TRUE:逆时针为正，FALSE:顺时针为正
        //g_PipelineState.GetRasterizerState().FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME;
        g_PipelineState.SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);

        g_PipelineState.Finalize();
    }
#endif
    void InitShader()
    {
        g_SampleShader.Create();
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
        //t_SampleResDescHeap = DescriptorHeap();
        //t_SampleResDescHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2);

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

        //t_SampleResDescHeap.BindShaderResourceView(0, t_DefaultTexture[1]);
        g_SampleShader.GetResourceDescHeap()->BindShaderResourceView(0, t_DefaultTexture[1]);


        auto constantBufferSize = UINT_UPPER(sizeof(MVPBuffer), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
        //auto constantBufferSize = sizeof(MVPBuffer);
        {
            g_MvpBufferRes.DirectCreate(constantBufferSize);
            g_MvpBufferRes.Map(0, reinterpret_cast<void**>(&g_MVPBuffer));
            g_MVPBuffer->m_MVP = Matrix4x4::Identity;
            //g_MvpBufferRes.DispatchCopyBuffer(g_GraphicsCommandList, &mvp);
            //t_SampleResDescHeap.BindConstantBufferView(1, g_MvpBufferRes);
            g_SampleShader.GetResourceDescHeap()->BindConstantBufferView(1, g_MvpBufferRes);
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

        g_SampleVBV = GraphicsBuffer();
        //g_SampleVBV.DirectCreate(vertexBufferSize);
        g_SampleVBV.PlacedCreate(vertexBufferSize, g_VertexPlacedHeap);
        g_SampleVBV.DispatchCopyBuffer(g_GraphicsCommandList, sizeof(Vertex), vertices);
        //g_SampleMeshs.push_back(Mesh());
        //g_SampleMeshs[g_SampleMeshs.size() - 1].DirectCreate((UINT)_countof(vertices), vertices);

        //g_SampleMesh = Mesh::CreateCube();
        //g_SampleMesh = Mesh::CreateSphere();
        //g_SampleMesh = Mesh::CreateGeoSphere();
        //g_SampleMesh = Mesh::CreateCylinder();
        //g_SampleMesh = Mesh::CreateCone();
        //g_SampleMesh = Mesh::CreateTorus();
        //g_SampleMesh = Mesh::CreateTetrahedron();
        //g_SampleMesh = Mesh::CreateOctahedron();
        //g_SampleMesh = Mesh::CreateDodecahedron();
        //g_SampleMesh = Mesh::CreateIcosahedron();
        g_SampleMeshs.push_back(Mesh::CreateTeapot());

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
        g_BundleCommandList.Create(D3D12_COMMAND_LIST_TYPE_BUNDLE, true);
        auto* bundleCommandList = g_BundleCommandList.GetD3D12CommandList();

#if 0
        // 设置根签名
        bundleCommandList->SetGraphicsRootSignature(g_PipelineState.GetD3D12RootSignature());
        // 管线状态
        bundleCommandList->SetPipelineState(g_PipelineState.GetD3D12PSO());

        // 绑定描述符堆
        {
            ID3D12DescriptorHeap* ppHeaps[] = { t_SampleResDescHeap.GetD3D12DescriptorHeap(), g_CommonSamplersDescriptorHeap.GetD3D12DescriptorHeap() };
            bundleCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            auto rootPrarmIndex = 0;
            for (UINT i = 0; i < t_SampleResDescHeap.GetDescriptorsCount(); i++)
                bundleCommandList->SetGraphicsRootDescriptorTable(rootPrarmIndex++, t_SampleResDescHeap.GetDescriptorHandle(i));
            bundleCommandList->SetGraphicsRootDescriptorTable(rootPrarmIndex++, g_SamplerLinearClamp);
        }

        // 设置顶点缓冲、索引缓冲
        bundleCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        bundleCommandList->IASetVertexBuffers(0, 1, g_SampleMeshs[0].GetVertexBuffer()->GetVBV());
        bundleCommandList->IASetIndexBuffer(g_SampleMeshs[0].GetIndexBuffer()->GetIBV());

        // DrawCall
        bundleCommandList->DrawIndexedInstanced(g_SampleMeshs[0].GetIndexCount(), 1, 0, 0, 0);
#endif
        g_SampleShader.ExecuteBindShader(&g_BundleCommandList);
        g_SampleMeshs[0].ExecuteDraw(&g_BundleCommandList);
        g_BundleCommandList.Close();
    }

    void OutputMatrix4x4(const Matrix4x4& m)
    {
        TRACE("%.2f\t%.2f\t%.2f\t%.2f", m._11, m._12, m._13, m._14);
        TRACE("%.2f\t%.2f\t%.2f\t%.2f", m._21, m._22, m._23, m._24);
        TRACE("%.2f\t%.2f\t%.2f\t%.2f", m._31, m._32, m._33, m._34);
        TRACE("%.2f\t%.2f\t%.2f\t%.2f", m._41, m._42, m._43, m._44);
    }
    void SampleDraw(ID3D12GraphicsCommandList* commandList)
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

            g_Camera.m_ProjectionMode = Input::KeyState(KeyCode::Enter) ? ProjectionMode::Orthographic : ProjectionMode::Perspective;
            Matrix4x4 pers = g_Camera.GetProjectionMatrix();

            Matrix4x4 view = g_Camera.GetViewMatrix();

            Matrix4x4 model = g_ModelTrans.GetLocalToWorldMatrix();

            g_MVPBuffer->m_P = pers;
            g_MVPBuffer->m_V = view;
            g_MVPBuffer->m_M = model;
#ifdef USE_COLUMN_MAJOR
            g_MVPBuffer->m_MVP = pers * view * model;
#else
            g_MVPBuffer->m_MVP = model * view * pers;
#endif // USE_COLUMN_MAJOR

            //TRACE("pers");
            //OutputMatrix4x4(pers);
            //TRACE("view");
            //OutputMatrix4x4(view);
            //TRACE("model");
            //OutputMatrix4x4(model);
            //TRACE("pers * view * model");
            //OutputMatrix4x4(g_MVPBuffer->m_MVP);

            //t_SampleResDescHeap.BindConstantBufferView(1, g_MvpBufferRes);

            //t_SampleResDescHeap.BindShaderResourceView(0, t_DefaultTexture[0]);
        }

        // 绑定描述符堆
        //{
        //    ID3D12DescriptorHeap* ppHeaps[] = { t_SampleResDescHeap.GetD3D12DescriptorHeap(), g_CommonSamplersDescriptorHeap.GetD3D12DescriptorHeap() };
        //    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

        //    auto rootPrarmIndex = 0;
        //    for (UINT i = 0; i < t_SampleResDescHeap.GetDescriptorsCount(); i++)
        //        commandList->SetGraphicsRootDescriptorTable(rootPrarmIndex++, t_SampleResDescHeap.GetDescriptorHandle(i));
        //    commandList->SetGraphicsRootDescriptorTable(rootPrarmIndex++, *samplers[useSamplerIndex]);
        //}

        //commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //commandList->IASetVertexBuffers(0, 1, g_SampleVBV.GetD3D12VBV());
        //commandList->DrawInstanced(3, 1, 0, 0);

        //for (int i = 0; i < g_SampleMeshs.size(); i++)
        //{
        //    commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //    commandList->IASetVertexBuffers(0, 1, g_SampleMeshs[i].GetVertexBuffer()->GetVBV());
        //    commandList->IASetIndexBuffer(g_SampleMeshs[i].GetIndexBuffer()->GetIBV());
        //    commandList->DrawIndexedInstanced(g_SampleMeshs[i].GetIndexCount(), 1, 0, 0, 0);
        //}

        // 使用三角形带渲染，这是最快的绘制矩形的方式，是渲染UI的核心方法
        //{
        //    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        //    commandList->IASetVertexBuffers(0, 1, g_SampleVBV.GetVBV());
        //    commandList->DrawInstanced(4, 1, 0, 0);
        //}


        g_SampleShader.ExecuteBindDescriptorHeap(&g_GraphicsCommandList);
        commandList->ExecuteBundle(g_BundleCommandList.GetD3D12CommandList());
    }

}