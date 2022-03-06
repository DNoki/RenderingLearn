#pragma once

namespace Graphics
{
    // 空描述符地址
    constexpr D3D12_GPU_VIRTUAL_ADDRESS D3D12_GPU_VIRTUAL_ADDRESS_NULL = 0;
    // 未知描述符地址
    constexpr D3D12_GPU_VIRTUAL_ADDRESS D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN = -1;

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
        DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle) : m_CpuHandle(CpuHandle), m_GpuHandle(GpuHandle) {}

        inline DescriptorHandle operator+ (INT OffsetScaledByDescriptorSize) const
        {
            DescriptorHandle ret = *this;
            ret += OffsetScaledByDescriptorSize;
            return ret;
        }

        inline void operator += (INT OffsetScaledByDescriptorSize)
        {
            if (m_CpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
                m_CpuHandle.ptr += OffsetScaledByDescriptorSize;
            if (m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
                m_GpuHandle.ptr += OffsetScaledByDescriptorSize;
        }

        inline operator D3D12_CPU_DESCRIPTOR_HANDLE () const { return m_CpuHandle; }
        inline operator D3D12_GPU_DESCRIPTOR_HANDLE () const { return m_GpuHandle; }
        inline operator const D3D12_CPU_DESCRIPTOR_HANDLE* () const { return &m_CpuHandle; }
        inline operator const D3D12_GPU_DESCRIPTOR_HANDLE* () const { return &m_GpuHandle; }

        /**
         * @brief 是否是着色器可见描述符
         * @return
        */
        inline bool IsShaderVisible() const { return m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }

    private:
        D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle{ D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN };    // Cpu句柄（必定CPU可见）
        D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle{ D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN };    // Gpu句柄
    };
}