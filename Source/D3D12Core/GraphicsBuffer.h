#pragma once

#include "IResource.h"

class UploadBuffer;
class GpuPlacedHeap;
class CommandList;

/**
 * @brief GPU 缓冲
*/
class GraphicsBuffer : public IBufferResource
{
public:
    // --------------------------------------------------------------------------
    GraphicsBuffer();

    // --------------------------------------------------------------------------
    /**
     * @brief 创建指定大小的默认堆
     * @param size 
    */
    void DirectCreate(UINT64 size) override;
    /**
     * @brief 使用定位方式创建一个默认堆
     * @param size 
     * @param pPlacedHeap 
    */
    void PlacedCreate(UINT64 size, GpuPlacedHeap& pPlacedHeap) override;

    void DispatchCopyBuffer(const CommandList& commandList, const void* data);
    /**
     * @brief 添加拷贝顶点缓冲命令
     * @param commandList 拷贝命令队列
     * @param strideSize 单个顶点数据结构大小
     * @param vertices 
    */
    void DispatchCopyVertexBuffer(const CommandList& commandList, UINT strideSize, const void* vertices);


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
    std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW> m_VertexBufferView;

    std::unique_ptr<UploadBuffer> m_UploadBuffer;
};
