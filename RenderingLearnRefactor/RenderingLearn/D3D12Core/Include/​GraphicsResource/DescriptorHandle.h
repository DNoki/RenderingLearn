#pragma once

namespace D3D12Core
{
    typedef D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress;
    typedef D3D12_GPU_DESCRIPTOR_HANDLE GpuDescriptorHandle;
    typedef D3D12_CPU_DESCRIPTOR_HANDLE CpuDescriptorHandle;

    // 空描述符地址
    constexpr GpuVirtualAddress D3D12_GPU_VIRTUAL_ADDRESS_NULL = 0;
    // 未知描述符地址
    constexpr GpuVirtualAddress D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN = -1;

    /**
     * @brief 描述符句柄
    */
    class DescriptorHandle
    {
    public:
        /**
         * @brief 空描述符
        */
        static const DescriptorHandle DESCRIPTOR_HANDLE_NULL;

        DescriptorHandle() = default;
        DescriptorHandle(CpuDescriptorHandle CpuHandle, GpuDescriptorHandle GpuHandle) : m_CpuHandle(CpuHandle), m_GpuHandle(GpuHandle) {}

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

        operator CpuDescriptorHandle () const { return m_CpuHandle; }
        operator GpuDescriptorHandle () const { return m_GpuHandle; }
        operator const CpuDescriptorHandle* () const { return &m_CpuHandle; }
        operator const GpuDescriptorHandle* () const { return &m_GpuHandle; }

        /**
         * @brief 是否是着色器可见描述符
         * @return
        */
        bool IsShaderVisible() const { return m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }

    private:
        CpuDescriptorHandle m_CpuHandle{ D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN };    // Cpu句柄（必定CPU可见）
        GpuDescriptorHandle m_GpuHandle{ D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN };    // Gpu句柄
    };
}