#include "pch.h"
#include "​GraphicsCommand/CopyCommandList.h"
#include "CommandListUtil.h"

using namespace D3D12Core;

void CopyCommandList::Create()
{
    CreateImpl(D3D12_COMMAND_LIST_TYPE_COPY, TEXT("CopyCommandList"));
}

void CopyCommandList::UpdateSubresources(IGraphicsResource* DstResource, const UploadBuffer* Intermediate, uint64 RowPitch, uint64 SlicePitch, const void* pData) const
{
    CL_UpdateSubresources(this, DstResource, Intermediate, RowPitch, SlicePitch, pData);
}

void CopyCommandList::CopyResource(IGraphicsResource* DstResource, const IGraphicsResource* SrcResource) const
{
    CL_CopyResource(this, DstResource, SrcResource);
}

void CopyCommandList::ResourceBarrier(IGraphicsResource* resource, D3D12_RESOURCE_STATES after) const
{
    CL_ResourceBarrier(this, resource, after);
}

void CopyCommandList::DispatchUploadBuffer(IBufferResource* resource, const void* data) const
{
    CL_DispatchUploadBuffer(this, resource, data);
}
