#pragma once

namespace Graphics
{
    class CommandQueue;
    class SwapChain;

    class GraphicsManager
    {
    public:
        inline static GraphicsManager& GetInstance() { return s_GraphicsManager; }

        static IDXGIFactory7* GetFactory();
        static ID3D12Device6* GetDevice();

        static CommandQueue* GetGraphicsCommandQueue();
        static CommandQueue* GetComputeCommandQueue();
        static CommandQueue* GetCopyCommandQueue();

        static SwapChain* GetSwapChain();

        void Initialize();
        void Destory();

    private:
        static GraphicsManager s_GraphicsManager;

        GraphicsManager() = default;

    };
}
