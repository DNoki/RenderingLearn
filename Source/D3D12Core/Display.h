#pragma once

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
    extern winrt::com_ptr<IDXGISwapChain4> g_SwapChain;
}