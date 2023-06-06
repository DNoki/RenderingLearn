#pragma once

#include "​GraphicsResource/DescriptorHandle.h"

namespace D3D12Core
{
    class DescriptorAllocator
    {
    public:
        DescriptorAllocator() = delete;

        static DescriptorHandle Allocat(const GraphicsContext* context, D3D12_DESCRIPTOR_HEAP_TYPE type);

    private:

    };
}
