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
        m_Version = 1;

        {
            m_PipelineState.reset(new GraphicsPipelineState());

            m_PipelineState->SetInputLayout(m_Shader->GetInputLayout());
            m_PipelineState->SetRootSignature(m_Shader->GetRootSignature());
            m_PipelineState->SetVertexShader(m_Shader->GetShaderBuffer(ShaderType::VertexShader));
            m_PipelineState->SetPixelShader(m_Shader->GetShaderBuffer(ShaderType::PixelShader));
        }
        {
            m_ResourceDescHeap.reset(new DescriptorHeap());
            m_ResourceDescHeap->Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_Shader->GetBindResourceCount());
        }
    }

    void Material::ExecuteBindMaterial(const CommandList* commandList, bool isOnlyBindDescriptorHeap) const
    {
        if (isOnlyBindDescriptorHeap)
        {
            commandList->SetDescriptorHeaps(m_ResourceDescHeap.get(), &g_CommonSamplersDescriptorHeap);
        }
        else
        {
            // 设置根签名
            commandList->SetGraphicsRootSignature(m_PipelineState->GetRootSignature());
            // 管线状态
            commandList->SetPipelineState(m_PipelineState.get());

            // 绑定描述符堆
            {
                commandList->SetDescriptorHeaps(m_ResourceDescHeap.get(), &g_CommonSamplersDescriptorHeap);

                auto rootPrarmIndex = 0;
                for (UINT i = 0; i < m_ResourceDescHeap->GetDescriptorsCount(); i++)
                    commandList->SetGraphicsRootDescriptorTable(rootPrarmIndex++, m_ResourceDescHeap->GetDescriptorHandle(i));

                if (m_SamplerDescriptorHandle)
                    commandList->SetGraphicsRootDescriptorTable(rootPrarmIndex++, *m_SamplerDescriptorHandle);
            }
        }
    }

    void Material::BindBuffer(int slot, const Graphics::IBufferResource& buffer)
    {
        m_ResourceDescHeap->BindConstantBufferView(slot, buffer);
        m_Version++;
    }
    void Material::BindTexture(int slot, const Graphics::ITexture& texture)
    {
        m_ResourceDescHeap->BindShaderResourceView(slot, texture);
        m_Version++;
    }


    void Material::SetRenderTargetsFormat(const MultiRenderTargets* mrt)
    {
        auto& pso = m_PipelineState->GetPsoDesc();

        // 比较和渲染管线渲染目标格式是否一致
        auto isSame = true;
        if (pso.NumRenderTargets != mrt->GetRenderTargetCount()
            || memcmp(pso.RTVFormats, mrt->GetRenderTargetsFormat(), sizeof(DXGI_FORMAT) * pso.NumRenderTargets) != 0
            || pso.DSVFormat != *mrt->GetDepthStencilFormat())
            isSame = false;

        if (!isSame)
        {
            pso.NumRenderTargets = mrt->GetRenderTargetCount();
            CopyMemory(pso.RTVFormats, mrt->GetRenderTargetsFormat(), sizeof(DXGI_FORMAT) * mrt->GetRenderTargetCount());
            pso.DSVFormat = *mrt->GetDepthStencilFormat();
        }
    }

    void Material::SetFillMode(D3D12_FILL_MODE fillMode)
    {
        auto& psoFillMode = m_PipelineState->GetRasterizerState().FillMode;
        if (psoFillMode != fillMode)
        {
            psoFillMode = fillMode;
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
        }
    }
    D3D12_COMPARISON_FUNC Material::GetDepthFunc() const
    {
        return m_PipelineState->GetDepthStencilState().DepthFunc;
    }
}

