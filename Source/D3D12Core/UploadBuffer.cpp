#include "pch.h"

#include "GraphicsCore.h"

#include "UploadBuffer.h"

using namespace Graphics;

UploadBuffer::UploadBuffer()
{
}

void UploadBuffer::Create(UINT64 size)
{
    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

    // TODO 当使用放置堆时，需要生成隐式堆吗
    auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CHECK_HRESULT(g_Device->CreateCommittedResource(
        &uploadHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &m_ResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, // 上传堆的初始状态必须此项，且不能更改
        nullptr,
        IID_PPV_ARGS(PutD3D12Resource())));// 注意：上传资源的生命周期必须等待GPU复制完成之后才能释放

    Finalize();
}
