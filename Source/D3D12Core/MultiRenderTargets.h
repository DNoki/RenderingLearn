#pragma once

namespace Resources
{
    class RenderTexture;
}

namespace Graphics
{
    class CommandList;

    class MultiRenderTargets
    {
    public:
        MultiRenderTargets() = default;

        void SetRenderTarget(UINT slot, Resources::RenderTexture* renderTexture, D3D12_CPU_DESCRIPTOR_HANDLE rtv);
        void SetDepthStencil(Resources::RenderTexture* renderTexture, D3D12_CPU_DESCRIPTOR_HANDLE dsv);

        inline UINT GetRenderTargetCount() const { return static_cast<UINT>(m_RenderTargets.size()); }
        UINT GetWidth() const;
        UINT GetHeight() const;

        inline Resources::RenderTexture* GetRenderTargets(UINT slot) const { return m_RenderTargets[slot]; }
        inline const D3D12_CPU_DESCRIPTOR_HANDLE* GetRtvDescriptors() const { return m_RtvDescriptors.data(); }
        inline const DXGI_FORMAT* GetRenderTargetsFormat() const { return m_RenderTargetsFormat.data(); }

        inline Resources::RenderTexture* GetDepthStencil() const { return m_DepthStencil; }
        inline const D3D12_CPU_DESCRIPTOR_HANDLE* DsvDescriptor() const { return &m_DsvDescriptor; }
        inline const DXGI_FORMAT* GetDepthStencilFormat() const { return &m_DepthStencilFormat; }

        /**
         * @brief 切换所有渲染目标资源状态
         * @param commandList 
         * @param state 
        */
        void DispatchRTsTransitionStates(const CommandList* commandList, D3D12_RESOURCE_STATES state) const;
        /**
         * @brief 清空所有渲染目标
         * @param commandList 
        */
        void DispatchClear(const CommandList* commandList);

    private:
        std::vector<Resources::RenderTexture*> m_RenderTargets{};
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_RtvDescriptors{};
        std::vector<DXGI_FORMAT> m_RenderTargetsFormat{};

        Resources::RenderTexture* m_DepthStencil{};
        D3D12_CPU_DESCRIPTOR_HANDLE m_DsvDescriptor{};
        DXGI_FORMAT m_DepthStencilFormat{};

    };
}
