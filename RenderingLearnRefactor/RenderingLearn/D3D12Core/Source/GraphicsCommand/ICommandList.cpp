#include "pch.h"
#include "​GraphicsCommand/ICommandList.h"

// --------------------------------------------------------------------------
/*
    创建和记录命令列表和捆绑包 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/recording-command-lists-and-bundles

    命令列表(CommandList)
        命令列表通常执行一次。 但是，如果应用程序在提交新执行之前确保先前的执行完成，则可以多次执行命令列表。
        任何线程都可以随时向任何命令队列提交命令列表，运行时将自动序列化命令队列中的命令列表提交，同时保留提交顺序。

    GPU 工作项的分组(Bundles)
        除了命令列表之外，Direct3D 12 通过添加第二级命令列表（称为bundles）来利用当今所有硬件中存在的功能。为了帮助区分这两种类型，可以将第一级命令列表称为直接命令列表。
        捆绑包的目的是允许应用程序将少量 API 命令组合在一起，以便以后从直接命令列表中重复执行。
        在创建 bundle 时，驱动程序将执行尽可能多的预处理，以提高后续执行的效率。然后可以从多个命令列表中执行捆绑包，并在同一命令列表中多次执行。

    图形管线
        DX11 https://docs.microsoft.com/zh-cn/windows/win32/direct3d11/overviews-direct3d-11-graphics-pipeline
        DX12 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/pipelines-and-shaders-with-directx-12
        IA：Input-Assembler
        SO：Stream Output
        RS：Rasterizer Stage
        OM：Output Merger

*/
// --------------------------------------------------------------------------

using namespace D3D12Core;

namespace D3D12Core
{
#pragma region Resource State

    /**
     * \brief 同步单个资源状态
     * \param commandList
     * \param resource
     * \param after
     */
    void CL_ResourceBarrier(const ICommandList* commandList, IGraphicsResource* resource, D3D12_RESOURCE_STATES after)
    {
        // 资源屏障同步资源状态 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/using-resource-barriers-to-synchronize-resource-states-in-direct3d-12

        auto before = resource->GetResourceStates();

        // 前后资源一致时忽略状态转换
        if (before == after)
            return;

        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource->GetD3D12Resource(), before, after);
        commandList->GetD3D12CommandList()->ResourceBarrier(1, &barrier);

        resource->SetResourceStates(after);
    }


    void CL_ClearRenderTargetView(const ICommandList* commandList, const IRenderTarget* renderTarget, const float colorRGBA[4])
    {
        commandList->GetD3D12CommandList()->ClearRenderTargetView(renderTarget->GetRtvOrDsv(), colorRGBA, 0, nullptr);
    }

    void CL_ClearDepthStencilView(const ICommandList* commandList, const IRenderTarget* depthStencil)
    {
        auto* clearValue = depthStencil->GetClearValue();
        D3D12_CLEAR_FLAGS clearFlags;
        switch (depthStencil->GetDsvDesc()->Flags)
        {
        case D3D12_DSV_FLAG_NONE:
            clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
            break;
        case D3D12_DSV_FLAG_READ_ONLY_DEPTH:
            clearFlags = D3D12_CLEAR_FLAG_DEPTH;
            break;
        case D3D12_DSV_FLAG_READ_ONLY_STENCIL:
            clearFlags = D3D12_CLEAR_FLAG_STENCIL;
            break;
        default: throw;
        }

        commandList->GetD3D12CommandList()->ClearDepthStencilView(
            depthStencil->GetRtvOrDsv(), clearFlags,
            clearValue->DepthStencil.Depth,
            clearValue->DepthStencil.Stencil,
            0, nullptr);
    }

#pragma endregion 

#pragma region Render Pipeline State

    /**
     * \brief 设置图形根签名的布局
     */
    void CL_SetGraphicsRootSignature(const ICommandList* commandList, const RootSignature* rootSignature)
    {
        commandList->GetD3D12CommandList()->SetGraphicsRootSignature(rootSignature->GetD3D12RootSignature());
    }

    /**
     * \brief 设置图形处理单元 (GPU) 管道的所有着色器和程序大部分固定函数状态
     */
    void CL_SetPipelineState(const ICommandList* commandList, const IPipelineState* pPipelineState)
    {
        commandList->GetD3D12CommandList()->SetPipelineState(pPipelineState->GetD3D12PSO());
    }

    /**
     * \brief 更改与命令列表关联的当前绑定描述符堆
     */
    void CL_SetDescriptorHeaps(const ICommandList* commandList, const DescriptorHeap* pResourceDescHeap, const DescriptorHeap* pSamplerDescHeap)
    {
        /*
            命令列表只能绑定 CBV_SRV_UAV 或 SAMPLER 类型描述符堆
            每种类型的描述符堆一次只能设置一个，即一次最多可以设置2个堆
        */
        Vector<ID3D12DescriptorHeap*> descHeaps{};
        if (pResourceDescHeap)
            descHeaps.push_back(pResourceDescHeap->GetD3D12DescriptorHeap());
        if (pSamplerDescHeap)
            descHeaps.push_back(pSamplerDescHeap->GetD3D12DescriptorHeap());

        if (descHeaps.size() > 0)
            commandList->GetD3D12CommandList()->SetDescriptorHeaps(static_cast<UINT>(descHeaps.size()), descHeaps.data());
    }

    /**
     * \brief 将描述符表绑定到图形根签名
     */
    void CL_SetGraphicsRootDescriptorTable(const ICommandList* commandList, UINT rootParameterIndex, const DescriptorHeap* descriptorHeap)
    {
        commandList->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(rootParameterIndex, descriptorHeap->GetDescriptorHandle(0));
    }

    /**
     * \brief 将常量缓冲绑定到图形根签名
     */
    void CL_SetGraphicsRootConstantBufferView(const ICommandList* commandList, UINT rootParameterIndex, const IBufferResource* buffer)
    {
        commandList->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(rootParameterIndex, buffer->GetGpuVirtualAddress());
    }

#pragma endregion

#pragma region IA - Input assembler stage

    void CL_IASetPrimitiveTopology(const ICommandList* commandList, D3D12_PRIMITIVE_TOPOLOGY primitiveTopology)
    {
        commandList->GetD3D12CommandList()->IASetPrimitiveTopology(primitiveTopology);
    }

    /**
     * \brief 设置顶点缓冲区的 CPU 描述符句柄
     * \param commandList
     * \param startSlot 索引到设备的从零开始的数组中，开始设置顶点缓冲区
     * \param pViews 指定顶点缓冲区视图
     */
    void CL_IASetVertexBuffers(const ICommandList* commandList, UINT startSlot, const D3D12_VERTEX_BUFFER_VIEW* pViews)
    {
        commandList->GetD3D12CommandList()->IASetVertexBuffers(startSlot, 1, pViews);
    }

    /**
     * \brief 设置索引缓冲区的视图
     */
    void CL_IASetIndexBuffer(const ICommandList* commandList, const D3D12_INDEX_BUFFER_VIEW* pView)
    {
        commandList->GetD3D12CommandList()->IASetIndexBuffer(pView);
    }
#pragma endregion

#pragma region RS - Rasterizer stage

    /**
     * \brief 将视区数组绑定到管道的光栅器阶段
     * \param commandList
     * \param numViewports 要绑定的视区数。 有效值的范围 (0，D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)
     * \param pViewports 要绑定到设备的 D3D12_VIEWPORT 结构的数组
     */
    void CL_RSSetViewports(const ICommandList* commandList, UINT numViewports, const D3D12_VIEWPORT* pViewports)
    {
        commandList->GetD3D12CommandList()->RSSetViewports(numViewports, pViewports);
    }

    /**
     * \brief 将剪刀矩形数组绑定到光栅器阶段
     * \param commandList
     * \param numRects 要绑定的剪刀矩形的数目
     * \param pRects 剪刀矩形数组
     */
    void CL_RSSetScissorRects(const ICommandList* commandList, UINT numRects, const D3D12_RECT* pRects)
    {
        commandList->GetD3D12CommandList()->RSSetScissorRects(numRects, pRects);
    }

#pragma endregion

#pragma region OM - Output merger stage

    /**
     * \brief 设置渲染目标与深度模板视图
     * \param commandList
     * \param renderTargets
     */
    void CL_OMSetRenderTargets(const ICommandList* commandList, std::initializer_list<const IRenderTarget*> renderTargets)
    {
        Vector<CpuDescriptorHandle> RtvHandles{};
        Vector<CpuDescriptorHandle> DsvHandle;

        for (auto rt = renderTargets.begin(); rt != renderTargets.end(); ++rt)
        {
            if (!*rt)
                continue;

            if ((*rt)->GetType() == RenderTargetType::Color)
            {
                RtvHandles.push_back((*rt)->GetRtvOrDsv());
            }
            else if ((*rt)->GetType() == RenderTargetType::DepthStencil)
            {
                DsvHandle.push_back((*rt)->GetRtvOrDsv());
            }
        }

        ASSERT(0 < RtvHandles.size());
        ASSERT(DsvHandle.size() < 2);

        commandList->GetD3D12CommandList()->OMSetRenderTargets(
            static_cast<UINT>(RtvHandles.size()),
            RtvHandles.data(),
            // True 表示传入的 RTV 指向连续的描述符范围指针，其在 GPU 内存上是连续存储的
            // False 表示传入的 RTV 是一个描述符句柄数组
            FALSE,
            DsvHandle.size() == 0 ? nullptr : DsvHandle.data());
    }

    void CL_OMSetBlendFactor(const ICommandList* commandList, const float BlendFactor[4])
    {
        commandList->GetD3D12CommandList()->OMSetBlendFactor(BlendFactor);
    }

    void CL_OMSetStencilRef(const ICommandList* commandList, UINT StencilRef)
    {
        commandList->GetD3D12CommandList()->OMSetStencilRef(StencilRef);
    }

#pragma endregion

#pragma region Draw Call

    /**
     * \brief 绘制非索引的实例化基元
     * \param commandList
     * \param vertexCountPerInstance 要绘制的顶点数
     * \param instanceCount 要绘制的实例数
     * \param startVertexLocation 第一个顶点的索引
     * \param startInstanceLocation 在从顶点缓冲区读取每实例数据之前，向每个索引添加一个值
     */
    void CL_DrawInstanced(const ICommandList* commandList, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation)
    {
        commandList->GetD3D12CommandList()->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
    }

    /**
     * \brief 绘制索引的实例化基元
     * \param commandList
     * \param indexCountPerInstance 从每个实例的索引缓冲区读取的索引数
     * \param instanceCount 要绘制的实例数
     * \param startIndexLocation GPU 从索引缓冲区读取的第一个索引的位置
     * \param baseVertexLocation 在从顶点缓冲区读取顶点之前，向每个索引添加一个值
     * \param startInstanceLocation 在从顶点缓冲区读取每实例数据之前，向每个索引添加一个值
     */
    void CL_DrawIndexedInstanced(const ICommandList* commandList, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation)
    {
        commandList->GetD3D12CommandList()->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }
#pragma endregion

#pragma region Copy

    /**
     * \brief 更新子资源，应通过调用 ID3D12Device：：GetCopyableFootprints 填充所有子资源数组。
     */
    void CL_UpdateSubresources(
        const ICommandList* CommandList,
        IGraphicsResource* DstResource,
        const UploadBuffer* Intermediate,
        UINT64 RowPitch,
        UINT64 SlicePitch,
        const void* pData)
    {
        ASSERT(Intermediate->GetResourceStates() & D3D12_RESOURCE_STATE_COPY_SOURCE);
        ASSERT(DstResource->GetResourceStates() & D3D12_RESOURCE_STATE_COPY_DEST);

        D3D12_SUBRESOURCE_DATA srcData;
        srcData.pData = pData;
        srcData.RowPitch = RowPitch;
        srcData.SlicePitch = SlicePitch;
        UpdateSubresources(
            CommandList->GetD3D12CommandList(),
            DstResource->GetD3D12Resource(),
            Intermediate->GetD3D12Resource(),
            0, 0, 1, &srcData);
    }

    /**
     * \brief 将源资源的全部内容复制到目标资源
     */
    void CL_CopyResource(const ICommandList* CommandList, const IGraphicsResource* DstResource, const IGraphicsResource* SrcResource)
    {
        ASSERT(SrcResource->GetResourceStates() & D3D12_RESOURCE_STATE_COPY_SOURCE);
        ASSERT(DstResource->GetResourceStates() & D3D12_RESOURCE_STATE_COPY_DEST);
        CommandList->GetD3D12CommandList()->CopyResource(DstResource->GetD3D12Resource(), SrcResource->GetD3D12Resource());
    }
    void CL_CopyTextureRegion(
        const ICommandList* CommandList,
        const D3D12_TEXTURE_COPY_LOCATION* DstResource,
        UINT DstX,
        UINT DstY,
        UINT DstZ,
        const D3D12_TEXTURE_COPY_LOCATION* pSrc,
        const D3D12_BOX* pSrcBox)
    {
        CommandList->GetD3D12CommandList()->CopyTextureRegion(DstResource, DstX, DstY, DstZ, pSrc, pSrcBox);
    }
#pragma endregion

}


void ICommandList::Close()
{
    ASSERT(!m_IsLocked);

    // 关闭命令列表
    CHECK_HRESULT(m_CommandList->Close());
    m_IsLocked = true;

    // 命令写入分配器之后，解除之前绑定的分配器
    m_CommandAllocator = nullptr;
}

void ICommandList::Reset()
{
    // 在应用程序调用Reset之前，命令列表必须处于“关闭”状态。
    ASSERT(m_IsLocked);

    // 返回一个命令分配器
    ASSERT(!m_CommandAllocator);
    m_CommandAllocator = CommandAllocatorPool::Request(m_Type);

    // 将命令列表重置回其初始状态
    // 通过使用Reset，您可以重用命令列表跟踪结构而无需任何分配。与ID3D12CommandAllocator::Reset不同，您可以在命令列表仍在执行时调用Reset。一个典型的模式是提交一个命令列表，然后立即重置它以将分配的内存重新用于另一个命令列表。
    CHECK_HRESULT(m_CommandList->Reset(m_CommandAllocator->GetD3D12Allocator(), nullptr));

    // 指示列表可以写入命令
    m_IsLocked = false;
}
