#pragma once

namespace D3D12Core
{
    class PlacedResourceDesc
    {
    public:
        PlacedResourceDesc() = default;
        virtual ~PlacedResourceDesc();
        PlacedResourceDesc(const PlacedResourceDesc& desc) = delete;
        PlacedResourceDesc(PlacedResourceDesc&& desc) = default;

        PlacedResourceDesc& operator = (const PlacedResourceDesc& desc) = delete;
        PlacedResourceDesc& operator = (PlacedResourceDesc&& desc) = default;

        UINT64 GetPlacedHeapOffset() const { return m_PlacedOrder * m_AllocationAlignment; }

        // 资源创建信息
        D3D12_HEAP_TYPE m_HeapType{};
        const D3D12_CLEAR_VALUE* m_OptimizedClearValue{};

        // 资源分配信息（从设备获取分配大小与对齐大小）
        UINT64 m_AllocationSize{};
        UINT64 m_AllocationAlignment{};

        // 资源放置信息（由放置堆填充）
        class PlacedHeap* m_PlacedHeapPtr{};
        UINT m_PlacedOrder{};         // 放置的定位位置

    private:

    };

    /**
     * @brief 放置堆
    */
    class PlacedHeap
    {
    public:
        PlacedHeap() = default;

        void Create(D3D12_HEAP_TYPE type, UINT64 size, D3D12_HEAP_FLAGS flags);

        bool PlacedResource(IGraphicsResource& resource);
        void ReleaseResource(UINT order);


        const D3D12_HEAP_DESC* GetHeapDesc() const { return &m_PlacedHeapDesc; }
        UINT64 GetHeapSize() const { return m_PlacedHeapDesc.SizeInBytes; }

        ID3D12Heap* GetPlacedHeap() const { return m_PlacedHeap.get(); }

        //virtual std::wstring GetName() const { return m_Name; }
        //virtual void SetName(const std::wstring& name)
        //{
        //    m_Name = std::wstring(name);
        //    if (m_PlacedHeap) SET_DEBUGNAME(m_PlacedHeap.get(), Application::Format(_T("%s (PlacedHeap)"), m_Name.c_str()));
        //}

    private:
        ComPtr<ID3D12Heap> m_PlacedHeap{};    // D3D12 放置堆对象
        D3D12_HEAP_DESC m_PlacedHeapDesc{};           // D3D12 放置堆描述

        UINT m_MinBlockSize{ D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT }; // 最小可分配内存块大小（对齐大小）
        UINT m_MaxOrderSize{}; // 放置堆容量

        Map<UINT, IGraphicsResource*> m_PlacedResources{}; // 已放置的资源
        Set<UINT> m_MemoryBlockOrders{}; // 已经定位的块索引

        //String m_Name{};
    };
}