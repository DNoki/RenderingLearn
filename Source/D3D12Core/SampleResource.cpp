#include "pch.h"

#include "DescriptorHeap.h"
#include "GpuBuffer.h"
#include "Texture2D.h"
#include "GpuPlacedHeap.h"
#include "GraphicsCommon.h"
#include "PipelineState.h"

#include "Display.h"

#include "SampleResource.h"

using namespace std;
using namespace winrt;
using namespace Graphics;

namespace SampleResource
{
    RootSignature g_RootSignature;
    GraphicsPipelineState g_PipelineState;


    GpuBuffer g_SampleVBV;

    DescriptorHeap t_TexDH;
    Texture2D t_DefaultTexture;

    GpuPlacedHeap g_PlacedHeap;
    GpuPlacedHeap g_UploadPlacedHeap;


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
        {
            g_RootSignature.Reset(2, 0);

            CD3DX12_DESCRIPTOR_RANGE1 ranges[2]{};
            ranges[0].Init(
                D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                1,
                0,
                0,
                D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
            ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
            g_RootSignature[0].InitAsDescriptorTable(1, &ranges[0]);
            g_RootSignature[1].InitAsDescriptorTable(1, &ranges[1]);
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
        auto shaderPath = L"F:\\DxProject\\RenderingLearnProject\\RenderingLearn\\Source\\Shaders\\SampleTexture.hlsl";
        ShaderUtility::CompileVSFromFile(shaderPath, vertexShader.put());
        ShaderUtility::CompilePSFromFile(shaderPath, pixelShader.put());


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
        g_PlacedHeap = GpuPlacedHeap();
        g_PlacedHeap.Create(D3D12_HEAP_TYPE_DEFAULT, 1024 * 1024 * 4 * 2, D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS);
        g_UploadPlacedHeap = GpuPlacedHeap();
        g_UploadPlacedHeap.Create(D3D12_HEAP_TYPE_UPLOAD, 1024 * 1024 * 4 * 2, D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
    }
    void InitTexture2D()
    {
        t_TexDH = DescriptorHeap();
        t_TexDH.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

        auto texPath = "F:/DxProject/RenderingLearnProject/RenderingLearn/Assets/Shimarin.png";

        // 创建Checker贴图
        //t_DefaultTexture.GenerateChecker(t_TexDH.GetDescriptorHandle(0), 256, 256);

        // 使用注册方式创建贴图
        //t_DefaultTexture.Create(texPath, t_TexDH.GetDescriptorHandle(0));

        // 使用定位方式创建贴图
        t_DefaultTexture.Placed(texPath, t_TexDH.GetDescriptorHandle(0), g_PlacedHeap, g_UploadPlacedHeap);

    }
    void InitMesh()
    {
        // 创建顶点缓冲
        struct Vertex
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 color;
            DirectX::XMFLOAT2 uv;
        };

        auto m_aspectRatio = Display::GetScreenAspect();
        auto meshSize = 2.5f;
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

        g_SampleVBV = GpuBuffer();
        g_SampleVBV.CreateVertexBuffer(sizeof(Vertex), sizeof(vertices), vertices);

        // TODO 学习使用定位方式创建顶点缓冲
        //D3D12_RESOURCE_DESC vertexBufferDesc{};
        //vertexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        //vertexBufferDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        //vertexBufferDesc.Width = vertexBufferSize;
        //vertexBufferDesc.Height = 1;
        //vertexBufferDesc.DepthOrArraySize = 1;
        //vertexBufferDesc.MipLevels = 1;
        //vertexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        //vertexBufferDesc.SampleDesc.Count = 1;
        //vertexBufferDesc.SampleDesc.Quality = 0;
        //vertexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        //vertexBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    }

    void SampleDraw(ID3D12GraphicsCommandList* commandList)
    {
        // 绑定描述符堆
        ID3D12DescriptorHeap* ppHeaps[] = { t_TexDH.GetD3D12DescriptorHeap(), g_CommonSamplersDescriptorHeap.GetD3D12DescriptorHeap() };
        commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

        commandList->SetGraphicsRootDescriptorTable(0, *t_DefaultTexture.GetDescriptorHandle());
        commandList->SetGraphicsRootDescriptorTable(1, g_SamplerLinearClamp);

        //commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //commandList->IASetVertexBuffers(0, 1, g_SampleVBV.GetD3D12VBV());
        //commandList->DrawInstanced(3, 1, 0, 0);

        // 使用三角形带渲染，这是最快的绘制矩形的方式，是渲染UI的核心方法
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        commandList->IASetVertexBuffers(0, 1, g_SampleVBV.GetD3D12VBV());
        commandList->DrawInstanced(4, 1, 0, 0);
    }
}