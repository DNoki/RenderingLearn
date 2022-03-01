#pragma once

#include "CommandQueue.h"
#include "Display.h"

constexpr UINT NODEMASK = 0; // TODO 多适配器系统 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/multi-engine

namespace Graphics
{
    class CommandList;

    extern CommandList g_GraphicsCommandList;

    void Initialize();

    void OnRender();


    class GraphicsManager
    {
    public:
        inline static GraphicsManager& GetInstance() { return m_GraphicsManager; }

        static IDXGIFactory7* GetFactory();
        static ID3D12Device6* GetDevice();

        static CommandQueue* GetGraphicsCommandQueue();
        static CommandQueue* GetComputeCommandQueue();
        static CommandQueue* GetCopyCommandQueue();

        static SwapChain* GetSwapChain();

        void Initialize();
        void Destory();

    private:
        static GraphicsManager m_GraphicsManager;

        GraphicsManager() {}


    };
}
