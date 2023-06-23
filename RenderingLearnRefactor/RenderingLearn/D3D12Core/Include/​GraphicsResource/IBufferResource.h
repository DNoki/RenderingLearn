#pragma once

#include "IGraphicsResource.h"

namespace D3D12Core
{
    class IBufferResource : public IGraphicsResource
    {
    public:
        IBufferResource() = default;
        ~IBufferResource() override = 0 {};
        IBufferResource(const IBufferResource& buffer) = delete;
        IBufferResource(IBufferResource&& buffer) = default;
        IBufferResource& operator = (const IBufferResource& buffer) = delete;
        IBufferResource& operator = (IBufferResource&& buffer) = default;

        // --------------------------------------------------------------------------
        /**
         * @brief 获取缓冲大小
         * @return
        */
        virtual uint64 GetBufferSize() const = 0;

        virtual D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const = 0;

        /**
         * @brief 创建一个资源并为其分配内存
         * @param size
        */
        virtual void DirectCreate(uint64 size) = 0;
        /**
         * @brief 使用定位方式创建一个资源
         * @param size
         * @param pPlacedHeap
        */
        virtual void PlacedCreate(uint64 size) = 0;

    protected:
        virtual void Finalize() = 0;

    };
}
