#pragma once

namespace Graphics
{
    class PipelineStateManager
    {
    public:
        PipelineStateManager() = delete;

        static ID3D12PipelineState* GetPipelineState(UINT64 psoDescHash);
        static void StorePipelineState(UINT64 psoDescHash, winrt::com_ptr<ID3D12PipelineState>& pso);
        static void DestoryAllPlpelineState();

    private:

    };
}