#include "pch.h"
#include "​GraphicsResource/UploadBuffer.h"

// --------------------------------------------------------------------------
/*
    上传堆缓冲
        对于GPU来说为只读，对于CPU来说可读可写（通常为只写）
*/
// --------------------------------------------------------------------------


using namespace D3D12Core;


void UploadBuffer::DirectCreate(uint64 size)
{
    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
    m_ResourceStates = D3D12_RESOURCE_STATE_GENERIC_READ; // 上传堆的初始状态必须此项，且不能更改

    auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CHECK_HRESULT(GraphicsContext::GetCurrentInstance()->GetDevice()->CreateCommittedResource(
        &heapType,
        D3D12_HEAP_FLAG_NONE,
        &m_ResourceDesc,
        m_ResourceStates,
        nullptr,
        IID_PPV_ARGS(PutD3D12Resource())));// 注意：上传资源的生命周期必须等待GPU复制完成之后才能释放

    Finalize();
}

void UploadBuffer::PlacedCreate(uint64 size)
{
    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
    m_ResourceStates = D3D12_RESOURCE_STATE_GENERIC_READ;

    m_PlacedResourceDesc.m_HeapType = D3D12_HEAP_TYPE_UPLOAD;
    m_PlacedResourceDesc.m_OptimizedClearValue = nullptr;
    GraphicsMemory::PlacedResource(*this);

    Finalize();
}

#if 0
void UploadBuffer::CopyVertexBuffer(uint32 strideSize, const void* vertices)
{
    ASSERT(m_Resource != nullptr);
    auto bufferSize = m_ResourceDesc.Width;

    uint8* pVertexDataBegin = nullptr;
    Map(0, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, vertices, bufferSize);
    Unmap(0);

    // 创建顶点缓冲视图
    m_VertexBufferView.reset(new D3D12_VERTEX_BUFFER_VIEW{
            m_GpuVirtualAddress,
            static_cast<uint32>(bufferSize),
            strideSize });

    TRACE(L"WARNING::正在使用上传堆顶点缓冲。");
}
#endif
