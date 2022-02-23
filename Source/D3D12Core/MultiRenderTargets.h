#pragma once

namespace Graphics
{
    class RenderTexture;

    class MultiRenderTargets
    {
    public:
        MultiRenderTargets() = default;

        inline void SetRenderTarget(UINT slot, D3D12_CPU_DESCRIPTOR_HANDLE rt, DXGI_FORMAT format)
        {
            ASSERT(slot < 8);
            if (slot < m_RenderTargets.size())
            {
                m_RenderTargets[slot] = rt;
                m_RenderTargetsFormat[slot] = format;
            }
            else
            {
                m_RenderTargets.push_back(rt);
                m_RenderTargetsFormat.push_back(format);
            }
        }
        inline void SetDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE ds, DXGI_FORMAT format)
        {
            m_DepthStencil = ds;
            m_DepthStencilFormat = format;
        }

        inline UINT GetRenderTargetCount() const { return static_cast<UINT>(m_RenderTargets.size()); }

        inline const D3D12_CPU_DESCRIPTOR_HANDLE* GetRenderTargets() const { return m_RenderTargets.data(); }
        inline const DXGI_FORMAT* GetRenderTargetsFormat() const { return m_RenderTargetsFormat.data(); }

        inline const D3D12_CPU_DESCRIPTOR_HANDLE* GetDepthStencil() const { return &m_DepthStencil; }
        inline const DXGI_FORMAT* GetDepthStencilFormat() const { return &m_DepthStencilFormat; }

    private:
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_RenderTargets;
        std::vector<DXGI_FORMAT> m_RenderTargetsFormat;

        D3D12_CPU_DESCRIPTOR_HANDLE m_DepthStencil;
        DXGI_FORMAT m_DepthStencilFormat;

    };
}
