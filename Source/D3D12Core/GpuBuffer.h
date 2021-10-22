#pragma once

#include "GpuResource.h"

enum class BufferType
{
    UNCREATED,
    VERTEX,
    //INDEX,
};

class GpuBuffer : public GpuResource
{
public:
    GpuBuffer() : m_Type(BufferType::UNCREATED), m_VertexBufferView(nullptr) {}

    /**
     * @brief 创建顶点缓冲视图（VBV）
     * @param strideSize 顶点数据结构大小
     * @param vertexCount 顶点数量
     * @param vertices 顶点列表
    */
    void CreateVertexBuffer(UINT strideSize, UINT vertexCount, const void* vertices);

    /**
     * @brief 获取 D3D12 顶点缓冲视图
     * @return 
    */
    inline D3D12_VERTEX_BUFFER_VIEW* GetD3D12VBV() const { return m_VertexBufferView.get(); }

private:
    BufferType m_Type; // 缓冲类型
    std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW> m_VertexBufferView;
};
