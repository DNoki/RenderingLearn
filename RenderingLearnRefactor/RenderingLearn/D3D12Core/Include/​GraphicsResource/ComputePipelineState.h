#pragma once

#include "IPipelineState.h"

namespace D3D12Core
{
    class ComputePipelineState : public IPipelineState
    {
        // TODO

    private:
        D3D12_COMPUTE_PIPELINE_STATE_DESC m_ComputePSODesc{};

    };
}
