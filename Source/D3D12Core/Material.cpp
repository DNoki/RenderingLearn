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
        m_IsChanged = true;

        {
            m_PipelineState.reset(new GraphicsPipelineState());

            m_PipelineState->SetInputLayout(m_Shader->GetInputLayout());
            m_PipelineState->SetRootSignature(m_Shader->GetRootSignature());
            m_PipelineState->SetVertexShader(m_Shader->GetShaderBuffer(ShaderType::VertexShader));
            m_PipelineState->SetPixelShader(m_Shader->GetShaderBuffer(ShaderType::PixelShader));
            m_PipelineState->SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);
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
        ASSERT(!m_IsChanged);
        if (m_IsChanged) return;

        ID3D12DescriptorHeap* ppHeaps[] = { m_ResourceDescHeap->GetD3D12DescriptorHeap(), g_CommonSamplersDescriptorHeap.GetD3D12DescriptorHeap() };
        commandList->GetD3D12CommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    }
    void Material::ExecuteBindMaterial(const CommandList* commandList) const
    {
        ASSERT(!m_IsChanged);
        if (m_IsChanged) return;

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
    D3D12_FILL_MODE Material::GetFillMode() const
    {
        return m_PipelineState->GetRasterizerState().FillMode;
    }
    void Material::SetCullMode(D3D12_CULL_MODE cullMode)
    {
        auto& psoCullMode = m_PipelineState->GetRasterizerState().CullMode;
        if (psoCullMode != cullMode)
        {
            psoCullMode = cullMode;
            m_IsChanged = true;
        }
    }
    D3D12_CULL_MODE Material::GetCullMode() const
    {
        return m_PipelineState->GetRasterizerState().CullMode;
    }
    void Material::SetFrontCounterClockwise(bool isCounterClockwise)
    {
        auto& psoFrontCounterClockwise = m_PipelineState->GetRasterizerState().FrontCounterClockwise;
        if (psoFrontCounterClockwise != (isCounterClockwise ? TRUE : FALSE))
        {
            psoFrontCounterClockwise = isCounterClockwise;
            m_IsChanged = true;
        }
    }
    bool Material::GetFrontCounterClockwise() const
    {
        return m_PipelineState->GetRasterizerState().FrontCounterClockwise;
    }

    void Material::SetDepthEnable(bool enable)
    {
        auto& psoValue = m_PipelineState->GetDepthStencilState().DepthEnable;
        if (psoValue != (enable ? TRUE : FALSE))
        {
            psoValue = enable;
            m_IsChanged = true;
        }
    }
    bool Material::GetDepthEnable() const
    {
        return m_PipelineState->GetDepthStencilState().DepthEnable;
    }
    void Material::SetDepthWriteEnable(bool enable)
    {
        D3D12_DEPTH_WRITE_MASK& psoValue = m_PipelineState->GetDepthStencilState().DepthWriteMask;
        if (psoValue != (enable ? TRUE : FALSE))
        {
            psoValue = enable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
            m_IsChanged = true;
        }
    }
    bool Material::GetDepthWriteEnable() const
    {
        return m_PipelineState->GetDepthStencilState().DepthWriteMask;
    }
    void Material::SetDepthFunc(D3D12_COMPARISON_FUNC func)
    {
        auto& psoValue = m_PipelineState->GetDepthStencilState().DepthFunc;
        if (psoValue != func)
        {
            psoValue = func;
            m_IsChanged = true;
        }
    }
    D3D12_COMPARISON_FUNC Material::GetDepthFunc() const
    {
        return m_PipelineState->GetDepthStencilState().DepthFunc;
    }
}

