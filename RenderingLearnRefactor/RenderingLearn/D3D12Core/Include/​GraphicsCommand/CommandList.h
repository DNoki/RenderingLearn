#pragma once

namespace D3D12Core
{
    class IGraphicsResource;
    class CommandAllocator;
    class RootSignature;
    class IPipelineState;
    class DescriptorHeap;
    class IBufferResource;

    class ICommandList
    {
    public:
        ICommandList() = default;
        virtual ~ICommandList() = 0 {};

        virtual void Close();
        virtual void Reset();

        bool IsLocked() const { return m_IsLocked; }

        D3D12_COMMAND_LIST_TYPE GetType() const { return m_Type; }
        CommandAllocator* GetCommandAllocator() const { return m_CommandAllocator; }

        ID3D12GraphicsCommandList5* GetD3D12CommandList() const { return m_CommandList.get(); }

    protected:
        D3D12_COMMAND_LIST_TYPE m_Type{}; // 命令列表类型
        CommandAllocator* m_CommandAllocator{}; // 命令分配器

        ComPtr<ID3D12GraphicsCommandList5> m_CommandList{};

        /**
         * \brief 命令列表锁定状态
         * true : 列表已关闭，不允许写入命令
         * false : 列表可以写入命令
         */
        bool m_IsLocked{};

    protected:

    };

    /**
     * \brief 图形命令列表
     */
    class GraphicsCommandList : public ICommandList
    {
    public:
        GraphicsCommandList() = default;
        ~GraphicsCommandList() override {};

        /**
         * @brief 创建命令队列
         * @param type 命令队列类型
         * @param isAllocate 是否分配内存
         * @param pso 初始管线状态
        */
        void Create();
        void Create(const IPipelineState* pso);
        void Reset() override { ICommandList::Reset(); };
        void Reset(const IPipelineState* pso);

#pragma region 捆绑包命令列表不可用
#if 0
        void ResourceBarrier(UINT numBarriers, const D3D12_RESOURCE_BARRIER* pBarriers) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->ResourceBarrier(numBarriers, pBarriers);
        }
#endif
        void ResourceTransitionBarrier(IGraphicsResource* resource, D3D12_RESOURCE_STATES after) const;
        void ResourceTransitionBarrier(IGraphicsResource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) const;
        void RSSetViewports(UINT numViewports, const D3D12_VIEWPORT* pViewports) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->RSSetViewports(numViewports, pViewports);
        }
        void RSSetScissorRects(UINT numRects, const D3D12_RECT* pRects) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->RSSetScissorRects(numRects, pRects);
        }
#if 0
        void OMSetRenderTargets(UINT numRTV, const D3D12_CPU_DESCRIPTOR_HANDLE* pRTVs, bool rtsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* pDSVs) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->OMSetRenderTargets(numRTV, pRTVs, rtsSingleHandleToDescriptorRange, pDSVs);
        }
#endif
        void OMSetRenderTargets(UINT rtvCount, const IRenderTarget* rtvArray);

        void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float colorRGBA[4], UINT numRects, const D3D12_RECT* pRects) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->ClearRenderTargetView(rtv, colorRGBA, numRects, pRects);
        }
        void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView, D3D12_CLEAR_FLAGS clearFlags, FLOAT depth, UINT8 stencil, UINT numRects, const D3D12_RECT* pRects) const
        {
            ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
            m_CommandList->ClearDepthStencilView(depthStencilView, clearFlags, depth, stencil, numRects, pRects);
        }
        //void DispatchBundleCommand(const CommandList* pCommandList) const
        //{
        //    ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
        //    m_CommandList->ExecuteBundle(pCommandList->GetD3D12CommandList());
        //}
#pragma endregion

        void SetGraphicsRootSignature(const RootSignature* pRootSignature) const;
        void SetPipelineState(const IPipelineState* pPipelineState) const;
        void SetDescriptorHeaps(const DescriptorHeap* pResourceDescHeap = nullptr, const DescriptorHeap* pSamplerDescHeap = nullptr) const;
        void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, const DescriptorHeap* baseDescriptor) const;
        void SetGraphicsRootConstantBufferView(UINT rootParameterIndex, const IBufferResource* buffer) const;


        void IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) const;
        void IASetVertexBuffers(UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews) const;
        void IASetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView) const;
        void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation) const;
        void DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation) const;

    };

    class CommandBundle : public ICommandList
    {
    public:
        CommandBundle() = default;
        ~CommandBundle() override {};

    };

    class ComputeCommandList : public ICommandList
    {
    public:
        ComputeCommandList() = default;
        ~ComputeCommandList() override {};

    };

    class CopyCommandList : public ICommandList
    {
    public:
        CopyCommandList() = default;
        ~CopyCommandList() override {};

        //void CopyBufferRegion() const;
        //void CopyTextureRegion() const;
        //void CopyResource() const;
        //void CopyTiles() const;

    };

    class CommandListPool
    {
    public:
        CommandListPool() = delete;

        template <class T>
        static T* Request()
        {
            static_assert(std::is_base_of<ICommandList, T>::value, TEXT("类型必须是 ICommandList 的子类"));
            if (typeid(T) == typeid(GraphicsCommandList))
            {
                return static_cast<GraphicsCommandList*>(RequstImpl(D3D12_COMMAND_LIST_TYPE_DIRECT));
            }
            return nullptr;
        }

        static void Restore(ICommandList** commandList);

    private:
        static ICommandList* RequstImpl(D3D12_COMMAND_LIST_TYPE type);

    };
}
