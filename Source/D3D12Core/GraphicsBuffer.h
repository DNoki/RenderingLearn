#pragma once

#include "UploadBuffer.h"

class GpuPlacedHeap;

/**
 * @brief GPU 缓冲
*/
class GraphicsBuffer : public IPlacedObject
{
public:
    GraphicsBuffer();

    void DirectCreate(UINT64 size);
    void PlacedCreate(UINT64 size, GpuPlacedHeap& pPlacedHeap);

    void CopyVertexBuffer(UINT strideSize, UINT vertexCount, const void* vertices);

#if 0
    /**
     * @brief 创建顶点缓冲视图（VBV）
     * @param strideSize 顶点数据结构大小
     * @param vertexCount 顶点数量
     * @param vertices 顶点列表
    */
    void CreateVertexBuffer(UINT strideSize, UINT vertexCount, const void* vertices);
    void PlacedVertexBuffer(UINT strideSize, UINT vertexCount, const void* vertices, GpuPlacedHeap& pPlacedHeap, GpuPlacedHeap& pUploadPlacedHeap);
#endif

    inline ID3D12Resource1** PutD3D12Resource() override { return m_Resource.put(); }
    inline const D3D12_RESOURCE_DESC& GetResourceDesc() override { return m_ResourceDesc; }

    /**
     * @brief 视为顶点缓冲视图
    */
    inline operator const D3D12_VERTEX_BUFFER_VIEW* ()
    {
        ASSERT(m_VertexBufferView != nullptr);
        return m_VertexBufferView.get();
    }

private:
    // 资源对象
    winrt::com_ptr<ID3D12Resource1> m_Resource;
    // 资源描述
    D3D12_RESOURCE_DESC m_ResourceDesc;

    // GPU 内存中的虚拟地址
    // IBV、VBV 等直接调用资源类型时使用
    D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;

    std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW> m_VertexBufferView;


    std::unique_ptr<UploadBuffer> m_UploadBuffer;

    void Finalize();
};
