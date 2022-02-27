#pragma once

#include "IResource.h"

namespace Graphics
{
    class UploadBuffer;
    class GpuPlacedHeap;
    class CommandList;

    /**
     * @brief GPU 缓冲
    */
    class GraphicsBuffer : public IBufferResource
    {
    public:
        // --------------------------------------------------------------------------
        GraphicsBuffer();


        // --------------------------------------------------------------------------
        /**
         * @brief 创建指定大小的默认堆
         * @param size
        */
        void DirectCreate(UINT64 size) override;
        /**
         * @brief 使用定位方式创建一个默认堆
         * @param size
         * @param pPlacedHeap
        */
        void PlacedCreate(UINT64 size) override;

        /**
         * @brief 添加拷贝缓冲命令
         * @param commandList 拷贝命令队列
         * @param data
        */
        void DispatchCopyBuffer(const CommandList& commandList, const void* data);


    private:
        std::unique_ptr<UploadBuffer> m_UploadBuffer;
    };
}