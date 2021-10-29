#pragma once

class CommandQueue;
class CommandList;

namespace Graphics
{
    extern winrt::com_ptr<IDXGIFactory7> g_Factory;
    extern winrt::com_ptr<ID3D12Device6> g_Device;
    extern CommandQueue g_GraphicsCommandQueue;

    extern CommandList g_GraphicCommandList;

    void Initialize();

    void OnRender();

    void OnDestroy();
}