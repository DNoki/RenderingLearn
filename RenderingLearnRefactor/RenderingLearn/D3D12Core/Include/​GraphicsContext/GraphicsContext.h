#pragma once

namespace D3D12Core
{
    class GraphicsContext
    {
    public:
        // TODO temp
        inline static GraphicsContext* CurrentInstance = nullptr;
        static GraphicsContext* GetCurrentInstance() { return CurrentInstance; }

        GraphicsContext() = default;

        void Initialize();
        void Destroy();

        IDXGIFactory7* GetFactory() const { return m_Factory.get(); }
        IDXGIAdapter4* GetAdapter() const { return m_Adapter.get(); }
        ID3D12Device6* GetDevice() const { return m_Device.get(); }

        const DXGI_ADAPTER_DESC3& GetAdapterDesc() const { return m_AdapterDesc; }

        void SetAltEnterEnable(HWND winHandle, bool enable);

        UINT GetNodeMask() const { return 0; }

    public:
        static void SetDebugName(IDXGIObject* pObj, const String& name);
        static void SetDebugName(ID3D12Object* pObj, const String& name);

    private:
#if DEBUG
        ComPtr<ID3D12Debug3> m_D3D12Debug{};
#endif // DEBUG
        ComPtr<IDXGIFactory7> m_Factory{};
        ComPtr<IDXGIAdapter4> m_Adapter{};
        ComPtr<ID3D12Device6> m_Device{};

        DXGI_ADAPTER_DESC3 m_AdapterDesc{};

    };
}
