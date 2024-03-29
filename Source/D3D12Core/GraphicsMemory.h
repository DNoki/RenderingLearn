﻿#pragma once

namespace Graphics
{
    class IResource;

    /**
     * @brief 放置堆
    */
    class PlacedHeap
    {
    public:
        PlacedHeap() = default;

        void Create(D3D12_HEAP_TYPE type, UINT64 size, D3D12_HEAP_FLAGS flags);

        bool PlacedResource(IResource& resource);
        void ReleaseResource(UINT order);


        inline const D3D12_HEAP_DESC* GetHeapDesc() const { return &m_PlacedHeapDesc; }
        inline UINT64 GetHeapSize() const { return m_PlacedHeapDesc.SizeInBytes; }

        inline ID3D12Heap* GetPlacedHeap() const { return m_PlacedHeap.get(); }

    private:
        winrt::com_ptr<ID3D12Heap> m_PlacedHeap;    // D3D12 放置堆对象
        D3D12_HEAP_DESC m_PlacedHeapDesc;           // D3D12 放置堆描述

        UINT m_MinBlockSize; // 最小可分配内存块大小（对齐大小）
        UINT m_MaxOrderSize; // 放置堆容量

        std::map<UINT, IResource*> m_PlacedResources; // 已放置的资源
        std::set<UINT> m_MemoryBlockOrders; // 已经定位的块索引
    };

    class GraphicsMemory
    {
    public:
        /**
         * @brief 放置紧凑型资源
         * @param resource
        */
        static void CompactPlacedResource(IResource& resource); // TODO
        /**
         * @brief 对齐放置资源
         * @param resource
        */
        static void PlacedResource(IResource& resource);

    private:
        static GraphicsMemory g_GraphicsMemory;
        static GraphicsMemory& GetInstance() { return g_GraphicsMemory; }

#ifdef DONT_USE_SMART_PTR
        std::vector<PlacedHeap> m_UploadBufferHeaps;     // 上传缓冲堆
        std::vector<PlacedHeap> m_DefaultBufferHeaps;    // 默认缓冲堆
        std::vector<PlacedHeap> m_TextureHeaps;          // 非渲染目标贴图缓冲堆
#else
        std::vector<std::unique_ptr<PlacedHeap>> m_UploadBufferHeaps;     // 上传缓冲堆
        std::vector<std::unique_ptr<PlacedHeap>> m_DefaultBufferHeaps;    // 默认缓冲堆
        std::vector<std::unique_ptr<PlacedHeap>> m_TextureHeaps;          // 非渲染目标贴图缓冲堆
#endif
    };
}