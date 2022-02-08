#pragma once

#if 0
namespace Display
{
    constexpr UINT SWAP_FRAME_BACK_BUFFER_COUNT = 3; // 交换后缓冲数量
    constexpr auto SC_RENDER_TARGET_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

    extern UINT g_CurrentBackBufferIndex;

    void Initialize();

    float GetScreenAspect();
    UINT GetScreenWidth();  // 获取绘制屏幕宽度
    UINT GetScreenHeight(); // 获取绘制屏幕高度

    void Resize(UINT width, UINT height);
}

namespace Graphics
{
    extern winrt::com_ptr<IDXGISwapChain4> g_SwapChain; // TODO 封装交换链
}
#endif

class SwapChain
{
public:
    SwapChain() = default;

    inline UINT GetWidth() { return m_SwapChainDesc.Width; }
    inline UINT GetHeight() { return m_SwapChainDesc.Height; }

    void CreateForHwnd(HWND hwnd, UINT count, DXGI_FORMAT format);

    void Resize(UINT width, UINT height);

private:

    winrt::com_ptr<IDXGISwapChain4> g_SwapChain;    // 交换链
    DXGI_SWAP_CHAIN_DESC1 m_SwapChainDesc;          // 交换链描述

    std::unique_ptr<DXGI_SWAP_CHAIN_FULLSCREEN_DESC> m_FullScreenDesc; // 全屏交换链描述

};