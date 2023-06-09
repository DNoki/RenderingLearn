#pragma once

namespace D3D12Core
{
    class PipelineStateManager
    {
    public:
        PipelineStateManager() = delete;

        static ID3D12PipelineState* GetPipelineState(HashValue psoDescHash);
        static void StorePipelineState(HashValue psoDescHash, ComPtr<ID3D12PipelineState>& pso);
        static void DestroyAllPlpelineState();

    private:

    };
}