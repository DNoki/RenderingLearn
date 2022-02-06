#pragma once

class GpuPlacedHeap;

/**
 * @brief D3D12 资源接口
 * @return
*/
class IResource
{
public:
    // --------------------------------------------------------------------------
    inline IResource() : m_Resource(), m_ResourceDesc(), m_GpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL) {}
    virtual ~IResource() = 0 {}

    // --------------------------------------------------------------------------
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

    // GPU 内存中的虚拟地址
    // IBV、VBV 等直接调用资源类型时使用
    D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;
};

class IBufferResource : public IResource
{
public:
    // --------------------------------------------------------------------------
    IBufferResource() {}
    virtual ~IBufferResource() = 0 {}

    // --------------------------------------------------------------------------
    /**
     * @brief 获取缓冲大小
     * @return
    */
    inline UINT64 GetBufferSize() const { return m_ResourceDesc.Width; }

    inline D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return m_GpuVirtualAddress; }

    /**
     * @brief 创建一个资源并为其分配内存
     * @param size
    */
    virtual void DirectCreate(UINT64 size) = 0;
    /**
     * @brief 使用定位方式创建一个资源
     * @param size
     * @param pPlacedHeap
    */
    virtual void PlacedCreate(UINT64 size, GpuPlacedHeap& pPlacedHeap) = 0;

protected:

    void Finalize()
    {
        // Resource必须创建以后才可以完成初始化
        ASSERT(m_Resource != nullptr);
        // 仅缓冲资源可以获取 GPU 虚拟地址
        m_GpuVirtualAddress = m_Resource->GetGPUVirtualAddress();
    }
};

class ITexture : public IResource
{
public:
    ITexture() {}
    virtual ~ITexture() = 0 {}

private:

};
