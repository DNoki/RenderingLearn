#pragma once

#include "IGraphicsResource.h"

namespace D3D12Core
{
    class UploadBuffer;
    class GraphicsCommandList;

    /**
     * @brief GPU 缓冲
    */
    class GraphicsBuffer : public IBufferResource
    {
    public:
        GraphicsBuffer() = default;
        ~GraphicsBuffer() override = default;

        GraphicsBuffer(const GraphicsBuffer& buffer) = delete;
        GraphicsBuffer(GraphicsBuffer&& buffer) = default;

        GraphicsBuffer& operator = (const GraphicsBuffer& buffer) = delete;
        GraphicsBuffer& operator = (GraphicsBuffer&& buffer) = default;

        // --------------------------------------------------------------------------
        UINT64  GetBufferSize() const override { return m_ResourceDesc.Width; }
        D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const override { return m_GpuVirtualAddress; }

        //virtual void SetName(const std::wstring& name) override
        //{
        //    m_Name = std::wstring(name);
        //    if (m_Resource) SET_DEBUGNAME(m_Resource.get(), Application::Format(_T("%s (GraphicsBuffer)"), m_Name.c_str()));
        //}

        // --------------------------------------------------------------------------
        /**
         * @brief 创建指定大小的默认堆
         * @param size
        */
        void DirectCreate(UINT64 size) override;
        /**
         * @brief 使用定位方式创建一个默认堆
         * @param size
         * @param pPlacedHeap
        */
        void PlacedCreate(UINT64 size) override;

        /**
         * @brief 添加拷贝缓冲命令
         * @param commandList 拷贝命令队列
         * @param data
        */
        void DispatchCopyBuffer(const GraphicsCommandList& commandList, const void* data);
        /**
         * @brief 改变资源状态
         * @param commandList 图形命令列表
         * @param after 要改变的状态
        */
        //void DispatchTransitionStates(const GraphicsCommandList* commandList, D3D12_RESOURCE_STATES after);


    private:
        UniquePtr<UploadBuffer> m_UploadBuffer{};

        void Finalize() override
        {
            // Resource必须创建以后才可以完成初始化
            ASSERT(m_Resource != nullptr);
            // 仅缓冲资源可以获取 GPU 虚拟地址
            m_GpuVirtualAddress = m_Resource->GetGPUVirtualAddress();
        }

    };
}