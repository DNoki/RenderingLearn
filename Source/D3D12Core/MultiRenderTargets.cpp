#include "pch.h"

#include "RenderTexture.h"
#include "CommandList.h"

#include "MultiRenderTargets.h"

using namespace Resources;

namespace Graphics
{
    void MultiRenderTargets::SetRenderTarget(UINT slot, RenderTargetTexture* renderTexture)
    {
        ASSERT(slot < 8);
        if (slot < m_RenderTargets.size())
        {
            m_RenderTargets[slot] = renderTexture;
            m_RtvDescriptors[slot] = renderTexture->GetRTV();
            m_RenderTargetsFormat[slot] = renderTexture->GetFormat();
        }
        else
        {
            m_RenderTargets.push_back(renderTexture);
            m_RtvDescriptors.push_back(renderTexture->GetRTV());
            m_RenderTargetsFormat.push_back(renderTexture->GetFormat());
        }
    }

    void MultiRenderTargets::SetDepthStencil(DepthStencilTexture* renderTexture)
    {
        m_DepthStencil = renderTexture;
        m_DsvDescriptor = renderTexture->GetDSV();
        m_DepthStencilFormat = renderTexture->GetFormat();
    }

    UINT MultiRenderTargets::GetWidth() const
    {
        UINT width = 0;
        if (m_RenderTargets.size() > 0)
            width = m_RenderTargets.at(0)->GetWidth();
        else if (m_DepthStencil)
            width = m_DepthStencil->GetWidth();
        else throw L"ERROR::设置任何渲染目标。";
        return width;
    }
    UINT MultiRenderTargets::GetHeight() const
    {
        UINT height = 0;
        if (m_RenderTargets.size() > 0)
            height = m_RenderTargets.at(0)->GetHeight();
        else if (m_DepthStencil)
            height = m_DepthStencil->GetHeight();
        else throw L"ERROR::设置任何渲染目标。";
        return height;
    }

    void MultiRenderTargets::DispatchTransitionStates(const CommandList* commandList, D3D12_RESOURCE_STATES rtvState, D3D12_RESOURCE_STATES dsvState) const
    {
        ASSERT(commandList->GetType() == D3D12_COMMAND_LIST_TYPE_DIRECT);

        for (UINT i = 0; i < m_RenderTargets.size(); i++)
        {
            m_RenderTargets[i]->DispatchTransitionStates(commandList, rtvState);
        }
        if (m_DepthStencil)
        {
            m_DepthStencil->DispatchTransitionStates(commandList, dsvState);
        }
    }

    void MultiRenderTargets::DispatchClear(const CommandList* commandList)
    {
        ASSERT(commandList->GetType() == D3D12_COMMAND_LIST_TYPE_DIRECT);

        // 清空渲染目标
        for (UINT i = 0; i < m_RenderTargets.size(); i++)
        {
            commandList->ClearRenderTargetView(m_RtvDescriptors[i], *(Color*)(m_RenderTargets[i]->GetClearValue()->Color), 0, nullptr);
        }

        // 清空深度模板贴图
        if (m_DepthStencil)
        {
            const auto* depthStencilClearValue = m_DepthStencil->GetClearValue();
            D3D12_CLEAR_FLAGS clearFlags;
            switch (m_DepthStencil->GetDsvDesc()->Flags)
            {
            case D3D12_DSV_FLAG_NONE:
                clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
                break;
            case D3D12_DSV_FLAG_READ_ONLY_DEPTH:
                clearFlags = D3D12_CLEAR_FLAG_DEPTH;
                break;
            case D3D12_DSV_FLAG_READ_ONLY_STENCIL:
                clearFlags = D3D12_CLEAR_FLAG_STENCIL;
                break;
            default: throw; break;
            }

            commandList->ClearDepthStencilView(m_DsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, depthStencilClearValue->DepthStencil.Depth, depthStencilClearValue->DepthStencil.Stencil, 0, nullptr);
        }
    }
}
