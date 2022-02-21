#include "pch.h"

#include "PipelineState.h"
#include "Shader.h"
#include "DescriptorHeap.h"
#include "CommandList.h"
#include "GraphicsCommon.h"

#include "Material.h"

using namespace std;
using namespace Graphics;

namespace Game
{
    void Material::Create(const Shader* shader)
    {
        m_Shader = shader;

        {
            m_PipelineState.reset(new GraphicsPipelineState());

            // 定义顶点输入层
            const D3D12_INPUT_ELEMENT_DESC sampleInputElementDescs[] =
            {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            };

            m_PipelineState->SetInputLayout(_countof(sampleInputElementDescs), sampleInputElementDescs);
            m_PipelineState->SetRootSignature(m_Shader->GetRootSignature());
            m_PipelineState->SetVertexShader(m_Shader->GetShaderBuffer(ShaderType::VertexShader));
            m_PipelineState->SetPixelShader(m_Shader->GetShaderBuffer(ShaderType::PixelShader));
            m_PipelineState->SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);
            m_PipelineState->GetRasterizerState().FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME;
            m_PipelineState->Finalize();
        }
        {
            m_ResourceDescHeap.reset(new DescriptorHeap());
            m_ResourceDescHeap->Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2);
        }
    }
    void Material::RefleshPipelineState()
    {
        m_PipelineState->Finalize();
        m_IsChanged = false;
    }
    void Material::ExecuteBindDescriptorHeap(const CommandList* commandList) const
    {
        ID3D12DescriptorHeap* ppHeaps[] = { m_ResourceDescHeap->GetD3D12DescriptorHeap(), g_CommonSamplersDescriptorHeap.GetD3D12DescriptorHeap() };
        commandList->GetD3D12CommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    }
    void Material::ExecuteBindMaterial(const CommandList* commandList) const
    {
        auto* d3d12CommandList = commandList->GetD3D12CommandList();

        // 设置根签名
        d3d12CommandList->SetGraphicsRootSignature(m_PipelineState->GetD3D12RootSignature());
        // 管线状态
        d3d12CommandList->SetPipelineState(m_PipelineState->GetD3D12PSO());

        // 绑定描述符堆
        {
            ExecuteBindDescriptorHeap(commandList);

            auto rootPrarmIndex = 0;
            for (UINT i = 0; i < m_ResourceDescHeap->GetDescriptorsCount(); i++)
                d3d12CommandList->SetGraphicsRootDescriptorTable(rootPrarmIndex++, m_ResourceDescHeap->GetDescriptorHandle(i));
            d3d12CommandList->SetGraphicsRootDescriptorTable(rootPrarmIndex++, g_SamplerLinearClamp);
        }
    }

    void Material::SetFillMode(D3D12_FILL_MODE fillMode)
    {
        auto& psoFillMode = m_PipelineState->GetRasterizerState().FillMode;
        if (psoFillMode != fillMode)
        {
            psoFillMode = fillMode;
            m_IsChanged = true;
        }
    }
}

