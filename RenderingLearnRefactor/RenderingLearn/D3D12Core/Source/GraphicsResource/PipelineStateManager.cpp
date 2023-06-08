#include "pch.h"
#include "​GraphicsResource/PipelineStateManager.h"

using namespace D3D12Core;


static Map<size_t, ComPtr<ID3D12PipelineState>> g_GraphicsPipelineStateMap{};

ID3D12PipelineState* PipelineStateManager::GetPipelineState(UINT64 psoDescHash)
{
    auto finded = g_GraphicsPipelineStateMap.find(psoDescHash);

    if (finded != g_GraphicsPipelineStateMap.end())
    {
        return g_GraphicsPipelineStateMap[psoDescHash].get();
    }
    return nullptr;
}

void PipelineStateManager::StorePipelineState(UINT64 psoDescHash, ComPtr<ID3D12PipelineState>& pso)
{
    g_GraphicsPipelineStateMap[psoDescHash] = std::move(pso);
}

void PipelineStateManager::DestoryAllPlpelineState()
{
    g_GraphicsPipelineStateMap.clear();
}
