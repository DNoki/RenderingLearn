#pragma once

#include "GraphicsResource.h"

namespace Graphics
{
    /**
     * @brief 上传堆缓冲
    */
    class UploadBuffer : public GraphicsResource, public IBufferResource
    {
    public:
        // --------------------------------------------------------------------------
        UploadBuffer();

        // --------------------------------------------------------------------------
        virtual inline UINT64  GetBufferSize() const override { return m_ResourceDesc.Width; }
        virtual inline D3D12_GPU_VIRTUAL_ADDRESS  GetGpuVirtualAddress() const override { return m_GpuVirtualAddress; }

        // --------------------------------------------------------------------------
        /**
         * @brief 创建一个上传堆并为其分配内存
         * @param size
        */
        virtual void DirectCreate(UINT64 size) override;
        /**
         * @brief 使用定位方式创建一个上传堆
         * @param size
         * @param pPlacedHeap
        */
        virtual void PlacedCreate(UINT64 size) override;


        /**
         * @brief 获取资源中指定子资源的 CPU 指针
         * @param Subresource 子资源的索引
         * @param ppData 一个指向内存块的指针，它接收指向资源数据的指针
         * @return
        */
        inline HRESULT Map(UINT Subresource, void** ppData) const
        {
            // 简单模型不允许 CPU 读取上传堆内数据
            return m_Resource->Map(Subresource, &c_ZeroReadRange, ppData);
        }
        /**
         * @brief 使指向资源中指定子资源的 CPU 指针无效
         * @param Subresource 子资源的索引
        */
        inline void Unmap(UINT Subresource) const { m_Resource->Unmap(Subresource, &c_ZeroReadRange); }


    private:
        // Map 时指示 CPU 不可读取上传堆资源
        static const CD3DX12_RANGE c_ZeroReadRange;

        virtual inline void Finalize() override
        {
            // Resource必须创建以后才可以完成初始化
            ASSERT(m_Resource != nullptr);
            // 仅缓冲资源可以获取 GPU 虚拟地址
            m_GpuVirtualAddress = m_Resource->GetGPUVirtualAddress();
        }

    };
}
