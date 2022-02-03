#pragma once

#include "DescriptorHandle.h"

/**
 * @brief GPU资源
*/
class GraphicsResource : public IPlacedObject
{
public:
    inline ID3D12Resource1* GetD3D12Resource() const { return m_Resource.get(); }
    inline ID3D12Resource1** PutD3D12Resource() override { return m_Resource.put(); }

    inline const D3D12_RESOURCE_DESC& GetResourceDesc() override { return m_ResourceDesc; }
    inline void SetResourceDesc(const D3D12_RESOURCE_DESC& desc) { m_ResourceDesc = desc; }
    inline void SetResourceDesc(const D3D12_RESOURCE_DESC&& desc) { m_ResourceDesc = desc; }

    /**
     * @brief 获取描述符句柄
     * @return 
    */
    inline const DescriptorHandle* GetDescriptorHandle() const { return &m_DescriptorHandle; }

    void Finalize(const DescriptorHandle* descriptorHandle);

protected:
    // 资源对象
    // 封装了 CPU 和 GPU 读取和写入物理内存或堆的通用能力。
    winrt::com_ptr<ID3D12Resource1> m_Resource;
    // 资源描述
    D3D12_RESOURCE_DESC m_ResourceDesc;


    // 描述符句柄
    // 由描述符引用的资源使用
    DescriptorHandle m_DescriptorHandle;


    GraphicsResource() :m_Resource(nullptr), m_DescriptorHandle(), m_ResourceDesc() {}
};

