#pragma once

namespace D3D12Core
{
#pragma region Resource State

    /**
     * \brief 同步单个资源状态
     * \param commandList
     * \param resource
     * \param after
     */
    extern void CL_ResourceBarrier(const ICommandList* commandList, IGraphicsResource* resource, D3D12_RESOURCE_STATES after);

    extern void CL_ClearRenderTargetView(const ICommandList* commandList, const IRenderTarget* renderTarget);

    extern void CL_ClearDepthStencilView(const ICommandList* commandList, const IRenderTarget* depthStencil);

#pragma endregion 

#pragma region Render Pipeline State

    /**
     * \brief 设置图形根签名的布局
     */
    extern void CL_SetGraphicsRootSignature(const ICommandList* commandList, const RootSignature* rootSignature);

    /**
     * \brief 设置图形处理单元 (GPU) 管道的所有着色器和程序大部分固定函数状态
     */
    extern void CL_SetPipelineState(const ICommandList* commandList, const IPipelineState* pPipelineState);

    /**
     * \brief 更改与命令列表关联的当前绑定描述符堆
     */
    extern void CL_SetDescriptorHeaps(const ICommandList* commandList, const DescriptorHeap* pResourceDescHeap, const DescriptorHeap* pSamplerDescHeap);

    /**
     * \brief 将描述符表绑定到图形根签名
     */
    extern void CL_SetGraphicsRootDescriptorTable(const ICommandList* commandList, uint32 rootParameterIndex, const DescriptorHeap* descriptorHeap);

    /**
     * \brief 将常量缓冲绑定到图形根签名
     */
    extern void CL_SetGraphicsRootConstantBufferView(const ICommandList* commandList, uint32 rootParameterIndex, const IBufferResource* buffer);

#pragma endregion

#pragma region IA - Input assembler stage

    extern void CL_IASetPrimitiveTopology(const ICommandList* commandList, D3D12_PRIMITIVE_TOPOLOGY primitiveTopology);

    /**
     * \brief 设置顶点缓冲区的 CPU 描述符句柄
     * \param commandList
     * \param startSlot 索引到设备的从零开始的数组中，开始设置顶点缓冲区
     * \param pViews 指定顶点缓冲区视图
     */
    extern void CL_IASetVertexBuffers(const ICommandList* commandList, uint32 startSlot, const D3D12_VERTEX_BUFFER_VIEW* pViews);

    /**
     * \brief 设置索引缓冲区的视图
     */
    extern void CL_IASetIndexBuffer(const ICommandList* commandList, const D3D12_INDEX_BUFFER_VIEW* pView);
#pragma endregion

#pragma region RS - Rasterizer stage

    /**
     * \brief 将视区数组绑定到管道的光栅器阶段
     * \param commandList
     * \param numViewports 要绑定的视区数。 有效值的范围 (0，D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)
     * \param pViewports 要绑定到设备的 D3D12_VIEWPORT 结构的数组
     */
    extern void CL_RSSetViewports(const ICommandList* commandList, uint32 numViewports, const D3D12_VIEWPORT* pViewports);

    /**
     * \brief 将剪刀矩形数组绑定到光栅器阶段
     * \param commandList
     * \param numRects 要绑定的剪刀矩形的数目
     * \param pRects 剪刀矩形数组
     */
    extern void CL_RSSetScissorRects(const ICommandList* commandList, uint32 numRects, const D3D12_RECT* pRects);

#pragma endregion

#pragma region OM - Output merger stage

    /**
     * \brief 设置渲染目标与深度模板视图
     * \param commandList
     * \param renderTargets
     */
    extern void CL_OMSetRenderTargets(const ICommandList* commandList, std::initializer_list<const IRenderTarget*> renderTargets);

    extern void CL_OMSetBlendFactor(const ICommandList* commandList, const float BlendFactor[4]);

    extern void CL_OMSetStencilRef(const ICommandList* commandList, uint32 StencilRef);

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
    extern void CL_DrawInstanced(const ICommandList* commandList, uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation);

    /**
     * \brief 绘制索引的实例化基元
     * \param commandList
     * \param indexCountPerInstance 从每个实例的索引缓冲区读取的索引数
     * \param instanceCount 要绘制的实例数
     * \param startIndexLocation GPU 从索引缓冲区读取的第一个索引的位置
     * \param baseVertexLocation 在从顶点缓冲区读取顶点之前，向每个索引添加一个值
     * \param startInstanceLocation 在从顶点缓冲区读取每实例数据之前，向每个索引添加一个值
     */
    extern void CL_DrawIndexedInstanced(const ICommandList* commandList, uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, int32 baseVertexLocation, uint32 startInstanceLocation);
#pragma endregion

#pragma region Copy
    /**
     * \brief 更新子资源
     */
    extern void CL_UpdateSubresources(
        const ICommandList* CommandList,
        IGraphicsResource* DstResource,
        const UploadBuffer* Intermediate,
        uint64 RowPitch,
        uint64 SlicePitch,
        const void* pData);

    extern void CL_CopyResource(const ICommandList* CommandList, IGraphicsResource* DstResource, const IGraphicsResource* SrcResource);
    extern void CL_CopyTextureRegion(
        const ICommandList* CommandList,
        const D3D12_TEXTURE_COPY_LOCATION* DstResource,
        uint32 DstX,
        uint32 DstY,
        uint32 DstZ,
        const D3D12_TEXTURE_COPY_LOCATION* pSrc,
        const D3D12_BOX* pSrcBox);

    /**
     * \brief 自动分配上传缓冲，拷贝数据到资源
     */
    extern void CL_DispatchUploadBuffer(const ICommandList* CommandList, IBufferResource* resource, const void* data);
    /**
     * \brief 自动分配上传缓冲，拷贝纹理数据到资源
     */
    extern void CL_DispatchUploadBuffer(const ICommandList* CommandList, ITexture* resource, const void* data);

#pragma endregion

}
