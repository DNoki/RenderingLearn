#pragma once

namespace Graphics
{
    class PlacedHeap;
    class CommandList;

    class PlacedResourceDesc
    {
    public:
        PlacedResourceDesc() = default;
        virtual ~PlacedResourceDesc();
        PlacedResourceDesc(const PlacedResourceDesc& desc) = delete;
        PlacedResourceDesc(PlacedResourceDesc&& desc) = default;

        inline PlacedResourceDesc& operator = (const PlacedResourceDesc& desc) = delete;
        inline PlacedResourceDesc& operator = (PlacedResourceDesc&& desc) = default;

        inline UINT64 GetPlacedHeapOffset() const { return m_PlacedOrder * m_AllocationAlignment; }

        // 资源创建信息
        D3D12_HEAP_TYPE m_HeapType{};
        const D3D12_CLEAR_VALUE* m_OptimizedClearValue{};

        // 资源分配信息（从设备获取分配大小与对齐大小）
        UINT64 m_AllocationSize{};
        UINT64 m_AllocationAlignment{};

        // 资源放置信息（由放置堆填充）
        PlacedHeap* m_PlacedHeapPtr{};
        UINT m_PlacedOrder{};         // 放置的定位位置

    private:

    };

    /**
     * @brief D3D12 资源抽象类
     * @return
    */
    class GraphicsResource
    {
    public:
        // --------------------------------------------------------------------------
        GraphicsResource() = default;
        virtual ~GraphicsResource() = 0 {}
        GraphicsResource(const GraphicsResource& res) = delete;
        GraphicsResource(GraphicsResource&& res) = default;

        inline GraphicsResource& operator = (const GraphicsResource& res) = delete;
        inline GraphicsResource& operator = (GraphicsResource&& res) = default;

        // --------------------------------------------------------------------------
        inline const D3D12_RESOURCE_DESC& GetResourceDesc() const noexcept
        {
            return m_ResourceDesc;
        }
        inline ID3D12Resource1* GetD3D12Resource() const noexcept
        {
            return m_Resource.get();
        }
        inline D3D12_RESOURCE_STATES GetResourceStates() const noexcept { return m_ResourceStates; }
        inline PlacedResourceDesc* GetPlacedResourceDesc() noexcept
        {
            return &m_PlacedResourceDesc;
        }

        inline ID3D12Resource1** PutD3D12Resource() noexcept
        {
            return m_Resource.put();
        }

        inline virtual std::wstring GetName() const { return m_Name; }
        inline virtual void SetName(const std::wstring& name)
        {
            m_Name = std::wstring(name);
            if (m_Resource) SET_DEBUGNAME(m_Resource.get(), Application::Format(_T("%s (Resource)"), m_Name.c_str()));
        }

    protected:
        // 资源对象
        // 封装了 CPU 和 GPU 读取和写入物理内存或堆的通用能力。
        winrt::com_ptr<ID3D12Resource1> m_Resource{};
        // 资源描述
        D3D12_RESOURCE_DESC m_ResourceDesc{};
        D3D12_RESOURCE_STATES m_ResourceStates{}; // 资源状态

        // GPU 内存中的虚拟地址
        // IBV、VBV 等直接调用资源类型时使用
        D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress{};

        // 放置资源描述
        PlacedResourceDesc m_PlacedResourceDesc{};

        // 资源名称
        std::wstring m_Name{};

    };

    class IBufferResource
    {
    public:
        // --------------------------------------------------------------------------
        IBufferResource() = default;
        virtual ~IBufferResource() = 0 {}
        IBufferResource(const IBufferResource& buffer) = delete;
        IBufferResource(IBufferResource&& buffer) = default;

        inline IBufferResource& operator = (const IBufferResource& buffer) = delete;
        inline IBufferResource& operator = (IBufferResource&& buffer) = default;

        // --------------------------------------------------------------------------
        /**
         * @brief 获取缓冲大小
         * @return
        */
        virtual UINT64 GetBufferSize() const = 0;

        virtual D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const = 0;

        /**
         * @brief 创建一个资源并为其分配内存
         * @param size
        */
        virtual void DirectCreate(UINT64 size) = 0;
        /**
         * @brief 使用定位方式创建一个资源
         * @param size
         * @param pPlacedHeap
        */
        virtual void PlacedCreate(UINT64 size) = 0;

    protected:
        virtual void Finalize() = 0;

    };

    class Texture : public GraphicsResource
    {
    public:
        virtual ~Texture() override = 0 {}

        inline DXGI_FORMAT GetFormat() const { return m_ResourceDesc.Format; }
        inline UINT GetWidth() const { return static_cast<UINT>(m_ResourceDesc.Width); }
        inline UINT GetHeight() const { return static_cast<UINT>(m_ResourceDesc.Height); }

        /**
         * @brief 改变资源状态
         * @param commandList 图形命令列表
         * @param after 要改变的状态
        */
        void DispatchTransitionStates(const CommandList* commandList, D3D12_RESOURCE_STATES after);

    };
}
