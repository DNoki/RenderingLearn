#pragma once

/**
 * @brief 放置堆
*/
class GpuPlacedHeap
{
public:
    GpuPlacedHeap();

    void Create(D3D12_HEAP_TYPE type, UINT64 size, D3D12_HEAP_FLAGS flags);

    void PlacedResource(D3D12_RESOURCE_STATES initialState, IPlacedObject& resource, const D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr);
    void PlacedResource(UINT64 offset, D3D12_RESOURCE_STATES initialState, IPlacedObject& resource, const D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr);


    inline const CD3DX12_HEAP_DESC* GetHeapDesc() const { return &m_PlacedHeapDesc; }
    inline UINT64 GetHeapSize() const { return m_PlacedHeapDesc.SizeInBytes; }

    inline ID3D12Heap* GetPlacedHeap() const { return m_PlacedHeap.get(); }

private:
    winrt::com_ptr<ID3D12Heap> m_PlacedHeap;
    CD3DX12_HEAP_DESC m_PlacedHeapDesc;

    bool m_IsMsaaAlignmentType; // 是否是 MSAA 资源大小对齐

    std::vector<IPlacedObject*> m_PlacedResources;
};

class SharedGraphicsMemory
{
public:

private:

};