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

#include "Display.h"
#include "Input.h"

#include "SampleResource.h"

using namespace std;
using namespace winrt;
using namespace Graphics;

namespace SampleResource
{
    RootSignature g_RootSignature;
    GraphicsPipelineState g_PipelineState;


    GraphicsBuffer g_SampleVBV;

    DescriptorHeap t_TexDH;
    Texture2D t_DefaultTexture[2];

    GpuPlacedHeap g_TexPlacedHeap;
    GpuPlacedHeap g_VertexPlacedHeap;
    GpuPlacedHeap g_UploadPlacedHeap; // TODO 上传堆可以改成全局管理分配模式，按需索取

    UploadBuffer g_MvpBufferRes[2];

    struct MVPBuffer
    {
        DirectX::XMFLOAT4X4 m_MVP;
    };
    MVPBuffer* g_MVPBuffer[2];


    void InitRootSignature()
    {
        // 创建根签名
        g_RootSignature = RootSignature();

        // 使用静态采样器
        //{
        //    g_RootSignature.Reset(1, 1);

        //    // 创建一块描述符表
        //    CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        //    ranges[0].Init(
        //        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, // 描述符表类型为着色器资源视图
        //        1, // 表中的描述符数量
        //        0, // 基准注册位置
        //        0, // 寄存器空间，通常可以为 0
        //        D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC); // 指定描述符和它们引用的数据的易失性
        //    g_RootSignature[0].InitAsDescriptorTable(1, ranges);

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
        g_PipelineState.GetDepthStencilState().DepthEnable = FALSE;
        g_PipelineState.SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN);

        g_PipelineState.Finalize();
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
        t_TexDH = DescriptorHeap();
        t_TexDH.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2);

        auto texPath = Application::GetAssetPath();
        texPath.append("Shimarin.png");

        // 创建Checker贴图
        //t_DefaultTexture.GenerateChecker(t_TexDH.GetDescriptorHandle(0), 256, 256);

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

        t_TexDH.BindShaderResourceView(0, t_DefaultTexture[1]);


        auto mat = Matrix4x4::Identity;
        mat._11 = cos(30.0f * 3.1415926f / 180.0f);
        mat._12 = sin(30.0f * 3.1415926f / 180.0f);
        mat._21 = -sin(30.0f * 3.1415926f / 180.0f);
        mat._22 = cos(30.0f * 3.1415926f / 180.0f);
        auto mvp = MVPBuffer
        {
            mat,
        };

        auto mat2 = mat;
        mat2._11 = cos(-30.0f * 3.1415926f / 180.0f);
        mat2._12 = sin(-30.0f * 3.1415926f / 180.0f);
        mat2._21 = -sin(-30.0f * 3.1415926f / 180.0f);
        mat2._22 = cos(-30.0f * 3.1415926f / 180.0f);
        auto mvp2 = MVPBuffer
        {
            mat2,
        };

        auto constantBufferSize = UINT_UPPER(sizeof(MVPBuffer), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
        //auto constantBufferSize = sizeof(MVPBuffer);
        {
            g_MvpBufferRes[0].DirectCreate(constantBufferSize);
            g_MvpBufferRes[0].Map(0, reinterpret_cast<void**>(&g_MVPBuffer[0]));
            g_MVPBuffer[0]->m_MVP = mat;
            //g_MvpBufferRes[0].DispatchCopyBuffer(g_GraphicsCommandList, &mvp);
            t_TexDH.BindConstantBufferView(1, g_MvpBufferRes[0]);

            g_MvpBufferRes[1].DirectCreate(constantBufferSize);
            g_MvpBufferRes[1].Map(0, reinterpret_cast<void**>(&g_MVPBuffer[1]));
            g_MVPBuffer[1]->m_MVP = mat2;
            //g_MvpBufferRes[1].DispatchCopyBuffer(g_GraphicsCommandList, &mvp2);
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
        g_SampleVBV.DispatchCopyVertexBuffer(g_GraphicsCommandList, sizeof(Vertex), vertices);
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
        if (useSamplerIndex >= 8) useSamplerIndex = 0;

        {
            static int index = 0;
            index += Input::KeyDown(KeyCode::Enter) ? 1 : 0;
            if (index > 1) index = 0;
            t_TexDH.BindConstantBufferView(1, g_MvpBufferRes[index]);

            t_TexDH.BindShaderResourceView(0, t_DefaultTexture[index]);
        }

        // 绑定描述符堆
        {
            ID3D12DescriptorHeap* ppHeaps[] = { t_TexDH.GetD3D12DescriptorHeap(), g_CommonSamplersDescriptorHeap.GetD3D12DescriptorHeap() };
            commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            auto rootPrarmIndex = 0;
            for (UINT i = 0; i < t_TexDH.GetDescriptorsCount(); i++)
                commandList->SetGraphicsRootDescriptorTable(rootPrarmIndex++, t_TexDH.GetDescriptorHandle(i));
            commandList->SetGraphicsRootDescriptorTable(rootPrarmIndex++, *samplers[useSamplerIndex]);
        }

        //commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //commandList->IASetVertexBuffers(0, 1, g_SampleVBV.GetD3D12VBV());
        //commandList->DrawInstanced(3, 1, 0, 0);

        // 使用三角形带渲染，这是最快的绘制矩形的方式，是渲染UI的核心方法
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        commandList->IASetVertexBuffers(0, 1, g_SampleVBV);
        commandList->DrawInstanced(4, 1, 0, 0);
    }
}