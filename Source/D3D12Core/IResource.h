#pragma once

/**
 * @brief D3D12 资源接口
 * @return 
*/
class IResource
{
public:
    inline IResource() : m_Resource(), m_ResourceDesc() {}
    virtual ~IResource() = 0 {}

    inline const D3D12_RESOURCE_DESC& GetResourceDesc() const noexcept
    {
        return m_ResourceDesc;
    }
    inline ID3D12Resource1* GetD3D12Resource() const noexcept
    {
        return m_Resource.get();
    }

    inline void SetResourceDesc(const D3D12_RESOURCE_DESC& desc) noexcept
    {
        m_ResourceDesc = desc;
    }
    inline void SetResourceDesc(const D3D12_RESOURCE_DESC&& desc) noexcept
    {
        m_ResourceDesc = desc;
    }
    inline ID3D12Resource1** PutD3D12Resource() noexcept
    {
        return m_Resource.put();
    }

protected:
    // 资源对象
    // 封装了 CPU 和 GPU 读取和写入物理内存或堆的通用能力。
    winrt::com_ptr<ID3D12Resource1> m_Resource;
    // 资源描述
    D3D12_RESOURCE_DESC m_ResourceDesc;
};