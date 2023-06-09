#include "pch.h"
#include "​GraphicsCommand/CopyCommandList.h"
#include "CommandListUtil.h"

using namespace D3D12Core;

void CopyCommandList::Create()
{
}

void CopyCommandList::UpdateSubresources(IGraphicsResource* DstResource, const UploadBuffer* Intermediate, UINT64 RowPitch, UINT64 SlicePitch, const void* pData) const
{
    // 转换目标资源到拷贝状态
    ResourceBarrier(DstResource, D3D12_RESOURCE_STATE_COPY_DEST);
    CL_UpdateSubresources(this, DstResource, Intermediate, RowPitch, SlicePitch, pData);
}

void CopyCommandList::CopyResource(IGraphicsResource* DstResource, const IGraphicsResource* SrcResource) const
{
    // 转换目标资源到拷贝状态
    ResourceBarrier(DstResource, D3D12_RESOURCE_STATE_COPY_DEST);
    CL_CopyResource(this, DstResource, SrcResource);
}

void CopyCommandList::ResourceBarrier(IGraphicsResource* resource, D3D12_RESOURCE_STATES after) const
{
    CL_ResourceBarrier(this, resource, after);
}

void CopyCommandList::DispatchCopyBuffer(IBufferResource* resource, const void* data) const
{
    ASSERT(resource->GetD3D12Resource());
    auto bufferSize = resource->GetResourceDesc().Width;

    // 创建上传缓冲
    SharedPtr<UploadBuffer> m_UploadBuffer;
    m_UploadBuffer.reset(new UploadBuffer());
    m_UploadBuffer->PlacedCreate(bufferSize);

    UpdateSubresources(resource, m_UploadBuffer.get(), bufferSize, bufferSize, data);

    // 拷贝完成后释放上传堆
    GetCommandAllocator()->AssignOnCompletedCallback([&]()
        {
            m_UploadBuffer.reset();
        });
}
