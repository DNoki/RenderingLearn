#include "pch.h"

#include "AppMain.h"
#include "GraphicsCore.h"
#include "CommandQueue.h"

#include "Display.h"

using namespace Graphics;

namespace Display
{

    void Initialize()
    {
        ASSERT(g_SwapChain == nullptr);

        // --------------------------------------------------------------------------
        // 创建交换链
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc{}; // 交换链描述
        swapChainDesc.Width = DEFAULT_SCREEN_WIDTH;                     // 分辨率宽度
        swapChainDesc.Height = DEFAULT_SCREEN_HEIGHT;                   // 分辨率高度
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;              // 显示格式
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // 后台缓冲区的表面使用情况和 CPU 访问选项
        swapChainDesc.BufferCount = SWAP_FRAME_BACK_BUFFER_COUNT;       // 交换链中缓冲区数量
        swapChainDesc.SampleDesc.Count = 1;                             // 多采样数量
        swapChainDesc.SampleDesc.Quality = 0;                           // 多采样质量
        swapChainDesc.Scaling = DXGI_SCALING_NONE;                      // 标识调整行为
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;       // 交换链使用的表示模型和用于在呈现表面后处理演示缓冲区内容的选项
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;               // 交换链后台缓冲区的透明度行为
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;   // 指定交换链行为

        //DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc  {};
        //fsSwapChainDesc.Windowed = TRUE;

        CHECK_HRESULT(g_Factory->CreateSwapChainForHwnd(
            g_GraphicsCommandQueue->GetD3D12CommandQueue(), // 对于 Direct3D 12，这是一个指向直接命令队列的指针
            Application::g_Hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            (IDXGISwapChain1**)g_SwapChain.put()
        ));

        //auto currentBackBufferIndex = pIDXGISwapChain4->GetCurrentBackBufferIndex(); // 当前被绘制的后缓冲序号
    }
}