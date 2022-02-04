#pragma once

#include "IResource.h"

class UploadBuffer;
class GpuPlacedHeap;
class CommandList;

/**
 * @brief GPU 缓冲
*/
class GraphicsBuffer : public IResource
{
public:
    GraphicsBuffer();

    /**
     * @brief 创建指定大小的默认堆
     * @param size 
    */
    void DirectCreate(UINT64 size);
    /**
     * @brief 使用定位方式创建一个默认堆
     * @param size 
     * @param pPlacedHeap 
    */
    void PlacedCreate(UINT64 size, GpuPlacedHeap& pPlacedHeap);

    /**
     * @brief 添加拷贝顶点缓冲命令
     * @param commandList 
     * @param strideSize 
     * @param vertices 
    */
    void DispatchCopyVertexBuffer(const CommandList& commandList, UINT strideSize, const void* vertices);


    /**
     * @brief 视为顶点缓冲视图
    */
    inline operator const D3D12_VERTEX_BUFFER_VIEW* ()
    {
        ASSERT(m_VertexBufferView != nullptr);
        return m_VertexBufferView.get();
    }

private:
    // GPU 内存中的虚拟地址
    // IBV、VBV 等直接调用资源类型时使用
    D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;

    std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW> m_VertexBufferView;


    std::unique_ptr<UploadBuffer> m_UploadBuffer;

    void Finalize();
};
