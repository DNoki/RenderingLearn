#include "pch.h"

#include "AppMain.h"
#include "GraphicsCore.h"
#include "CommandQueue.h"

#include "Display.h"

// DirectX 图形基础结构 (DXGI) ：最佳实践 https://docs.microsoft.com/zh-cn/windows/win32/direct3darticles/dxgi-best-practices

using namespace std;
using namespace Application;
using namespace Graphics;

#include "DescriptorHeap.h"
#include "Texture.h"
namespace Graphics
{
    extern unique_ptr<DescriptorHeap> g_RTVDescriptorHeap;
    extern vector<unique_ptr<RenderTexture>> g_RenderTargets;
}

namespace Display
{
    UINT g_DisplayWidth;
    UINT g_DisplayHeight;

    UINT g_CurrentBackBufferIndex;

    void Initialize()
    {
        ASSERT(g_Hwnd != nullptr);
        ASSERT(g_SwapChain == nullptr);

        if (g_Hwnd == NULL)
            throw L"ERROR::窗口句柄不能为0！";

        RECT clientRect;
        GetClientRect(g_Hwnd, &clientRect);
        g_DisplayWidth = clientRect.right - clientRect.left;
        g_DisplayHeight = clientRect.bottom - clientRect.top;

        // --------------------------------------------------------------------------
        // 创建交换链
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc{}; // 交换链描述
        swapChainDesc.Width = g_DisplayWidth;                           // 分辨率宽度
        swapChainDesc.Height = g_DisplayHeight;                         // 分辨率高度
        swapChainDesc.Format = SC_RENDER_TARGET_FORMAT;                 // 显示格式
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
            g_GraphicsCommandQueue.GetD3D12CommandQueue(), // 对于 Direct3D 12，这是一个指向直接命令队列的指针
            Application::g_Hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            (IDXGISwapChain1**)g_SwapChain.put()
        ));

        //auto backColor = DXGI_RGBA{ 1.0f, 1.0f, 1.0f, 1.0f };
        //g_SwapChain->SetBackgroundColor(&backColor);

        //auto currentBackBufferIndex = pIDXGISwapChain4->GetCurrentBackBufferIndex(); // 当前被绘制的后缓冲序号
    }

    float GetScreenAspect()
    {
        return g_DisplayWidth / (float)g_DisplayHeight;
    }

    UINT GetScreenWidth()
    {
        return g_DisplayWidth;
    }
    UINT GetScreenHeight()
    {
        return g_DisplayHeight;
    }

    void Resize(UINT width, UINT height)
    {
        // TODO 需要在这里等待指令队列完成
        g_GraphicsCommandQueue.WaitForQueueCompleted();

        g_DisplayWidth = width;
        g_DisplayHeight = height;

        g_RenderTargets.clear(); // 清除现有的渲染目标贴图

        // 重置交换链缓冲大小
        CHECK_HRESULT(g_SwapChain->ResizeBuffers(
            SWAP_FRAME_BACK_BUFFER_COUNT,
            g_DisplayWidth, g_DisplayHeight,
            SC_RENDER_TARGET_FORMAT,
            0));

        for (UINT i = 0; i < SWAP_FRAME_BACK_BUFFER_COUNT; i++)
        {
            g_RenderTargets.push_back(unique_ptr<RenderTexture>(new RenderTexture()));
            auto& rt = g_RenderTargets[i];
            rt->CreateFromSwapChain(i, nullptr, g_RTVDescriptorHeap->GetDescriptorHandle(i));
        }

        g_SwapChain->Present(1, 0); // 交换一次缓冲，使黑色填充屏幕

        // TODO 需要在这里等待指令队列完成
        g_GraphicsCommandQueue.WaitForQueueCompleted();

        TRACE("Changing display resolution to %ux%u", width, height);
    }
}