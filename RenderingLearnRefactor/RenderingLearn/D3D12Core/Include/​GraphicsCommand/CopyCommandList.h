#pragma once

#include "ICommandList.h"

namespace D3D12Core
{
    class CopyCommandList : public ICommandList
    {
    public:
        CopyCommandList() = default;

        void Create();

        /**
         * \brief 更新子资源
         * \param DstResource 目标资源
         * \param Intermediate 中间资源
         * \param RowPitch 子资源数据的行间距、宽度或物理大小（以字节为单位）
         * \param SlicePitch 子资源数据的深度间距、宽度或物理大小（以字节为单位）
         * \param pData 指向包含子资源数据的内存块的指针
         */
        void UpdateSubresources(IGraphicsResource* DstResource, const UploadBuffer* Intermediate, UINT64 RowPitch, UINT64 SlicePitch, const void* pData) const;

        /**
         * \brief 将源资源的全部内容复制到目标资源
         */
        void CopyResource(IGraphicsResource* DstResource, const IGraphicsResource* SrcResource) const;

        void ResourceBarrier(IGraphicsResource* resource, D3D12_RESOURCE_STATES after) const;


        void DispatchUploadBuffer(IBufferResource* resource, const void* data) const;
    };
}
