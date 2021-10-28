#pragma once

#include "DescriptorHandle.h"

class GpuResource
{
public:
    inline ID3D12Resource1* GetD3D12Resource() const { return m_Resource.get(); }
    inline ID3D12Resource1** PutD3D12Resource() { return m_Resource.put(); }

    inline const D3D12_RESOURCE_DESC& GetResourceDesc() { return m_ResourceDesc; }
    inline void SetResourceDesc(const D3D12_RESOURCE_DESC& desc) { m_ResourceDesc = desc; }
    inline void SetResourceDesc(const D3D12_RESOURCE_DESC&& desc) { m_ResourceDesc = desc; }

    inline D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return m_GpuVirtualAddress; }
    inline const DescriptorHandle* GetDescriptorHandle() const { return &m_DescriptorHandle; }

protected:
    // 资源对象
    // 封装了 CPU 和 GPU 读取和写入物理内存或堆的通用能力。
    winrt::com_ptr<ID3D12Resource1> m_Resource;
    D3D12_RESOURCE_DESC m_ResourceDesc;

    // GPU 内存中的虚拟地址
    // IBV、VBV 等直接调用资源类型时使用
    D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;

    // 描述符句柄
    // 由描述符引用的资源使用
    DescriptorHandle m_DescriptorHandle;


    GpuResource() :m_Resource(nullptr), m_GpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL), m_DescriptorHandle(), m_ResourceDesc() {}

    void SetAddressOrDescriptor(D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL, const DescriptorHandle& descriptorHandle = DescriptorHandle::DESCRIPTOR_HANDLE_NULL);
};

