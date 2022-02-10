#pragma once

namespace Graphics
{
    class SwapChain;
    class CommandQueue;
    class CommandList;

    extern winrt::com_ptr<IDXGIFactory7> g_Factory;
    extern winrt::com_ptr<ID3D12Device6> g_Device;
    extern CommandQueue g_GraphicsCommandQueue;
    extern SwapChain g_SwapChain;

    extern CommandList g_GraphicsCommandList;

    void Initialize();

    void OnRender();

    void OnDestroy();
}

namespace Graphics
{
    class GraphicManager
    {
    public:
        static GraphicManager& GetInstance();

    private:
        GraphicManager() {}


    };
}
