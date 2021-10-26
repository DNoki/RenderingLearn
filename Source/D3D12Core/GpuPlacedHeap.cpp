#include "pch.h"

#include "GraphicsCore.h"
#include "GpuResource.h"

#include "GpuPlacedHeap.h"

// --------------------------------------------------------------------------
/*
    放置堆
    Direct3D 12 的新增，可以创建独立于资源的堆。之后，可以在单个堆中放置多个资源

    放置的资源是可用的最轻量级资源对象，创建和销毁速度最快。

*/
// --------------------------------------------------------------------------

using namespace Graphics;

GpuPlacedHeap::GpuPlacedHeap() : m_PlacedHeap(nullptr), m_PlacedHeapDesc(), m_IsMsaaAlignmentType(false), m_PlacedResourceHandles()
{
}

void GpuPlacedHeap::Create(D3D12_HEAP_TYPE type, UINT64 size, D3D12_HEAP_FLAGS flags)
{
    m_IsMsaaAlignmentType = false;
    m_PlacedHeapDesc = CD3DX12_HEAP_DESC(
        UINT_UPPER(size, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), // 计算边界对齐后大小
        type, // 放置堆类型
        D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
        flags); // 放置堆选项，例如堆是否可以包含纹理

    CHECK_HRESULT(g_Device->CreateHeap(&m_PlacedHeapDesc, IID_PPV_ARGS(m_PlacedHeap.put())));
}

void GpuPlacedHeap::PlacedResource(UINT64 offset, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES states, GpuResource& resource, const D3D12_CLEAR_VALUE* pOptimizedClearValue)
{
    // TODO 在这里查询要放置资源大小，并比较剩余可分配大小
    // 当需要放置多个资源时，可以直接利用 GetResourceAllocationInfo1 函数计算偏移
    D3D12_RESOURCE_ALLOCATION_INFO1 rai1[1]{};
    g_Device->GetResourceAllocationInfo1(
        1,
        1,      // 资源描述的数量
        pDesc,  // 资源描述列表
        rai1);  // 用于填充资源描述额外信息

    for (auto i = 0; i < _countof(rai1); i++)
        ASSERT((rai1[i].Offset % rai1[i].Alignment) == 0);

    CHECK_HRESULT(g_Device->CreatePlacedResource(
        m_PlacedHeap.get(), // 放置资源的堆
        offset,             // 资源的偏移量，必须是资源的对齐的倍数
        pDesc,              // 资源描述
        states,             // 资源的初始状态
        pOptimizedClearValue, // 描述用于优化特定资源的清除操作的值
        IID_PPV_ARGS(resource.PutD3D12Resource()))); // 要放置的资源

    auto size = GetRequiredIntermediateSize(resource.GetD3D12Resource(), 0, 1);

    auto handle = PlacedHandle
    {
        offset,
        size,
    };
    m_PlacedResourceHandles.push_back(handle);
}

UINT64 GpuPlacedHeap::GetNextOffset()
{
    UINT64 offset = 0;
    if (m_PlacedResourceHandles.size() > 0)
    {
        auto& endResource = *m_PlacedResourceHandles.end()._Ptr;
        return GetNextOffset(endResource);
    }
    return offset;
}

UINT64 GpuPlacedHeap::GetNextOffset(const PlacedHandle& handle)
{
    UINT64 offset = handle.HeapOffset;
    offset += UINT64_UPPER(handle.HeapSize, (m_IsMsaaAlignmentType ? D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT : D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT));

    return offset;
}
