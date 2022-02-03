#include "pch.h"

#include "GraphicsCore.h"
#include "GpuPlacedHeap.h"

#include "UploadBuffer.h"

using namespace std;
using namespace Graphics;

struct UploadBufferPack
{
    UINT64 StartIndex;
    UINT64 AlignmentSize;
    unique_ptr<UploadBuffer> pUploadBuffer;
};

class UploadBufferManager
{
public:
    UploadBufferManager()
    {
        const auto size =
            D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT * 1024;
            //4096ull * 4096ull * 4ull;
        m_UploadPlacedHeap.Create(D3D12_HEAP_TYPE_UPLOAD, size, D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);

    }

    UploadBuffer& Request(UINT64 size)
    {
        // 计算对齐大小
        size = UINT_UPPER(size, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
        ASSERT(size <= m_UploadPlacedHeap.GetHeapSize()); // 请求大小不能大于上传堆大小，否则需要使用一个更大的上传堆

        if (m_AllocatedBuffers.empty())
        {
            m_AllocatedBuffers.push_back(UploadBufferPack());
            auto& ubp = m_AllocatedBuffers.back();
            ubp.StartIndex = 0;
            ubp.AlignmentSize = UINT_UPPER(size, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
            ubp.pUploadBuffer = unique_ptr<UploadBuffer>(new UploadBuffer());

            // TODO 尝试修改放置堆可以乱序放入
            //ubp.pUploadBuffer->Create(size);
            ubp.pUploadBuffer->SetResourceDesc(CD3DX12_RESOURCE_DESC::Buffer(size));
            //m_UploadPlacedHeap.PlacedResource()
        }
    }


private:
    GpuPlacedHeap m_UploadPlacedHeap;

    vector<UploadBufferPack> m_AllocatedBuffers;
};


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

void UploadBuffer::Finalize()
{
    // Resource必须创建以后才可以完成初始化
    ASSERT(m_Resource != nullptr);
    m_GpuVirtualAddress = m_Resource->GetGPUVirtualAddress();
}
