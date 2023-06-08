#pragma once

namespace D3D12Core
{
    class IRenderTarget;

    class SwapChain
    {
    public:
        // --------------------------------------------------------------------------
        SwapChain() = default;
        //SwapChain(const SwapChain& sc) = delete;
        //SwapChain(SwapChain&& sc) = default;

        //SwapChain& operator = (const SwapChain& sc) = delete;
        //SwapChain& operator = (SwapChain&& sc) = default;

        // --------------------------------------------------------------------------
        /**
         * @brief 从窗口创建交换链
         * @param hwnd 窗口句柄
         * @param count 交换链数量
         * @param format 帧缓冲格式
        */
        void CreateForHwnd(const GraphicsContext& context, const class CommandQueue* commandQueue, HWND hwnd, UINT count, DXGI_FORMAT format);

        /**
         * @brief 调整渲染目标大小
         * @param width 宽度（为0时从窗口获取大小）
         * @param height 高度（为0时从窗口获取大小）
        */
        void Resize(UINT width = 0, UINT height = 0);

        // --------------------------------------------------------------------------
        IDXGISwapChain4* GetD3D12SwapChain() const { return m_SwapChain.get(); }

        /**
         * @brief 获取窗口宽度
         * @return
        */
        UINT GetWidth() const { return m_SwapChainDesc.Width; }
        /**
         * @brief 获取窗口高度
         * @return
        */
        UINT GetHeight() const { return m_SwapChainDesc.Height; }
        /**
         * @brief 获取窗口宽高比
         * @return
        */
        float GetScreenAspect() const { return GetWidth() / static_cast<float>(GetHeight()); }

        // --------------------------------------------------------------------------
        /**
         * @brief 获取当前后台缓冲索引
         * @return
        */
        UINT GetCurrentBackBufferIndex() const
        {
            return m_SwapChain->GetCurrentBackBufferIndex();
        }
        /**
         * @brief 获取渲染目标贴图
         * @return
        */
        IRenderTarget* GetRenderTarget(UINT index) const { return m_RenderTargets[index].get(); }

        // TODO temp
        const GraphicsContext* m_GraphicsContext{};
        const GraphicsContext* GetGraphicsContext() const { return m_GraphicsContext; }

    private:
        ComPtr<IDXGISwapChain4> m_SwapChain{};    // 交换链
        DXGI_SWAP_CHAIN_DESC1 m_SwapChainDesc{};          // 交换链描述

        UniquePtr<DXGI_SWAP_CHAIN_FULLSCREEN_DESC> m_FullScreenDesc{}; // 全屏交换链描述

        Vector<UniquePtr<IRenderTarget>> m_RenderTargets{}; // 渲染目标贴图列表

        bool m_VsyncEnable = false; // 垂直同步

        /**
         * @brief 重新生成渲染目标贴图
        */
        void RebuildRenderTargets();
    };
}