#pragma once

namespace Resources
{
    class RenderTargetTexture;
    class DepthStencilTexture;
}

namespace Graphics
{
    class CommandList;

    class MultiRenderTargets
    {
    public:
        MultiRenderTargets() = default;

        void SetRenderTarget(UINT slot, Resources::RenderTargetTexture* renderTexture);
        void SetDepthStencil(Resources::DepthStencilTexture* renderTexture);

        inline UINT GetRenderTargetCount() const { return static_cast<UINT>(m_RenderTargets.size()); }
        UINT GetWidth() const;
        UINT GetHeight() const;

        inline Resources::RenderTargetTexture* GetRenderTargets(UINT slot) const { return m_RenderTargets[slot]; }
        inline const D3D12_CPU_DESCRIPTOR_HANDLE* GetRtvDescriptors() const { return m_RtvDescriptors.data(); }
        inline const DXGI_FORMAT* GetRenderTargetsFormat() const { return m_RenderTargetsFormat.data(); }

        inline Resources::DepthStencilTexture* GetDepthStencil() const { return m_DepthStencil; }
        inline const D3D12_CPU_DESCRIPTOR_HANDLE* DsvDescriptor() const { return &m_DsvDescriptor; }
        inline const DXGI_FORMAT* GetDepthStencilFormat() const { return &m_DepthStencilFormat; }

        /**
         * @brief 切换所有渲染目标资源状态
         * @param commandList
         * @param state
        */
        void DispatchTransitionStates(const CommandList* commandList, D3D12_RESOURCE_STATES rtvState, D3D12_RESOURCE_STATES dsvState) const;
        /**
         * @brief 设置视口和剪切口
         * @param commandList
        */
        void DispatchViewportsAndScissor(const CommandList* commandList) const;
        /**
         * @brief 清空所有渲染目标
         * @param commandList
        */
        void DispatchClear(const CommandList* commandList, bool isClearColor = true, bool isClearDepth = true);

    private:
        std::vector<Resources::RenderTargetTexture*> m_RenderTargets{};
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_RtvDescriptors{};
        std::vector<DXGI_FORMAT> m_RenderTargetsFormat{};

        Resources::DepthStencilTexture* m_DepthStencil{};
        D3D12_CPU_DESCRIPTOR_HANDLE m_DsvDescriptor{};
        DXGI_FORMAT m_DepthStencilFormat{};

    };
}
