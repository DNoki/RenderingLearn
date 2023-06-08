#include "pch.h"
#include "​GraphicsResource/DescriptorAllocator.h"

#include "​GraphicsResource/DescriptorHeap.h"

using namespace D3D12Core;

#define FIXED_DESCRIPTOR_HEAP_SIZE 256

struct AllocatorHeap
{
public:
    UniquePtr<DescriptorHeap> m_DescriptorHeap{};
    UINT m_NextHandle{};

};

class DescriptorAllocatorImpl
{
public:
    DescriptorAllocatorImpl() = default;

    DescriptorHandle Allocat(const GraphicsContext& context, D3D12_DESCRIPTOR_HEAP_TYPE type)
    {
        if (m_CurrentHeap[type] == nullptr || (m_CurrentHeap[type]->m_NextHandle >= m_CurrentHeap[type]->m_DescriptorHeap->GetDescriptorsCount()))
        {
            m_Heaps[type].push_back(AllocatorHeap());
            m_CurrentHeap[type] = &m_Heaps[type].back();
            m_CurrentHeap[type]->m_DescriptorHeap.reset(new DescriptorHeap());
            m_CurrentHeap[type]->m_DescriptorHeap->Create(context, type, FIXED_DESCRIPTOR_HEAP_SIZE, false);

            static const String heapNames[] =
            {
                TEXT("CBV_SRV_UAV"),
                TEXT("Sampler"),
                TEXT("RTV"),
                TEXT("DSV"),
            };
            GraphicsContext::SetDebugName(m_CurrentHeap[type]->m_DescriptorHeap->GetD3D12DescriptorHeap(), heapNames[type]);
        }
        return m_CurrentHeap[type]->m_DescriptorHeap->GetDescriptorHandle(m_CurrentHeap[type]->m_NextHandle++);
    }

private:
    Vector<AllocatorHeap> m_Heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{};

    AllocatorHeap* m_CurrentHeap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{};

};

Map<const GraphicsContext*, DescriptorAllocatorImpl> g_DescriptorAllocatorImpl;

DescriptorHandle DescriptorAllocator::Allocat(const GraphicsContext& context, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    return g_DescriptorAllocatorImpl[&context].Allocat(context, type);
}

