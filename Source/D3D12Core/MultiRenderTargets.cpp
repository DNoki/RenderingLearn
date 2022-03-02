#include "pch.h"

#include "RenderTexture.h"

#include "MultiRenderTargets.h"


namespace Graphics
{
    void MultiRenderTargets::SetRenderTarget(UINT slot, RenderTexture* renderTexture, D3D12_CPU_DESCRIPTOR_HANDLE rtv)
    {
        ASSERT(slot < 8);
        if (slot < m_RenderTargets.size())
        {
            m_RenderTargets[slot] = renderTexture;
            m_RtvDescriptors[slot] = rtv;
            m_RenderTargetsFormat[slot] = renderTexture->GetFormat();
        }
        else
        {
            m_RenderTargets.push_back(renderTexture);
            m_RtvDescriptors.push_back(rtv);
            m_RenderTargetsFormat.push_back(renderTexture->GetFormat());
        }
    }

    void MultiRenderTargets::SetDepthStencil(RenderTexture* renderTexture, D3D12_CPU_DESCRIPTOR_HANDLE dsv)
    {
        m_DepthStencil = renderTexture;
        m_DsvDescriptor = dsv;
        m_DepthStencilFormat = renderTexture->GetFormat();
    }

    UINT MultiRenderTargets::GetWidth() const
    {
        ASSERT(m_RenderTargets.size() > 0);
        return m_RenderTargets.at(0)->GetWidth();
    }
    UINT MultiRenderTargets::GetHeight() const
    {
        ASSERT(m_RenderTargets.size() > 0);
        return m_RenderTargets.at(0)->GetHeight();
    }
}
