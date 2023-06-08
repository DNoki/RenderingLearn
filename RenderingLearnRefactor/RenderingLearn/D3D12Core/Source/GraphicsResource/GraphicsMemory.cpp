#include "pch.h"
#include "​GraphicsResource/GraphicsMemory.h"

#include "​GraphicsResource/PlacedHeap.h"

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

// --------------------------------------------------------------------------
/*
    图形内存设计思路
    模式一：对齐型放置
        将一块内存区域按对齐大小分割为一个个小区域块，放置资源时始终对齐到块大小
        优点：内存区域可以重复使用
        缺点：对于大量小资源可能会产生大量碎片
    模式二：紧凑型放置
        TODO 将资源紧密的放置在堆中，使用过的区域是一次性的

*/
// --------------------------------------------------------------------------

using namespace D3D12Core;

const UINT64 DEFAULT_HEAP_SIZE = 1000 * D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;


void GraphicsMemory::PlacedResource(IGraphicsResource& resource)
{
    // 获取资源分配需要的信息
    auto& resourceDesc = resource.GetResourceDesc();
    auto* placedDesc = resource.GetPlacedResourceDesc();

    ASSERT(placedDesc != nullptr);
    if (placedDesc == nullptr)
        return;

    auto allocationInfo = GraphicsContext::GetCurrentInstance()->GetDevice()->GetResourceAllocationInfo(GraphicsContext::GetCurrentInstance()->GetNodeMask(), 1, &resourceDesc);
    placedDesc->m_AllocationSize = allocationInfo.SizeInBytes;
    placedDesc->m_AllocationAlignment = allocationInfo.Alignment;

    // TODO 扩展 MSAA 的放置堆 D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT
    ASSERT(placedDesc->m_AllocationAlignment == D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
    // TODO 放置资源的大小超过了设计的堆大小
    ASSERT(placedDesc->m_AllocationSize <= DEFAULT_HEAP_SIZE);

    D3D12_HEAP_FLAGS heapFlags{};
    Vector<UniquePtr<PlacedHeap>>* heaps{};
    String resourceTypeName = TEXT("");
    switch (resourceDesc.Dimension)
    {
    case D3D12_RESOURCE_DIMENSION_BUFFER:
    {
        heaps = &(placedDesc->m_HeapType == D3D12_HEAP_TYPE_DEFAULT ? GetInstance().m_DefaultBufferHeaps : GetInstance().m_UploadBufferHeaps);
        heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
        resourceTypeName = TEXT("Buffer");
    }
    break;
    case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
    case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
    case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
    {
        ASSERT(placedDesc->m_HeapType == D3D12_HEAP_TYPE_DEFAULT);

        if (dynamic_cast<IRenderTarget*>(&resource))
        {
            heaps = &(GetInstance().m_RenderTextureHeaps);
            heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
            resourceTypeName = TEXT("RenderTexture");
        }
        else
        {
            heaps = &(GetInstance().m_TextureHeaps);
            heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
            resourceTypeName = TEXT("DefaultTexture");
        }
    }
    break;
    default: ASSERT(0); break;
    }

    auto pHeap = heaps->begin();
    for (; pHeap != heaps->end(); ++pHeap)
    {
        // 尝试将资源放入堆
        if ((**pHeap).PlacedResource(resource))
        {
            break;
        }
    }

    if (pHeap == heaps->end())
    {
        // 没有堆可以放置资源，创建一个新的堆
        heaps->push_back(UniquePtr<PlacedHeap>(new PlacedHeap()));
        auto& newHeap = *(heaps->back());

        newHeap.Create(placedDesc->m_HeapType, DEFAULT_HEAP_SIZE, heapFlags);

        static const String typeNames[] =
        {
            TEXT(""),
            TEXT("Default"),
            TEXT("Upload"),
            TEXT("Readback"),
            TEXT("Custom"),
        };
        GraphicsContext::SetDebugName(newHeap.GetPlacedHeap(), FORMAT(TEXT("%s %s"), typeNames[placedDesc->m_HeapType].c_str(), resourceTypeName.c_str()));

        if (!newHeap.PlacedResource(resource))
        {
            ASSERT(0);
        }
    }
}
