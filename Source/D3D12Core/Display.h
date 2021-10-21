#pragma once

namespace Display
{
    constexpr UINT SWAP_FRAME_BACK_BUFFER_COUNT = 3; // 交换后缓冲数量

    void Initialize();
}

namespace Graphics
{
    extern winrt::com_ptr<IDXGISwapChain4> g_SwapChain;
}