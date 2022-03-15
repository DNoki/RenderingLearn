#pragma once

#include "DescriptorHeap.h"

constexpr UINT SWAP_FRAME_BACK_BUFFER_COUNT = 3; // 交换后缓冲数量
constexpr auto SWAP_CHAIN_RENDER_TARGET_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

namespace Resources
{
    class RenderTexture;
}

namespace Graphics
{
    class SwapChain
    {
    public:
        // --------------------------------------------------------------------------
        SwapChain() = default;
        SwapChain(const SwapChain& sc) = delete;
        SwapChain(SwapChain&& sc) = default;

        inline SwapChain& operator = (const SwapChain& sc) = delete;
        inline SwapChain& operator = (SwapChain&& sc) = default;

        // --------------------------------------------------------------------------
        inline IDXGISwapChain4* GetD3D12SwapChain() const { return m_SwapChain.get(); }

        /**
         * @brief 获取窗口宽度
         * @return
        */
        inline UINT GetWidth() const { return m_SwapChainDesc.Width; }
        /**
         * @brief 获取窗口高度
         * @return
        */
        inline UINT GetHeight() const { return m_SwapChainDesc.Height; }
        /**
         * @brief 获取窗口宽高比
         * @return
        */
        inline float GetScreenAspect() const { return GetWidth() / (float)GetHeight(); }

        // --------------------------------------------------------------------------
        /**
         * @brief 获取当前后台缓冲索引
         * @return
        */
        inline UINT GetCurrentBackBufferIndex() const
        {
            return m_SwapChain->GetCurrentBackBufferIndex();
        }
        /**
         * @brief 获取 RTV 描述符
         * @return
        */
        inline DescriptorHandle GetRtvDescHandle(UINT index) const
        {
            return m_RtvDescriptorHeap.GetDescriptorHandle(index);
        }
        /**
         * @brief 获取渲染目标贴图
         * @return
        */
        inline Resources::RenderTexture* GetRenderTarget(UINT index) const { return m_RenderTargets[index].get(); }

        inline DescriptorHandle GetDsvDescHandle() const { return m_DsvDescriptorHeap.GetDescriptorHandle(0); }
        inline Resources::RenderTexture* GetDepthStencil() const { return m_DepthStencils.get(); }

        // --------------------------------------------------------------------------
        /**
         * @brief 从窗口创建交换链
         * @param hwnd 窗口句柄
         * @param count 交换链数量
         * @param format 帧缓冲格式
        */
        void CreateForHwnd(HWND hwnd, UINT count, DXGI_FORMAT format);

        /**
         * @brief 调整渲染目标大小
         * @param width 宽度（为0时从窗口获取大小）
         * @param height 高度（为0时从窗口获取大小）
        */
        void Resize(UINT width = 0, UINT height = 0);

    private:
        winrt::com_ptr<IDXGISwapChain4> m_SwapChain{};    // 交换链
        DXGI_SWAP_CHAIN_DESC1 m_SwapChainDesc{};          // 交换链描述

        std::unique_ptr<DXGI_SWAP_CHAIN_FULLSCREEN_DESC> m_FullScreenDesc{}; // 全屏交换链描述

        DescriptorHeap m_RtvDescriptorHeap{}; // 渲染目标描述符堆
        std::vector<std::unique_ptr<Resources::RenderTexture>> m_RenderTargets{}; // 渲染目标贴图列表

        DescriptorHeap m_DsvDescriptorHeap{};
        std::unique_ptr<Resources::RenderTexture> m_DepthStencils{}; // 渲染目标贴图列表

        /**
         * @brief 重新生成渲染目标贴图
        */
        void RebuildRenderTargets();
    };
}