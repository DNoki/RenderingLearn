﻿#pragma once

#include "MultiRenderTargets.h"

namespace Graphics
{
    class CommandAllocator;
    class RootSignature;
    class PipelineState;
    class DescriptorHeap;

    class CommandList
    {
    public:
        CommandList();
        ~CommandList();

        /**
         * @brief 创建命令队列
         * @param type 命令队列类型
         * @param isAllocate 是否分配内存
         * @param pso 初始管线状态
        */
        void Create(D3D12_COMMAND_LIST_TYPE type, bool isAllocate = false, const PipelineState* pso = nullptr);

        void Close();
        void Reset(const PipelineState* pso = nullptr);

        inline bool IsLocked() const { return m_IsLocked; }

        inline D3D12_COMMAND_LIST_TYPE GetType() const { return m_Type; }
        inline CommandAllocator* GetCommandAllocator() const { return m_CommandAllocator; }
        inline ID3D12GraphicsCommandList5* GetD3D12CommandList() const { return m_CommandList.get(); }

        inline const MultiRenderTargets* GetRenderTargetInfos() const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            return &m_CurrentRenderTargets;
        }

#pragma region 捆绑包命令列表不可用
        inline void ResourceBarrier(UINT numBarriers, const D3D12_RESOURCE_BARRIER* pBarriers) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->ResourceBarrier(numBarriers, pBarriers);
        }
        inline void RSSetViewports(UINT numViewports, const D3D12_VIEWPORT* pViewports) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->RSSetViewports(numViewports, pViewports);
        }
        inline void RSSetScissorRects(UINT numRects, const D3D12_RECT* pRects) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->RSSetScissorRects(numRects, pRects);
        }
#if 0
        inline void OMSetRenderTargets(UINT numRTV, const D3D12_CPU_DESCRIPTOR_HANDLE* pRTVs, bool rtsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* pDSVs) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->OMSetRenderTargets(numRTV, pRTVs, rtsSingleHandleToDescriptorRange, pDSVs);
        }
#endif
        inline void OMSetRenderTargets(const MultiRenderTargets* mrt)
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CurrentRenderTargets = *mrt;
            m_CommandList->OMSetRenderTargets(m_CurrentRenderTargets.GetRenderTargetCount(), m_CurrentRenderTargets.GetRenderTargets(), FALSE, m_CurrentRenderTargets.GetDepthStencil());
        }
        inline void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtv, const Color& colorRGBA, UINT numRects, const D3D12_RECT* pRects) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->ClearRenderTargetView(rtv, colorRGBA, numRects, pRects);
        }
        inline void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView, D3D12_CLEAR_FLAGS clearFlags, FLOAT depth, UINT8 stencil, UINT numRects, const D3D12_RECT* pRects) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->ClearDepthStencilView(depthStencilView, clearFlags, depth, stencil, numRects, pRects);
        }
        inline void ExecuteBundle(const CommandList* pCommandList) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->ExecuteBundle(pCommandList->GetD3D12CommandList());
        }
#pragma endregion

        void SetGraphicsRootSignature(const RootSignature* pRootSignature) const;
        void SetPipelineState(const PipelineState* pPipelineState) const;
        void SetDescriptorHeaps(const DescriptorHeap* pResourceDescHeap = nullptr, const DescriptorHeap* pSamplerDescHeap = nullptr) const;
        inline void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor) const
        {
            m_CommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, baseDescriptor);
        }

        inline void IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) const
        {
            m_CommandList->IASetPrimitiveTopology(primitiveTopology);
        }
        inline void IASetVertexBuffers(UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews) const
        {
            m_CommandList->IASetVertexBuffers(startSlot, numViews, pViews);
        }
        inline void IASetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView) const
        {
            m_CommandList->IASetIndexBuffer(pView);
        }
        inline void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation) const
        {
            m_CommandList->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
        }
        inline void DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation) const
        {
            m_CommandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
        }

    private:
        D3D12_COMMAND_LIST_TYPE m_Type;
        CommandAllocator* m_CommandAllocator;

        winrt::com_ptr<ID3D12GraphicsCommandList5> m_CommandList;
        //winrt::com_ptr<ID3D12CommandList> m_CommandList;

        bool m_IsLocked; // 是否允许写入命令

        MultiRenderTargets m_CurrentRenderTargets;
    };
}
