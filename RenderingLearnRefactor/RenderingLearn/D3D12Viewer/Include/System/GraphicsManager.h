#pragma once

namespace D3D12Viewer
{
    class GraphicsManager
    {
    public:
        UINT SwapFrameBackBufferCount = 3; // 交换链后缓冲数量
        DXGI_FORMAT SwapChainRenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    public:
        static GraphicsManager& GetInstance() { return s_Instance; }

        void Initialize(HWND windowHandle);
        void Destory();

        D3D12Core::GraphicsContext* GetGraphicsContext() { return m_GraphicsContext.get(); }
        D3D12Core::CommandQueue* GetGraphicsCommandQueue() { return m_GraphicsCommandQueue.get(); }
        D3D12Core::SwapChain* GetSwapChain() { return m_SwapChain.get(); }

    private:
        static GraphicsManager s_Instance;

        UniquePtr<D3D12Core::GraphicsContext> m_GraphicsContext;

        UniquePtr<D3D12Core::CommandQueue> m_GraphicsCommandQueue;
        UniquePtr<D3D12Core::CommandQueue> m_ComputeCommandQueue;
        UniquePtr<D3D12Core::CommandQueue> m_CopyCommandQueue;

        UniquePtr<D3D12Core::SwapChain> m_SwapChain;


        GraphicsManager() = default;

    };
}
