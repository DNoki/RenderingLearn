#pragma once

#include "​GraphicsResource/DescriptorHandle.h"

namespace D3D12Core
{
    /**
     * \brief 描述符分配器
     */
    class DescriptorAllocator
    {
    public:
        DescriptorAllocator() = delete;

        /**
         * \brief 获取一个新的描述符
         * \param context
         * \param type
         * \return
         */
        static DescriptorHandle Allocat(const GraphicsContext& context, D3D12_DESCRIPTOR_HEAP_TYPE type);

    private:

    };
}
