#include "pch.h"
#include "​GraphicsResource/GraphicsBuffer.h"

//#include "DescriptorHandle.h"
//#include "UploadBuffer.h"
//#include "GraphicsManager.h"
//#include "GraphicsMemory.h"
//#include "CommandQueue.h"
//#include "CommandList.h"
//
//#include "GraphicsBuffer.h"


// --------------------------------------------------------------------------
/*
    GPU 缓冲是存储于 GPU （默认堆）上的缓冲，提供了以下资源的创建
        ・常量缓冲视图（CBV）：常量缓冲
        ・顶点缓冲视图（VBV）：顶点数据
        ・索引缓冲视图（IBV）：索引数据

    使用 GPU 内存虚拟地址来绑定资源
*/
// --------------------------------------------------------------------------


using namespace D3D12Core;

void GraphicsBuffer::DirectCreate(UINT64 size)
{
    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
    m_ResourceStates = D3D12_RESOURCE_STATE_COPY_DEST; // 初始状态为拷贝目标

    auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CHECK_HRESULT(GraphicsContext::GetCurrentInstance()->GetDevice()->CreateCommittedResource(
        &heapType,
        D3D12_HEAP_FLAG_NONE,
        &m_ResourceDesc,
        m_ResourceStates,
        nullptr,
        IID_PPV_ARGS(PutD3D12Resource())
    ));

    Finalize();
}

void GraphicsBuffer::PlacedCreate(UINT64 size)
{
    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
    m_ResourceStates = D3D12_RESOURCE_STATE_COPY_DEST; // 初始状态为拷贝目标

    m_PlacedResourceDesc.m_HeapType = D3D12_HEAP_TYPE_DEFAULT;
    m_PlacedResourceDesc.m_OptimizedClearValue = nullptr;
    GraphicsMemory::PlacedResource(*this);

    Finalize();
}

void GraphicsBuffer::DispatchCopyBuffer(const GraphicsCommandList& commandList, const void* data)
{
    ASSERT(m_Resource != nullptr);
    auto bufferSize = m_ResourceDesc.Width;

    // 创建上传缓冲
    m_UploadBuffer.reset(new UploadBuffer());
    m_UploadBuffer->PlacedCreate(bufferSize);

    // 添加拷贝命令到命令队列
    {
        // 使用 UpdateSubresources 拷贝资源
        D3D12_SUBRESOURCE_DATA srcData = {};
        srcData.pData = data;
        srcData.RowPitch = bufferSize;
        srcData.SlicePitch = bufferSize;
        UpdateSubresources(
            commandList.GetD3D12CommandList(),
            m_Resource.get(),
            m_UploadBuffer->GetD3D12Resource(),
            0, 0, 1,
            &srcData);

#if 0
        // 等待拷贝完成
        auto barriers = CD3DX12_RESOURCE_BARRIER::Transition(
            m_Resource.get(),
            D3D12_RESOURCE_STATE_COPY_DEST,                 // 之前的状态
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);    // 之后的状态 TODO
        commandList.ResourceBarrier(1, &barriers);
#endif
    }

    // 拷贝完成后释放上传堆
    commandList.GetCommandAllocator()->AssignOnCompletedCallback([this]()
        {
            m_UploadBuffer.reset();
        });
}

//void GraphicsBuffer::DispatchTransitionStates(const GraphicsCommandList* commandList, D3D12_RESOURCE_STATES after)
//{
//    ASSERT(m_ResourceStates != after);
//    commandList->ResourceTransitionBarrier(this, m_ResourceStates, after);
//    m_ResourceStates = after; // TODO 资源状态并非是立即更新的
//}

#if 0
void GraphicsBuffer::GenerateVertexOrIndexView(UINT strideSize)
{
    // 创建顶点缓冲视图
    m_VertexBufferView.reset(new D3D12_VERTEX_BUFFER_VIEW{
            m_GpuVirtualAddress,
            static_cast<UINT>(m_ResourceDesc.Width),
            strideSize });

    DXGI_FORMAT ibvFormat = DXGI_FORMAT_R16_UINT;
    switch (strideSize)
    {
        case sizeof(UINT16) : ibvFormat = DXGI_FORMAT_R16_UINT; break;
        default: break;
    }
    m_IndexBufferView.reset(new D3D12_INDEX_BUFFER_VIEW{
            m_GpuVirtualAddress,
            static_cast<UINT>(m_ResourceDesc.Width),
            ibvFormat });
}

void GraphicsBuffer::CreateVertexBuffer(UINT strideSize, UINT vertexCount, const void* vertices)
{
    // 说明：上传缓冲区对 CPU 和 GPU 都是可见的，但由于内存是写合并的，因此应避免使用 CPU 读取数据。 上传缓冲区用于将数据移动到默认 GPU 缓冲区。 您可以将文件直接读入上传缓冲区，而不是读入常规缓存内存，将其复制到上传缓冲区，然后将其复制到 GPU。

    // 注意：不推荐使用上传堆来传输静态数据，如垂直缓冲区。 每次 GPU 需要它时，上传堆都会被编组。 请阅读默认堆用法。 此处使用上传堆是为了代码简单，并且因为要实际传输的顶点很少。
    auto bufferSize = strideSize * vertexCount;
    auto pHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    CHECK_HRESULT(Graphics::GraphicsManager::GetDevice()->CreateCommittedResource(
        &pHeapProperties, // 为资源的堆提供属性
        D3D12_HEAP_FLAG_NONE, // 堆选项
        &m_ResourceDesc, // 描述资源
        D3D12_RESOURCE_STATE_GENERIC_READ, // 资源的初始状态
        nullptr, // 清除操作最优化的值
        IID_PPV_ARGS(m_Resource.put())));


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
}

void GraphicsBuffer::PlacedVertexBuffer(UINT strideSize, UINT vertexCount, const void* vertices, PlacedHeap& pPlacedHeap, PlacedHeap& pUploadPlacedHeap)
{
    auto bufferSize = strideSize * vertexCount;

    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    pPlacedHeap.PlacedResource(D3D12_RESOURCE_STATE_COPY_DEST, *this);


    m_UploadBuffer = std::unique_ptr<UploadBuffer>(new UploadBuffer());
    m_UploadBuffer->PlacedCreate(bufferSize, pUploadPlacedHeap);

    D3D12_SUBRESOURCE_DATA srcData = {};
    srcData.pData = vertices;
    srcData.RowPitch = bufferSize;
    srcData.SlicePitch = bufferSize;
    UpdateSubresources(
        g_GraphicsCommandList.GetD3D12CommandList(),
        m_Resource.get(),
        m_UploadBuffer->GetD3D12Resource(),
        0, 0, 1,
        &srcData);

    auto barriers = CD3DX12_RESOURCE_BARRIER::Transition(
        m_Resource.get(),
        D3D12_RESOURCE_STATE_COPY_DEST,                 // 之前的状态
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);    // 之后的状态
    g_GraphicsCommandList->ResourceBarrier(1, &barriers);


    Finalize();

    // 创建顶点缓冲视图
    m_VertexBufferView = std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW>(new D3D12_VERTEX_BUFFER_VIEW{
            m_GpuVirtualAddress,
            bufferSize,
            strideSize });
}
#endif
