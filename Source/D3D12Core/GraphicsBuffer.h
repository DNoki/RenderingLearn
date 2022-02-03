#pragma once

//#include "GraphicsResource.h"
#include "UploadBuffer.h"

class GpuPlacedHeap;

enum class BufferType
{
    UNCREATED,
    VERTEX,
    //INDEX,
};

class GraphicsBuffer : public IPlacedObject
{
public:
    GraphicsBuffer();

    /**
     * @brief 创建顶点缓冲视图（VBV）
     * @param strideSize 顶点数据结构大小
     * @param vertexCount 顶点数量
     * @param vertices 顶点列表
    */
    void CreateVertexBuffer(UINT strideSize, UINT vertexCount, const void* vertices);
    void PlacedVertexBuffer(UINT strideSize, UINT vertexCount, const void* vertices, GpuPlacedHeap& pPlacedHeap, GpuPlacedHeap& pUploadPlacedHeap);

    inline ID3D12Resource1** PutD3D12Resource() override { return m_Resource.put(); }
    inline const D3D12_RESOURCE_DESC& GetResourceDesc() override { return m_ResourceDesc; }
    /**
     * @brief 获取 D3D12 顶点缓冲视图
     * @return
    */
    inline D3D12_VERTEX_BUFFER_VIEW* GetD3D12VBV() const { return m_VertexBufferView.get(); }

private:
    // 资源对象
    winrt::com_ptr<ID3D12Resource1> m_Resource;
    // 资源描述
    D3D12_RESOURCE_DESC m_ResourceDesc;

    // GPU 内存中的虚拟地址
    // IBV、VBV 等直接调用资源类型时使用
    D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;

    BufferType m_Type; // 缓冲类型
    std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW> m_VertexBufferView;


    std::unique_ptr<UploadBuffer> m_UploadBuffer;

    void Finalize();
};
