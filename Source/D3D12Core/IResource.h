#pragma once

namespace Graphics
{
    class GpuPlacedHeap;

    class PlacedResourceDesc
    {
    public:
        PlacedResourceDesc() = default;
        ~PlacedResourceDesc()
        {
            // TODO
        }

        // 资源创建信息
        D3D12_HEAP_TYPE m_HeapType;
        D3D12_RESOURCE_STATES m_InitialState;
        const D3D12_CLEAR_VALUE* m_OptimizedClearValue;

        // 资源分配信息（从设备获取分配大小与对齐大小）
        UINT64 m_AllocationSize;
        UINT64 m_AllocationAlignment;

        // 资源放置信息（由放置堆填充）
        const GpuPlacedHeap* m_PlacedHeapPtr;
        UINT m_PlacedHeapOffset;
        UINT m_PlacedOrderIndex;

    private:

    };

    /**
     * @brief D3D12 资源接口
     * @return
    */
    class IResource
    {
    public:
        // --------------------------------------------------------------------------
        IResource() = default;
        virtual ~IResource() = 0 {}

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

        PlacedResourceDesc m_PlacedResourceDesc;
    };

    class IBufferResource : public IResource
    {
    public:
        // --------------------------------------------------------------------------
        IBufferResource() {}
        virtual ~IBufferResource() = 0 {}

        // --------------------------------------------------------------------------
        /**
         * @brief 获取缓冲大小
         * @return
        */
        inline UINT64 GetBufferSize() const { return m_ResourceDesc.Width; }

        inline D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return m_GpuVirtualAddress; }

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

        void Finalize()
        {
            // Resource必须创建以后才可以完成初始化
            ASSERT(m_Resource != nullptr);
            // 仅缓冲资源可以获取 GPU 虚拟地址
            m_GpuVirtualAddress = m_Resource->GetGPUVirtualAddress();
        }
    };

    class ITexture : public IResource
    {
    public:
        ITexture() {}
        virtual ~ITexture() = 0 {}

    private:

    };
}
