﻿#pragma once

namespace D3D12Core
{
    class GraphicsContext
    {
    public:
        GraphicsContext() = default;

        void Initialize();
        void Destory();

        [[nodiscard]] IDXGIFactory7* GetFactory() const { return m_Factory.get(); }
        [[nodiscard]] IDXGIAdapter4* GetAdapter() const { return m_Adapter.get(); }
        [[nodiscard]] ID3D12Device6* GetDevice() const { return m_Device.get(); }

        const DXGI_ADAPTER_DESC3& GetAdapterDesc() { return m_AdapterDesc; }

        void SetAltEnterEnable(HWND winHandle, bool enable);

        UINT GetNodeMask() const { return 0; }

    public:
        static void SetDebugName(IDXGIObject* pObj, const String& name);
        static void SetDebugName(ID3D12Object* pObj, const String& name);

    private:
#if DEBUG
        winrt::com_ptr<ID3D12Debug3> m_D3D12Debug{};
#endif // DEBUG
        winrt::com_ptr<IDXGIFactory7> m_Factory{};
        winrt::com_ptr<IDXGIAdapter4> m_Adapter{};
        winrt::com_ptr<ID3D12Device6> m_Device{};

        DXGI_ADAPTER_DESC3 m_AdapterDesc;

    };
}
