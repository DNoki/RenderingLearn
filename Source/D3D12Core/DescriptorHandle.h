﻿#pragma once

// 空描述符地址
constexpr SIZE_T D3D12_GPU_VIRTUAL_ADDRESS_NULL = ((D3D12_GPU_VIRTUAL_ADDRESS)0);
// 未知描述符地址
constexpr SIZE_T D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN = ((D3D12_GPU_VIRTUAL_ADDRESS)-1);


/**
 * @brief 描述符句柄
*/
struct DescriptorHandle
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

    operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return m_CpuHandle; }
    operator D3D12_GPU_DESCRIPTOR_HANDLE() const { return m_GpuHandle; }

    inline const D3D12_CPU_DESCRIPTOR_HANDLE* GetCpuHandle() const { return &m_CpuHandle; }
    inline const D3D12_GPU_DESCRIPTOR_HANDLE* GetGpuHandle() const { return &m_GpuHandle; }

    /**
     * @brief 是否是着色器可见描述符
     * @return
    */
    bool IsShaderVisible() const { return m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }

private:
    D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;    // Cpu句柄（必定CPU可见）
    D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle;    // Gpu句柄
};