#include "pch.h"

#include "GraphicsCore.h"
#include "GpuPlacedHeap.h"
#include "CommandQueue.h"
#include "CommandList.h"

#include "GpuBuffer.h"

using namespace Graphics;


GpuBuffer::GpuBuffer() : m_Type(BufferType::UNCREATED), m_VertexBufferView(nullptr), m_UploadBuffer(nullptr) {}

void GpuBuffer::CreateVertexBuffer(UINT strideSize, UINT vertexCount, const void* vertices)
{
    // 说明：上传缓冲区对 CPU 和 GPU 都是可见的，但由于内存是写合并的，因此应避免使用 CPU 读取数据。 上传缓冲区用于将数据移动到默认 GPU 缓冲区。 您可以将文件直接读入上传缓冲区，而不是读入常规缓存内存，将其复制到上传缓冲区，然后将其复制到 GPU。

    // 注意：不推荐使用上传堆来传输静态数据，如垂直缓冲区。 每次 GPU 需要它时，上传堆都会被编组。 请阅读默认堆用法。 此处使用上传堆是为了代码简单，并且因为要实际传输的顶点很少。
    auto bufferSize = strideSize * vertexCount;
    auto pHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    CHECK_HRESULT(Graphics::g_Device->CreateCommittedResource(
        &pHeapProperties, // 为资源的堆提供属性
        D3D12_HEAP_FLAG_NONE, // 堆选项
        &m_ResourceDesc, // 描述资源
        D3D12_RESOURCE_STATE_GENERIC_READ, // 资源的初始状态
        nullptr, // 清除操作最优化的值
        IID_PPV_ARGS(PutD3D12Resource())));


    // 将三角形数据复制到顶点缓冲区
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0); // 不在 CPU 上读取此资源
    CHECK_HRESULT(m_Resource->Map( // 获取指向资源中指定子资源的 CPU 指针，但不得将指针值透露给应用程序。
        0, // 指定子资源的索引号
        &readRange, // 表示 CPU 可能读取的区域
        reinterpret_cast<void**>(&pVertexDataBegin))); // 指向内存块的指针，该内存块接收指向资源数据的指针。
    memcpy(pVertexDataBegin, vertices, bufferSize);
    m_Resource->Unmap(0, nullptr);

    Finalize();

    // 创建顶点缓冲视图
    m_VertexBufferView = std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW>(new D3D12_VERTEX_BUFFER_VIEW{
            m_GpuVirtualAddress,
            bufferSize,
            strideSize });

    m_Type = BufferType::VERTEX;
}

void GpuBuffer::PlacedVertexBuffer(UINT strideSize, UINT vertexCount, const void* vertices, GpuPlacedHeap& pPlacedHeap, GpuPlacedHeap& pUploadPlacedHeap)
{
    auto bufferSize = strideSize * vertexCount;

    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    pPlacedHeap.PlacedResource(D3D12_RESOURCE_STATE_COPY_DEST, *this);


    m_UploadBuffer = std::unique_ptr<UploadBuffer>(new UploadBuffer());
    //m_UploadBuffer->Create(bufferSize);
    m_UploadBuffer->SetResourceDesc(CD3DX12_RESOURCE_DESC::Buffer(bufferSize));
    pUploadPlacedHeap.PlacedResource(D3D12_RESOURCE_STATE_GENERIC_READ, *m_UploadBuffer);
    m_UploadBuffer->Finalize();

    D3D12_SUBRESOURCE_DATA srcData = {};
    srcData.pData = vertices;
    srcData.RowPitch = bufferSize;
    srcData.SlicePitch = bufferSize;
    UpdateSubresources(
        g_GraphicCommandList.GetD3D12CommandList(),
        m_Resource.get(),
        m_UploadBuffer->GetD3D12Resource(),
        0, 0, 1,
        &srcData);

    auto barriers = CD3DX12_RESOURCE_BARRIER::Transition(
        m_Resource.get(),
        D3D12_RESOURCE_STATE_COPY_DEST,                 // 之前的状态
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);    // 之后的状态
    g_GraphicCommandList->ResourceBarrier(1, &barriers);


    Finalize();

    // 创建顶点缓冲视图
    m_VertexBufferView = std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW>(new D3D12_VERTEX_BUFFER_VIEW{
            m_GpuVirtualAddress,
            bufferSize,
            strideSize });
}
