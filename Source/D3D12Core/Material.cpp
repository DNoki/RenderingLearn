#include "pch.h"

#include "PipelineState.h"
#include "Shader.h"
#include "DescriptorHeap.h"
#include "CommandList.h"
#include "GraphicsCommon.h"
#include "GraphicsResource.h"
#include "GraphicsManager.h"

#include "Material.h"

using namespace std;
using namespace Graphics;

namespace Game
{
    void Material::Create(const Shader* shader)
    {
        if (shader == nullptr) throw L"ERROR::Shader is nullptr";
        m_Shader = shader;
        m_Version = 1;

        // 创建管线状态对象
        {
            m_PipelineState.reset(new GraphicsPipelineState());

            m_PipelineState->SetInputLayout(m_Shader->GetInputLayout());
            m_PipelineState->SetRootSignature(m_Shader->GetRootSignature());
            m_PipelineState->SetVertexShader(m_Shader->GetShaderBuffer(ShaderType::VertexShader));
            m_PipelineState->SetPixelShader(m_Shader->GetShaderBuffer(ShaderType::PixelShader));
        }

        {
            // 创建常量缓冲列表
            m_ConstantBuffers.resize(m_Shader->GetShaderDesc().m_CbvCount);
            // 创建资源描述符堆
            if (m_Shader->GetShaderDesc().GetBindResourceCount() > 0)
            {
                m_ResourceDescHeap.reset(new DescriptorHeap());
                m_ResourceDescHeap->Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_Shader->GetShaderDesc().GetBindResourceCount(), true);
            }
            if (m_Shader->GetShaderDesc().m_SamplerCount)
            {
                // 创建采样器描述符堆
                m_SamplerDescHeap.reset(new DescriptorHeap());
                m_SamplerDescHeap->Create(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, m_Shader->GetShaderDesc().m_SamplerCount, true);
            }
        }
    }

    void Material::SetName(const std::wstring& name)
    {
        m_Name = std::wstring(name);
        if (m_PipelineState)
        {
            // TODO
            //SET_DEBUGNAME(m_PipelineState->GetD3D12PSO(), Application::Format(_T("%s (PipelineState)"), m_Name.c_str()));
        }
        if (m_ResourceDescHeap)
        {
            m_ResourceDescHeap->SetName(Application::Format(_T("%s (Material::Resources)"), m_Name.c_str()));
        }
        if (m_SamplerDescHeap)
        {
            m_SamplerDescHeap->SetName(Application::Format(_T("%s (Material::Samplers)"), m_Name.c_str()));
        }
    }

    void Material::DispatchBindMaterial(const CommandList* commandList, bool isOnlyBindDescriptorHeap) const
    {
        /*
            允许 图形命令列表 或 捆绑包命令列表 执行绑定材质操作。
            执行捆绑包时，要求描述符堆和主命令列表上的一致
        */
        if (isOnlyBindDescriptorHeap)
        {
            ASSERT(commandList->GetType() == D3D12_COMMAND_LIST_TYPE_DIRECT);
            commandList->SetDescriptorHeaps(m_ResourceDescHeap.get(), m_SamplerDescHeap.get());
        }
        else
        {
            // 设置根签名
            commandList->SetGraphicsRootSignature(m_Shader->GetRootSignature());
            // 管线状态
            commandList->SetPipelineState(m_PipelineState.get());

            // 绑定描述符堆
            {
                commandList->SetDescriptorHeaps(m_ResourceDescHeap.get(), m_SamplerDescHeap.get());

                UINT rootParamIndex = 0;

                // 绑定常量缓冲
                for (int i = 0; i < m_Shader->GetShaderDesc().m_CbvCount; i++)
                {
                    commandList->SetGraphicsRootConstantBufferView(rootParamIndex++, m_ConstantBuffers[i]);
                }
                // 绑定资源
                if (m_ResourceDescHeap && m_ResourceDescHeap->GetDescriptorsCount() > 0)
                    commandList->SetGraphicsRootDescriptorTable(rootParamIndex++, m_ResourceDescHeap.get());
                // 绑定采样器
                if (m_SamplerDescHeap && m_SamplerDescHeap->GetDescriptorsCount() > 0)
                    commandList->SetGraphicsRootDescriptorTable(rootParamIndex++, m_SamplerDescHeap.get());
#if 0
                auto rootPrarmIndex = 0;
                for (UINT i = 0; i < m_ResourceDescHeap->GetDescriptorsCount(); i++)
                    commandList->SetGraphicsRootDescriptorTable(rootPrarmIndex++, m_ResourceDescHeap->GetDescriptorHandle(i));

                if (m_SamplerDescriptorHandle)
                    commandList->SetGraphicsRootDescriptorTable(rootPrarmIndex++, *m_SamplerDescriptorHandle);
#endif
            }
        }
    }

    void Material::BindBuffer(int slot, const Graphics::IBufferResource& buffer)
    {
        ASSERT(slot < m_Shader->GetShaderDesc().m_CbvCount);
        if (m_ConstantBuffers[slot] != &buffer)
        {
            m_ConstantBuffers[slot] = &buffer;
            //m_Version++; // TODO
        }

    }
    void Material::BindTexture(int slot, const Graphics::Texture& texture)
    {
        ASSERT(slot < m_Shader->GetShaderDesc().m_SrvCount);
        GraphicsManager::GetDevice()->CopyDescriptorsSimple(1,
            m_ResourceDescHeap->GetDescriptorHandle(slot), texture.GetSRV(),
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        //m_Version++; // TODO
    }

    void Material::BindSampler(int slot, const DescriptorHandle& sampler)
    {
        ASSERT(slot < m_Shader->GetShaderDesc().m_SamplerCount);
        GraphicsManager::GetDevice()->CopyDescriptorsSimple(1,
            m_SamplerDescHeap->GetDescriptorHandle(slot), sampler,
            D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        //m_Version++; // TODO
    }

    void Material::SetRenderTargets(const MultiRenderTargets* mrt)
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
            m_PipelineState->SetRenderTargetFormats(mrt->GetRenderTargetCount(), mrt->GetRenderTargetsFormat(), *mrt->GetDepthStencilFormat());
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

