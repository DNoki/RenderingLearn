#pragma once

/**
 * @brief 描述符句柄
*/
class DescriptorHandle
{
public:
    /**
     * @brief 空描述符
    */
    const static DescriptorHandle DESCRIPTOR_HANDLE_NULL;

    DescriptorHandle()
    {
        m_CpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
        m_GpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
    }

    DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle) : m_CpuHandle(CpuHandle), m_GpuHandle(GpuHandle) {}

    DescriptorHandle operator+ (INT OffsetScaledByDescriptorSize) const
    {
        DescriptorHandle ret = *this;
        ret += OffsetScaledByDescriptorSize;
        return ret;
    }

    void operator += (INT OffsetScaledByDescriptorSize)
    {
        if (m_CpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
            m_CpuHandle.ptr += OffsetScaledByDescriptorSize;
        if (m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
            m_GpuHandle.ptr += OffsetScaledByDescriptorSize;
    }

    operator D3D12_CPU_DESCRIPTOR_HANDLE () const { return m_CpuHandle; }
    operator D3D12_GPU_DESCRIPTOR_HANDLE () const { return m_GpuHandle; }
    operator const D3D12_CPU_DESCRIPTOR_HANDLE* () const { return &m_CpuHandle; }
    operator const D3D12_GPU_DESCRIPTOR_HANDLE* () const { return &m_GpuHandle;; }

    /**
     * @brief 是否是着色器可见描述符
     * @return
    */
    bool IsShaderVisible() const { return m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }

private:
    D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;    // Cpu句柄（必定CPU可见）
    D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle;    // Gpu句柄
};