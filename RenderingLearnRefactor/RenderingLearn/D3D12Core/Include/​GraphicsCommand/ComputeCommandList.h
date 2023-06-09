#pragma once

#include "ICommandList.h"

namespace D3D12Core
{
    class ComputeCommandList : public ICommandList
    {
    public:
        ComputeCommandList() = default;
        //~ComputeCommandList() override {}

    };
}
