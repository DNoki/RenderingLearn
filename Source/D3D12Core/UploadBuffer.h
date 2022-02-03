#pragma once

#include "GraphicsResource.h"

class UploadBuffer : public IPlacedObject
{
public:
    //static UploadBuffer* Request(UINT64 size);
    inline ID3D12Resource1* GetD3D12Resource() const { return m_Resource.get(); }
    inline ID3D12Resource1** PutD3D12Resource() override { return m_Resource.put(); }

    UploadBuffer();

    void Create(UINT64 size);

    inline const D3D12_RESOURCE_DESC& GetResourceDesc() override { return m_ResourceDesc; }
    inline void SetResourceDesc(const D3D12_RESOURCE_DESC& desc) { m_ResourceDesc = desc; }
    inline void SetResourceDesc(const D3D12_RESOURCE_DESC&& desc) { m_ResourceDesc = desc; }

    void Finalize();

private:
    //UINT64 m_Size;

    // 资源对象
    winrt::com_ptr<ID3D12Resource1> m_Resource;
    // 资源描述
    D3D12_RESOURCE_DESC m_ResourceDesc;

    // GPU 内存中的虚拟地址
    // IBV、VBV 等直接调用资源类型时使用
    D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;

};
