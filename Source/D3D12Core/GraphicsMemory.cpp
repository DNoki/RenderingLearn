#include "pch.h"

#include "IResource.h"
#include "GraphicsCore.h"

#include "GraphicsMemory.h"

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

using namespace std;

namespace Graphics
{
    constexpr UINT64 DEFAULT_HEAP_SIZE = 1000 * D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

    void PlacedHeap::Create(D3D12_HEAP_TYPE type, UINT64 size, D3D12_HEAP_FLAGS flags)
    {
        m_MinBlockSize = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

        m_PlacedHeapDesc = CD3DX12_HEAP_DESC(
            UINT_UPPER(size, m_MinBlockSize), // 计算边界对齐后大小
            type, // 放置堆类型
            m_MinBlockSize, // 最小块大小（对齐大小）
            flags); // 放置堆选项，例如堆是否可以包含纹理

        CHECK_HRESULT(g_Device->CreateHeap(&m_PlacedHeapDesc, IID_PPV_ARGS(m_PlacedHeap.put())));
        SET_DEBUGNAME(m_PlacedHeap.get(), _T("Heap"));

        m_MaxOrderSize = static_cast<UINT>(m_PlacedHeapDesc.SizeInBytes / m_MinBlockSize);

        m_PlacedResources = map<UINT, IResource*>();
        m_MemoryBlockOrders = set<UINT>();
        m_MemoryBlockOrders.insert(0);
    }

#if 0
    void PlacedHeap::PlacedResource(D3D12_RESOURCE_STATES initialState, IResource& resource, const D3D12_CLEAR_VALUE* pOptimizedClearValue)
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
        auto& allocationInfo = resourceInfos[index];
        ASSERT((allocationInfo.Offset + allocationInfo.SizeInBytes) <= m_PlacedHeapDesc.SizeInBytes, L"WARNING::PlacedResource()指定资源无法放置到堆。");

        CHECK_HRESULT(g_Device->CreatePlacedResource(
            m_PlacedHeap.get(),     // 放置资源的堆
            allocationInfo.Offset,    // 资源的偏移量，必须是资源的对齐的倍数
            &resourceDescs[index],  // 资源描述
            initialState,           // 资源的初始状态
            pOptimizedClearValue,   // 描述用于优化特定资源的清除操作的值
            IID_PPV_ARGS(resource.PutD3D12Resource()))); // 要放置的资源
        SET_DEBUGNAME(resource.GetD3D12Resource(), _T("Resource"));
    }

    void PlacedHeap::PlacedResource(UINT64 offset, D3D12_RESOURCE_STATES initialState, IResource& resource, const D3D12_CLEAR_VALUE* pOptimizedClearValue)
    {
        // TODO 能否用Map来管理放置堆？
    }
#endif

    bool PlacedHeap::PlacedResource(IResource& resource)
    {
        auto& resourceDesc = resource.GetResourceDesc();
        auto* placedDesc = resource.GetPlacedResourceDesc();

        UINT allocateSize = UINT64_UPPER(placedDesc->m_AllocationSize, m_MinBlockSize) / m_MinBlockSize; // 对齐到块大小，计算需要使用块数量

        if (allocateSize > m_MaxOrderSize)
        {
            // 单个资源大小超过了堆大小上限
            ASSERT(0);
            return false;
        }

        // 从第一个位置开始查询
        auto pBlock = m_MemoryBlockOrders.begin();
        while (pBlock != m_MemoryBlockOrders.end())
        {
            // 查找该位置是否已被使用
            auto pFindedRes = m_PlacedResources.find(*pBlock);
            if (pFindedRes == m_PlacedResources.end())
            {
                // 当前 Block 位置未被使用
                set<UINT>::iterator checkFreeBlock = pBlock;
                checkFreeBlock++; // 设定为下一个块必定是使用状态，当资源不使用时，应当移除绑定块的索引

                // 剩余可分配大小
                UINT remain;
                if (checkFreeBlock == m_MemoryBlockOrders.end())
                    remain = m_MaxOrderSize - *pBlock;
                else
                    remain = *checkFreeBlock - *pBlock;

                if (allocateSize <= remain)
                {
                    // 允许分配资源
                    break;
                }
                else
                {
                    // 该内存块超过可分配最大限制
                    pBlock++;
                    continue;
                }
            }
            else
            {
                // 该位置已被使用
                pBlock++;
                continue;
            }
        }

        if (pBlock != m_MemoryBlockOrders.end())
        {
            placedDesc->m_PlacedOrder = *pBlock;
            placedDesc->m_PlacedHeapPtr = this;
            ASSERT(placedDesc->m_AllocationSize == (allocateSize * (UINT64)m_MinBlockSize));

            CHECK_HRESULT(g_Device->CreatePlacedResource(
                m_PlacedHeap.get(), // 放置资源的堆
                (*pBlock * (UINT64)m_MinBlockSize), // 资源的偏移量，必须是资源的对齐的倍数
                &resourceDesc, // 资源描述
                placedDesc->m_InitialState, // 资源的初始状态
                placedDesc->m_OptimizedClearValue, // 描述用于优化特定资源的清除操作的值
                IID_PPV_ARGS(resource.PutD3D12Resource()))); // 要放置的资源
            SET_DEBUGNAME(resource.GetD3D12Resource(), _T("Resource"));

            m_PlacedResources.emplace(*pBlock, &resource); // 保留已放置资源链接
            m_MemoryBlockOrders.insert(*pBlock + allocateSize); // 插入内存块位置
            return true;
        }
        else
        {
            // 该放置堆无可分配内存
            return false;
        }
    }

    void PlacedHeap::ReleaseResource(UINT order)
    {
        // 移除该资源
        auto findedRes = m_PlacedResources.find(order);
        if (findedRes != m_PlacedResources.end())
            m_PlacedResources.erase(order);

        auto bindedOrder = m_MemoryBlockOrders.find(order);
        if (bindedOrder != m_MemoryBlockOrders.end())
        {
            // 检测尾部块是否被使用
            {
                auto nextOrder = bindedOrder;
                nextOrder++;
                if (nextOrder != m_MemoryBlockOrders.end())
                {
                    auto findedNextRes = m_PlacedResources.find(*nextOrder);
                    if (findedNextRes == m_PlacedResources.end())
                    {
                        // 尾部块未被使用，将绑定的块和下一个块合并
                        m_MemoryBlockOrders.erase(nextOrder);
                    }
                }
            }

            // 检测上一个资源是否存在
            if (bindedOrder != m_MemoryBlockOrders.begin())
            {
                auto prevOrder = bindedOrder;
                prevOrder--;
                auto findedPrevRes = m_PlacedResources.find(*prevOrder);
                if (findedPrevRes == m_PlacedResources.end())
                {
                    // 上一个资源已被释放，将绑定的块和上一个块合并
                    m_MemoryBlockOrders.erase(bindedOrder);
                }
            }
        }

    }


    GraphicsMemory GraphicsMemory::g_GraphicsMemory = GraphicsMemory();


    void GraphicsMemory::PlacedResource(IResource& resource)
    {
        // 获取资源分配需要的信息
        auto& resourceDesc = resource.GetResourceDesc();
        auto* placedDesc = resource.GetPlacedResourceDesc();

        ASSERT(placedDesc != nullptr);
        if (placedDesc == nullptr)
            return;

        auto allocationInfo = g_Device->GetResourceAllocationInfo(NODEMASK, 1, &resourceDesc);
        placedDesc->m_AllocationSize = allocationInfo.SizeInBytes;
        placedDesc->m_AllocationAlignment = allocationInfo.Alignment;

        // TODO 扩展 MSAA 的放置堆 D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT
        ASSERT(placedDesc->m_AllocationAlignment == D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
        // TODO 放置资源的大小超过了设计的堆大小
        ASSERT(placedDesc->m_AllocationSize <= DEFAULT_HEAP_SIZE);

        D3D12_HEAP_FLAGS heapFlags{};
#ifdef DONT_USE_SMART_PTR
        vector<PlacedHeap>* heaps{};
#else
        vector<std::unique_ptr<PlacedHeap>>* heaps{};
#endif
        switch (resourceDesc.Dimension)
        {
        case D3D12_RESOURCE_DIMENSION_BUFFER:
        {
            heaps = &(placedDesc->m_HeapType == D3D12_HEAP_TYPE_DEFAULT ? GetInstance().m_DefaultBufferHeaps : GetInstance().m_UploadBufferHeaps);
            heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
        }
        break;
        case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
        case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
        case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
        {
            ASSERT(placedDesc->m_HeapType == D3D12_HEAP_TYPE_DEFAULT);
            heaps = &(GetInstance().m_TextureHeaps);
            heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
        }
        break;
        default: ASSERT(0); break;
        }

        auto pHeap = heaps->begin();
        for (; pHeap != heaps->end(); pHeap++)
        {
            // 尝试将资源放入堆
#ifdef DONT_USE_SMART_PTR
            if ((*pHeap).PlacedResource(resource))
#else
            if ((**pHeap).PlacedResource(resource))
#endif
            {
                break;
            }
        }

        if (pHeap == heaps->end())
        {
            // 没有堆可以放置资源，创建一个新的堆
#ifdef DONT_USE_SMART_PTR
            heaps->push_back(PlacedHeap());
            auto& newHeap = heaps->back();
#else
            heaps->push_back(unique_ptr<PlacedHeap>(new PlacedHeap()));
            auto& newHeap = *(heaps->back());
#endif

            newHeap.Create(placedDesc->m_HeapType, DEFAULT_HEAP_SIZE, heapFlags);
            if (!newHeap.PlacedResource(resource))
            {
                ASSERT(0);
            }
        }
    }
}