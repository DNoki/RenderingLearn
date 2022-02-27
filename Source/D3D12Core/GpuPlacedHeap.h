#pragma once

namespace Graphics
{
    class IResource;

    /**
     * @brief 放置堆
    */
    class GpuPlacedHeap
    {
    public:
        GpuPlacedHeap();

        void Create(D3D12_HEAP_TYPE type, UINT64 size, D3D12_HEAP_FLAGS flags);

#if 0
        void PlacedResource(D3D12_RESOURCE_STATES initialState, IResource& resource, const D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr);
        void PlacedResource(UINT64 offset, D3D12_RESOURCE_STATES initialState, IResource& resource, const D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr);
#endif
        bool PlacedResource(IResource& resource);


        inline const CD3DX12_HEAP_DESC* GetHeapDesc() const { return &m_PlacedHeapDesc; }
        inline UINT64 GetHeapSize() const { return m_PlacedHeapDesc.SizeInBytes; }

        inline ID3D12Heap* GetPlacedHeap() const { return m_PlacedHeap.get(); }

    private:
        winrt::com_ptr<ID3D12Heap> m_PlacedHeap;
        CD3DX12_HEAP_DESC m_PlacedHeapDesc;

        UINT m_MinBlockSize;

        //std::vector<IResource*> m_PlacedResources;

        UINT m_MaxOrder;
        std::map<UINT, IResource*> m_PlacedResources;
        std::set<UINT> m_MemoryBlocks;
    };

    class GraphicsMemory
    {
    public:
        static void PlacedResource(IResource& resource);

    private:
        static GraphicsMemory g_GraphicsMemory;
        static GraphicsMemory& GetInstance() { return g_GraphicsMemory; }

        std::vector<GpuPlacedHeap> m_SharedBufferHeaps;
        std::vector<GpuPlacedHeap> m_BufferHeaps;
        std::vector<GpuPlacedHeap> m_TextureHeaps;
    };
}