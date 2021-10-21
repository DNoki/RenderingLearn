#include "pch.h"

#include "GpuResource.h"


void GpuResource::SetAddressOrDescriptor(D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress, const DescriptorHandle& descriptorHandle)
{
    m_GpuVirtualAddress = gpuVirtualAddress;
    m_DescriptorHandle = descriptorHandle;
}


