#include "pch.h"

#include "GraphicsCore.h"
#include "GraphicsResource.h"
#include "GraphicsBuffer.h"

#include "GpuPlacedHeap.h"

// --------------------------------------------------------------------------
/*
    放置堆
    Direct3D 12 的新增，可以创建独立于资源的堆。之后，可以在单个堆中放置多个资源

    放置的资源是可用的最轻量级资源对象，创建和销毁速度最快。
    CreatePlacedResource类似于将保留资源完全映射到堆内的偏移量；但是与堆相关联的虚拟地址空间也可以重用。
    放置资源的创建和销毁比提交资源更轻。这是因为在这些操作期间没有创建或销毁堆。此外，与资源创建和销毁相比，放置的资源可以实现更轻量级的内存重用技术，即通过别名和别名屏障进行重用。多个放置的资源可以同时在同一个堆上相互重叠，但一次只能使用一个重叠资源。

    简单模型
        可以将放置的资源视为两种状态之一：活动或非活动。GPU 从非活动资源读取或写入是无效的。放置的资源在非活动状态下创建。

*/
// --------------------------------------------------------------------------

using namespace Graphics;

GpuPlacedHeap::GpuPlacedHeap() : m_PlacedHeap(nullptr), m_PlacedHeapDesc(), m_IsMsaaAlignmentType(false), m_PlacedResources()
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

void GpuPlacedHeap::PlacedResource(D3D12_RESOURCE_STATES initialState, IPlacedObject& resource, const D3D12_CLEAR_VALUE* pOptimizedClearValue)
{
    auto index = m_PlacedResources.size();
    auto size = static_cast<UINT>(index + 1);
    m_PlacedResources.push_back(&resource);

    std::vector<D3D12_RESOURCE_DESC> resourceDescs(size);
    for (auto i = 0u; i < size; i++)
        resourceDescs[i] = m_PlacedResources[i]->GetResourceDesc();
    std::vector<D3D12_RESOURCE_ALLOCATION_INFO1> resourceInfos(size);

    // 当需要放置多个资源时，可以直接利用 GetResourceAllocationInfo1 函数计算偏移
    g_Device->GetResourceAllocationInfo1(
        1u,
        size,                   // 资源描述的数量
        resourceDescs.data(),   // 资源描述列表
        resourceInfos.data());  // 用于填充资源描述额外信息


    // 查询要放置资源大小，并比较剩余可分配大小
    auto& resourceInfo = resourceInfos[index];
    ASSERT((resourceInfo.Offset + resourceInfo.SizeInBytes) <= m_PlacedHeapDesc.SizeInBytes, L"PlacedResource::指定资源无法放置到堆。");
    
    CHECK_HRESULT(g_Device->CreatePlacedResource(
        m_PlacedHeap.get(),     // 放置资源的堆
        resourceInfo.Offset,    // 资源的偏移量，必须是资源的对齐的倍数
        &resourceDescs[index],  // 资源描述
        initialState,           // 资源的初始状态
        pOptimizedClearValue,   // 描述用于优化特定资源的清除操作的值
        IID_PPV_ARGS(resource.PutD3D12Resource()))); // 要放置的资源
}

void GpuPlacedHeap::PlacedResource(UINT64 offset, D3D12_RESOURCE_STATES initialState, IPlacedObject& resource, const D3D12_CLEAR_VALUE* pOptimizedClearValue)
{
    // TODO 能否用Map来管理放置堆？
}