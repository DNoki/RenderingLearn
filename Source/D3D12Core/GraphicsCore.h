#pragma once

class CommandQueue;

namespace Graphics
{
    extern winrt::com_ptr<IDXGIFactory7> g_Factory;
    extern winrt::com_ptr<ID3D12Device6> g_Device;
    extern std::unique_ptr<CommandQueue> g_GraphicsCommandQueue;

    void Initialize();

    void OnRender();

    void OnDestroy();
}