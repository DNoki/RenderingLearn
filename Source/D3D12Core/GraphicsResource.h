#pragma once

namespace Graphics
{
    class PlacedHeap;

    class PlacedResourceDesc
    {
    public:
        PlacedResourceDesc() = default;
        virtual ~PlacedResourceDesc();

        inline UINT64 GetPlacedHeapOffset() const { return m_PlacedOrder * m_AllocationAlignment; }

        // 资源创建信息
        D3D12_HEAP_TYPE m_HeapType;
        D3D12_RESOURCE_STATES m_InitialState;
        const D3D12_CLEAR_VALUE* m_OptimizedClearValue;

        // 资源分配信息（从设备获取分配大小与对齐大小）
        UINT64 m_AllocationSize;
        UINT64 m_AllocationAlignment;

        // 资源放置信息（由放置堆填充）
        PlacedHeap* m_PlacedHeapPtr;
        UINT m_PlacedOrder;         // 放置的定位位置

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
        inline GraphicsResource() { ZeroMemory(this, sizeof(GraphicsResource)); }
        virtual ~GraphicsResource() = 0 {}

        // --------------------------------------------------------------------------
        inline const D3D12_RESOURCE_DESC& GetResourceDesc() const noexcept
        {
            return m_ResourceDesc;
        }
        inline ID3D12Resource1* GetD3D12Resource() const noexcept
        {
            return m_Resource.get();
        }
        inline PlacedResourceDesc* GetPlacedResourceDesc() noexcept
        {
            return &m_PlacedResourceDesc;
        }

        inline void SetResourceDesc(const D3D12_RESOURCE_DESC& desc) noexcept
        {
            m_ResourceDesc = desc;
        }
        inline void SetResourceDesc(const D3D12_RESOURCE_DESC&& desc) noexcept
        {
            m_ResourceDesc = desc;
        }
        inline ID3D12Resource1** PutD3D12Resource() noexcept
        {
            return m_Resource.put();
        }

    protected:
        // 资源对象
        // 封装了 CPU 和 GPU 读取和写入物理内存或堆的通用能力。
        winrt::com_ptr<ID3D12Resource1> m_Resource;
        // 资源描述
        D3D12_RESOURCE_DESC m_ResourceDesc;

        // GPU 内存中的虚拟地址
        // IBV、VBV 等直接调用资源类型时使用
        D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;

        // 放置资源描述
        PlacedResourceDesc m_PlacedResourceDesc;
    };

    class IBufferResource
    {
    public:
        // --------------------------------------------------------------------------
        virtual ~IBufferResource() = 0 {}

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
        virtual ~Texture() = 0 {}

    };
}
