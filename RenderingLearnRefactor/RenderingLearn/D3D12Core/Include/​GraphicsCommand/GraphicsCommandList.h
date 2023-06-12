#pragma once

#include "ICommandList.h"

namespace D3D12Core
{
    /**
     * \brief 图形命令列表
     */
    class GraphicsCommandList : public ICommandList
    {
    public:
        GraphicsCommandList() = default;

        /**
         * @brief 创建命令队列
        */
        void Create();
        void Create(const IPipelineState* pso);
        void Reset() override { ICommandList::Reset(); };
        void Reset(const IPipelineState* pso);


        void ResourceBarrier(IGraphicsResource* resource, D3D12_RESOURCE_STATES after) const;

        void ClearRenderTargetView(const IRenderTarget* renderTarget, const float colorRGBA[4]) const;
        void ClearDepthStencilView(const IRenderTarget* depthStencil) const;

        void SetGraphicsRootSignature(const RootSignature* pRootSignature) const;
        void SetPipelineState(const IPipelineState* pPipelineState) const;
        void SetDescriptorHeaps(const DescriptorHeap* pResourceDescHeap = nullptr, const DescriptorHeap* pSamplerDescHeap = nullptr) const;
        void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, const DescriptorHeap* baseDescriptor) const;
        void SetGraphicsRootConstantBufferView(UINT rootParameterIndex, const IBufferResource* buffer) const;

        void IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) const;
        void IASetVertexBuffers(UINT startSlot, const D3D12_VERTEX_BUFFER_VIEW* VBV) const;
        void IASetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* IBV) const;

        void RSSetViewports(float topLeftX, float topLeftY, float width, float height) const;
        void RSSetScissorRects(UINT Left, UINT Top, UINT Right, UINT Bottom) const;

        void OMSetRenderTarget(const IRenderTarget* renderTarget, const IRenderTarget* depthStencil = nullptr) const;

        void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount = 1, UINT startVertexLocation = 0, UINT startInstanceLocation = 0) const;
        void DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount = 1, UINT startIndexLocation = 0, INT baseVertexLocation = 0, UINT startInstanceLocation = 0) const;

        void DispatchBundleCommand(const class CommandBundle* pCommandList);

    };
}
