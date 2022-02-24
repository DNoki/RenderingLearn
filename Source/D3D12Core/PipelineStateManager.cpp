#include "pch.h"


#include "PipelineStateManager.h"

using namespace std;
using namespace winrt;


namespace Graphics
{
    map<size_t, com_ptr<ID3D12PipelineState>> g_GraphicsPipelineStateMap;

    ID3D12PipelineState* PipelineStateManager::GetPipelineState(UINT64 psoDescHash)
    {
        auto finded = g_GraphicsPipelineStateMap.find(psoDescHash);

        if (finded != g_GraphicsPipelineStateMap.end())
        {
            return g_GraphicsPipelineStateMap[psoDescHash].get();
        }
        return nullptr;
    }

    void PipelineStateManager::StorePipelineState(UINT64 psoDescHash, com_ptr<ID3D12PipelineState>& pso)
    {
        g_GraphicsPipelineStateMap[psoDescHash] = move(pso);
    }
}

