#include "pch.h"

#include "GraphicsCore.h"
#include "GpuPlacedHeap.h"

#include "UploadBuffer.h"

// --------------------------------------------------------------------------
/*
    上传堆缓冲
        对于GPU来说为只读，对于CPU来说可读可写（通常为只写）
*/
// --------------------------------------------------------------------------


using namespace std;

namespace Graphics
{

#if 0
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
            const auto size = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT * 1024;
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
                //ubp.pUploadBuffer->DirectCreate(size);
                //ubp.pUploadBuffer->SetResourceDesc(CD3DX12_RESOURCE_DESC::Buffer(size));
                //m_UploadPlacedHeap.PlacedResource()
            }
        }


    private:
        GpuPlacedHeap m_UploadPlacedHeap;

        vector<UploadBufferPack> m_AllocatedBuffers;
    };
#endif


    const CD3DX12_RANGE UploadBuffer::c_ZeroReadRange = CD3DX12_RANGE(0, 0);

    UploadBuffer::UploadBuffer() : m_VertexBufferView(nullptr) {}

    void UploadBuffer::DirectCreate(UINT64 size)
    {
        m_ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

        auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        CHECK_HRESULT(g_Device->CreateCommittedResource(
            &heapType,
            D3D12_HEAP_FLAG_NONE,
            &m_ResourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ, // 上传堆的初始状态必须此项，且不能更改
            nullptr,
            IID_PPV_ARGS(PutD3D12Resource())));// 注意：上传资源的生命周期必须等待GPU复制完成之后才能释放
        SET_DEBUGNAME(m_Resource.get(), _T("Resource"));

        Finalize();
    }

    void UploadBuffer::PlacedCreate(UINT64 size, GpuPlacedHeap& pPlacedHeap)
    {
        m_ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

        // 要放入的放置堆类型必须是上传堆
        ASSERT(pPlacedHeap.GetHeapDesc()->Properties.Type == D3D12_HEAP_TYPE_UPLOAD);

        pPlacedHeap.PlacedResource(D3D12_RESOURCE_STATE_GENERIC_READ, *this);

        Finalize();
    }

    void UploadBuffer::CopyVertexBuffer(UINT strideSize, const void* vertices)
    {
        ASSERT(m_Resource != nullptr);
        auto bufferSize = m_ResourceDesc.Width;

        UINT8* pVertexDataBegin = nullptr;
        Map(0, reinterpret_cast<void**>(&pVertexDataBegin));
        memcpy(pVertexDataBegin, vertices, bufferSize);
        Unmap(0);

        // 创建顶点缓冲视图
        m_VertexBufferView.reset(new D3D12_VERTEX_BUFFER_VIEW{
                m_GpuVirtualAddress,
                static_cast<UINT>(bufferSize),
                strideSize });

        TRACE(L"WARNING::正在使用上传堆顶点缓冲。");
    }
}
