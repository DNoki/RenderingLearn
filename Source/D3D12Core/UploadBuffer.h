#pragma once

#include "IResource.h"

namespace Graphics
{
    class GpuPlacedHeap;

    /**
     * @brief 上传堆缓冲
    */
    class UploadBuffer : public IBufferResource
    {
    public:
        // --------------------------------------------------------------------------
        UploadBuffer();

        // --------------------------------------------------------------------------
        /**
         * @brief 获取缓冲大小
         * @return
        */
        inline UINT64 GetBufferSize() const { return m_ResourceDesc.Width; }

        // --------------------------------------------------------------------------
        /**
         * @brief 创建一个上传堆并为其分配内存
         * @param size
        */
        void DirectCreate(UINT64 size) override;
        /**
         * @brief 使用定位方式创建一个上传堆
         * @param size
         * @param pPlacedHeap
        */
        void PlacedCreate(UINT64 size, GpuPlacedHeap& pPlacedHeap) override;

        /**
         * @brief 向上传堆拷贝顶点缓冲（立即）
         * @param strideSize 单个顶点数据结构大小
         * @param vertexCount
         * @param vertices
        */
        void CopyVertexBuffer(UINT strideSize, const void* vertices);

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


        // --------------------------------------------------------------------------
        /**
         * @brief 视为顶点缓冲视图
        */
        inline operator const D3D12_VERTEX_BUFFER_VIEW* ()
        {
            ASSERT(m_VertexBufferView != nullptr);
            return m_VertexBufferView.get();
        }

    private:
        // Map 时指示 CPU 不可读取上传堆资源
        static const CD3DX12_RANGE c_ZeroReadRange;

        std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW> m_VertexBufferView;
    };
}
