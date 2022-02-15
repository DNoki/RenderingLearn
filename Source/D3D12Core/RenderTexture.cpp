#include "pch.h"

#include "GraphicsCore.h"
#include "Display.h"

#include "RenderTexture.h"

using namespace std;

namespace Graphics
{

    void RenderTexture::DirectCreateDSV(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth, UINT8 optStencil)
    {
        // 清除优化值
        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        {
            depthOptimizedClearValue.Format = format;
            depthOptimizedClearValue.DepthStencil.Depth = optDepth;
            depthOptimizedClearValue.DepthStencil.Stencil = optStencil;
        }

        // 资源描述
        {
            m_ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);

            // 允许为资源创建深度模板视图，并允许资源转换到 D3D12_RESOURCE_STATE_DEPTH_WRITE 和/或 D3D12_RESOURCE_STATE_DEPTH_READ 状态。
            m_ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        }

        auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        CHECK_HRESULT(g_Device->CreateCommittedResource(
            &heapType,
            D3D12_HEAP_FLAG_NONE,
            &m_ResourceDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE, // 允许深度模板缓冲写入
            &depthOptimizedClearValue,
            IID_PPV_ARGS(PutD3D12Resource())));

        // 深度模板视图描述
        m_DsvDesc = unique_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC>(new D3D12_DEPTH_STENCIL_VIEW_DESC());
        m_DsvDesc->Format = format;
        m_DsvDesc->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        m_DsvDesc->Flags = D3D12_DSV_FLAG_NONE;
    }

    void RenderTexture::GetRtvFromSwapChain(const SwapChain& swapChain, UINT index)
    {
        ASSERT(m_Resource == nullptr);

        m_RtvDesc = nullptr;
        CHECK_HRESULT(swapChain.GetD3D12SwapChain()->GetBuffer(index, IID_PPV_ARGS(PutD3D12Resource())));

        m_ResourceDesc = m_Resource->GetDesc();
    }
}
