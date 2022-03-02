#pragma once

namespace Graphics
{
    class RenderTexture;

    class MultiRenderTargets
    {
    public:
        MultiRenderTargets() = default;

        void SetRenderTarget(UINT slot, RenderTexture* renderTexture, D3D12_CPU_DESCRIPTOR_HANDLE rtv);
        void SetDepthStencil(RenderTexture* renderTexture, D3D12_CPU_DESCRIPTOR_HANDLE dsv);

        inline UINT GetRenderTargetCount() const { return static_cast<UINT>(m_RenderTargets.size()); }
        UINT GetWidth() const;
        UINT GetHeight() const;

        inline RenderTexture* GetRenderTargets(UINT slot) const { return m_RenderTargets[slot]; }
        inline const D3D12_CPU_DESCRIPTOR_HANDLE* GetRtvDescriptors() const { return m_RtvDescriptors.data(); }
        inline const DXGI_FORMAT* GetRenderTargetsFormat() const { return m_RenderTargetsFormat.data(); }

        inline RenderTexture* GetDepthStencil() const { return m_DepthStencil; }
        inline const D3D12_CPU_DESCRIPTOR_HANDLE* DsvDescriptor() const { return &m_DsvDescriptor; }
        inline const DXGI_FORMAT* GetDepthStencilFormat() const { return &m_DepthStencilFormat; }

    private:
        std::vector<RenderTexture*> m_RenderTargets;
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_RtvDescriptors;
        std::vector<DXGI_FORMAT> m_RenderTargetsFormat;

        RenderTexture* m_DepthStencil;
        D3D12_CPU_DESCRIPTOR_HANDLE m_DsvDescriptor;
        DXGI_FORMAT m_DepthStencilFormat;

    };
}
