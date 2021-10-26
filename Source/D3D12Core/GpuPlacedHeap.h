#pragma once

class GpuResource;

struct PlacedHandle
{
    UINT64 HeapOffset;
    UINT64 HeapSize;
};

/**
 * @brief 放置堆
*/
class GpuPlacedHeap
{
public:
    GpuPlacedHeap();

    void Create(D3D12_HEAP_TYPE type, UINT64 size, D3D12_HEAP_FLAGS flags);

    void PlacedResource(UINT64 offset, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES states, GpuResource& resource, const D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr);

    inline UINT64 GetHeapSize() { return m_PlacedHeapDesc.SizeInBytes; }
    UINT64 GetNextOffset();
    UINT64 GetNextOffset(const PlacedHandle& handle);

    inline ID3D12Heap* GetPlacedHeap() const { return m_PlacedHeap.get(); }

private:
    winrt::com_ptr<ID3D12Heap> m_PlacedHeap;
    CD3DX12_HEAP_DESC m_PlacedHeapDesc;

    bool m_IsMsaaAlignmentType; // 是否是 MSAA 资源大小对齐

    std::vector<PlacedHandle> m_PlacedResourceHandles;
};