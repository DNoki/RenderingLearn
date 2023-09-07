#include "pch.h"
#include "​GraphicsCommand/GraphicsCommandList.h"
#include "CommandListUtil.h"

using namespace D3D12Core;

void GraphicsCommandList::Create()
{
    CreateImpl(D3D12_COMMAND_LIST_TYPE_DIRECT, TEXT("GraphicsCommandList"));
}

void GraphicsCommandList::Create(const IPipelineState* pso)
{
    ASSERT(pso);
    m_Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    // 返回一个命令分配器
    m_CommandAllocator = CommandAllocatorPool::Request(m_Type);

    CHECK_HRESULT(GraphicsContext::GetCurrentInstance()->GetDevice()->CreateCommandList(
        GraphicsContext::GetCurrentInstance()->GetNodeMask(),
        m_Type,                     // 命令列表类型
        m_CommandAllocator->GetD3D12Allocator(),    // 命令列表分配器
        pso->GetD3D12PSO(),         // 初始管线状态
        IID_PPV_ARGS(m_CommandList.put())));

    // 指示列表可以写入命令
    m_IsLocked = false;

    GraphicsContext::SetDebugName(m_CommandList.get(), TEXT("GraphicsCommandList"));
}

void GraphicsCommandList::Reset(const IPipelineState* pso)
{
    ASSERT(m_IsLocked);
    ASSERT(pso);
    ASSERT(!m_CommandAllocator);
    m_CommandAllocator = CommandAllocatorPool::Request(m_Type);

    CHECK_HRESULT(m_CommandList->Reset(m_CommandAllocator->GetD3D12Allocator(), pso->GetD3D12PSO()));

    // 指示列表可以写入命令
    m_IsLocked = false;
}

//ICommandList::~CommandList()
//{
//    if (m_Type == D3D12_COMMAND_LIST_TYPE_BUNDLE)
//    {
//        // 回收捆绑包使用的分配器
//        ASSERT(m_CommandAllocator);
//        CommandAllocatorPool::Restore(&m_CommandAllocator);
//    }
//}

void GraphicsCommandList::ResourceBarrier(IGraphicsResource* resource, D3D12_RESOURCE_STATES after) const
{
    CL_ResourceBarrier(this, resource, after);
}
void GraphicsCommandList::ClearRenderTargetView(const IRenderTarget* renderTarget) const
{
    CL_ClearRenderTargetView(this, renderTarget);
}

void GraphicsCommandList::ClearDepthStencilView(const IRenderTarget* depthStencil) const
{
    CL_ClearDepthStencilView(this, depthStencil);
}

void GraphicsCommandList::SetGraphicsRootSignature(const RootSignature* pRootSignature) const
{
    CL_SetGraphicsRootSignature(this, pRootSignature);
}
void GraphicsCommandList::SetPipelineState(const IPipelineState* pPipelineState) const
{
    CL_SetPipelineState(this, pPipelineState);
}
void GraphicsCommandList::SetDescriptorHeaps(const DescriptorHeap* pResourceDescHeap, const DescriptorHeap* pSamplerDescHeap) const
{
    CL_SetDescriptorHeaps(this, pResourceDescHeap, pSamplerDescHeap);
}
void GraphicsCommandList::SetGraphicsRootDescriptorTable(uint32 rootParameterIndex, const DescriptorHeap* descriptorHeap) const
{
    CL_SetGraphicsRootDescriptorTable(this, rootParameterIndex, descriptorHeap);
}
void GraphicsCommandList::SetGraphicsRootConstantBufferView(uint32 rootParameterIndex, const IBufferResource* buffer) const
{
    CL_SetGraphicsRootConstantBufferView(this, rootParameterIndex, buffer);
}

void GraphicsCommandList::IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) const
{
    CL_IASetPrimitiveTopology(this, primitiveTopology);
}
void GraphicsCommandList::IASetVertexBuffers(uint32 startSlot, const D3D12_VERTEX_BUFFER_VIEW* VBV) const
{
    CL_IASetVertexBuffers(this, startSlot, VBV);
}
void GraphicsCommandList::IASetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* IBV) const
{
    CL_IASetIndexBuffer(this, IBV);
}

void GraphicsCommandList::RSSetViewports(float topLeftX, float topLeftY, float width, float height) const
{
    const auto viewport = CD3DX12_VIEWPORT(topLeftX, topLeftY, width, height);
    CL_RSSetViewports(this, 1, &viewport);
}

void GraphicsCommandList::RSSetScissorRects(uint32 Left, uint32 Top, uint32 Right, uint32 Bottom) const
{
    const auto scissorRect = CD3DX12_RECT(Left, Top, Right, Bottom);
    CL_RSSetScissorRects(this, 1, &scissorRect);
}

void GraphicsCommandList::OMSetRenderTarget(const IRenderTarget* renderTarget, const IRenderTarget* depthStencil) const
{
    CL_OMSetRenderTargets(this, { renderTarget, depthStencil });
}

void GraphicsCommandList::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) const
{
    CL_DrawInstanced(this, vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void GraphicsCommandList::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, int32 baseVertexLocation, uint32 startInstanceLocation) const
{
    CL_DrawIndexedInstanced(this, indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}
