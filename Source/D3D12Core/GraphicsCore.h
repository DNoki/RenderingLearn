#pragma once

class SwapChain;
class CommandQueue;
class CommandList;

namespace Graphics
{
    extern winrt::com_ptr<IDXGIFactory7> g_Factory;
    extern winrt::com_ptr<ID3D12Device6> g_Device;
    extern CommandQueue g_GraphicsCommandQueue;
    extern SwapChain g_SwapChain;

    extern CommandList g_GraphicsCommandList;

    extern UINT64 g_FrameCount;

    void Initialize();

    void OnRender();

    void OnDestroy();
}

class GraphicManager
{
public:
    static GraphicManager& GetInstance();

private:
    GraphicManager() {}


};
