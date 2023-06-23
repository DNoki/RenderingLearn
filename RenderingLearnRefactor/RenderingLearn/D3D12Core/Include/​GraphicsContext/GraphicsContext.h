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

        /**
         * \brief 获取显卡设备数量
         */
        uint32 GetGpuCount() const;
        /**
         * \brief 获取显卡设备名称
         */
        String GetGpuDescription(uint32 Node = 0) const;
        /**
         * \brief 获取 GPU 显存大小
         */
        uint64 GetGpuDedicatedMemory(uint32 Node = 0) const;
        /**
         * \brief 获取 GPU 共享内存大小
         */
        uint64 GetGpuSharedMemory(uint32 Node = 0) const;
        /**
         * \brief 获取 GPU 分配给此应用的内存预算
         */
        uint64 GetGpuMemoryBudget(uint32 Node = 0) const;
        /**
         * \brief 获取当前已使用 GPU 内存的大小
         */
        uint64 GetGpuUsagedMemory(uint32 Node = 0) const;


        void SetAltEnterEnable(HWND winHandle, bool enable);

        uint32 GetNodeMask() const { return 0; }

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

        Vector<DXGI_ADAPTER_DESC3> m_AdapterDesc{};

    };
}
