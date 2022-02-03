#pragma once

#include "GraphicsResource.h"

class UploadBuffer : public IPlacedObject
{
public:
    //static UploadBuffer* Request(UINT64 size);
    inline ID3D12Resource1* GetD3D12Resource() const { return m_Resource.get(); }
    inline ID3D12Resource1** PutD3D12Resource() override { return m_Resource.put(); }

    UploadBuffer();

    /**
     * @brief 创建一个上传堆并为其分配内存
     * @param size
    */
    void Create(UINT64 size);
    void Placed(UINT64 size);

    inline HRESULT Map(UINT Subresource, const D3D12_RANGE* pReadRange, void** ppData) { return m_Resource->Map(Subresource, pReadRange, ppData); }
    inline void UnMap(UINT Subresource, const D3D12_RANGE* pWrittenRange) { m_Resource->Unmap(Subresource, pWrittenRange); }

    inline const D3D12_RESOURCE_DESC& GetResourceDesc() override { return m_ResourceDesc; }
    inline void SetResourceDesc(const D3D12_RESOURCE_DESC& desc) { m_ResourceDesc = desc; }
    inline void SetResourceDesc(const D3D12_RESOURCE_DESC&& desc) { m_ResourceDesc = desc; }

    inline UINT64 GetBufferSize() { return m_ResourceDesc.Width; }

    void Finalize();

private:
    // 资源对象
    winrt::com_ptr<ID3D12Resource1> m_Resource;
    // 资源描述
    D3D12_RESOURCE_DESC m_ResourceDesc;

    // GPU 内存中的虚拟地址
    // IBV、VBV 等直接调用资源类型时使用
    D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;

};
