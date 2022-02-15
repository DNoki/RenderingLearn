#pragma once

#include "IResource.h"

namespace Graphics
{
    class SwapChain;

    /**
     * @brief 渲染贴图（RTV、DSV）
    */
    class RenderTexture : public ITexture
    {
    public:
        RenderTexture() = default;

        /**
         * @brief 获取 RTV 描述
         * @return
        */
        inline const D3D12_RENDER_TARGET_VIEW_DESC* GetRtvDesc() const { return m_RtvDesc.get(); }
        /**
         * @brief 获取 DSV 描述
         * @return
        */
        inline const D3D12_DEPTH_STENCIL_VIEW_DESC* GetDsvDesc() const { return m_DsvDesc.get(); }

        /**
         * @brief 注册方式创建深度模板视图
         * @param format
         * @param width
         * @param height
         * @param optDepth
         * @param optStencil
        */
        void DirectCreateDSV(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth = 1.0f, UINT8 optStencil = 0);
        void PlacedCreateDSV(GpuPlacedHeap& pPlacedHeap, DXGI_FORMAT format, UINT64 width, UINT height, float optDepth = 1.0f, UINT8 optStencil = 0); // TODO

        /**
         * @brief 从交换链获取 RTV 缓冲
         * @param swapChain
         * @param index
        */
        void GetRtvFromSwapChain(const SwapChain& swapChain, UINT index);

    protected:
        /**
         * @brief 渲染目标视图描述
        */
        std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC> m_RtvDesc;
        /**
         * @brief 深度模板视图描述
        */
        std::unique_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC> m_DsvDesc;

    };
}