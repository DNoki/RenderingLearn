#include "pch.h"

#include "GraphicsCore.h"

#include "DescriptorHeap.h"

using namespace winrt;
using namespace Graphics;


// --------------------------------------------------------------------------
/*
    描述符堆
    https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/descriptor-heaps-overview

    描述符
    https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/descriptors-overview


    描述符堆包含不属于管道状态对象（PSO）的许多对象类型。包括：
        着色器可见描述符：
            ・着色器资源视图（SRV）
            ・无序访问视图（UAV）
            ・常量缓冲区视图（CBV）
            ・取样器（Sampler）
        非着色器可见描述符：
            ・呈现目标视图（RTV）
            ・深度模板视图（DSV）

    SRV、UAV、CBV 可以保存在同一个描述符堆中

*/
// --------------------------------------------------------------------------



DescriptorHeap::DescriptorHeap() : m_DescriptorHeap(nullptr), m_DescriptorHeapDesc(), m_DescriptorSize(), m_StartDescriptorHandle()
{
}

void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count)
{
    ASSERT(m_DescriptorHeap == nullptr);
    m_DescriptorHeapDesc = {};

    // 是否为着色器可见（SRV、UAV、CBV、Sampler）
    auto isShaderVisible = (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    // 描述符堆描述
    m_DescriptorHeapDesc.NumDescriptors = count;
    m_DescriptorHeapDesc.Type = type;
    m_DescriptorHeapDesc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    m_DescriptorHeapDesc.NodeMask = 1;


    // 创建描述符堆
    CHECK_HRESULT(g_Device->CreateDescriptorHeap(&m_DescriptorHeapDesc, IID_PPV_ARGS(m_DescriptorHeap.put())));

    // 单个描述符大小
    m_DescriptorSize = g_Device->GetDescriptorHandleIncrementSize(type);

    // 描述符起始句柄
    m_StartDescriptorHandle = {
        m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        isShaderVisible ? m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN }
    };
}

DescriptorHandle DescriptorHeap::GetDescriptorHandle(UINT index)
{
    ASSERT(0 <= index || index < GetDescriptorsCount());
    return m_StartDescriptorHandle + index * m_DescriptorSize;
}
