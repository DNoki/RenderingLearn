﻿#pragma once

#include "GraphicsResource.h"

namespace Graphics
{
    class UploadBuffer;
    class CommandList;

    /**
     * @brief GPU 缓冲
    */
    class GraphicsBuffer : public GraphicsResource, public IBufferResource
    {
    public:
        // --------------------------------------------------------------------------
        GraphicsBuffer();
        virtual ~GraphicsBuffer() override = default;
        GraphicsBuffer(const GraphicsBuffer& buffer) = delete;
        GraphicsBuffer(GraphicsBuffer&& buffer) = default;

        inline GraphicsBuffer& operator = (const GraphicsBuffer & buffer) = delete;
        inline GraphicsBuffer& operator = (GraphicsBuffer && buffer) = default;

        // --------------------------------------------------------------------------
        virtual inline UINT64  GetBufferSize() const override { return m_ResourceDesc.Width; }
        virtual inline D3D12_GPU_VIRTUAL_ADDRESS  GetGpuVirtualAddress() const override { return m_GpuVirtualAddress; }

        inline virtual void SetName(const std::wstring& name) override
        {
            m_Name = std::wstring(name);
            if (m_Resource) SET_DEBUGNAME(m_Resource.get(), Application::Format(_T("%s (GraphicsBuffer)"), m_Name.c_str()));
        }

        // --------------------------------------------------------------------------
        /**
         * @brief 创建指定大小的默认堆
         * @param size
        */
        virtual void DirectCreate(UINT64 size) override;
        /**
         * @brief 使用定位方式创建一个默认堆
         * @param size
         * @param pPlacedHeap
        */
        virtual void PlacedCreate(UINT64 size) override;

        /**
         * @brief 添加拷贝缓冲命令
         * @param commandList 拷贝命令队列
         * @param data
        */
        void DispatchCopyBuffer(const CommandList& commandList, const void* data);
        /**
         * @brief 改变资源状态
         * @param commandList 图形命令列表
         * @param after 要改变的状态
        */
        void DispatchTransitionStates(const CommandList* commandList, D3D12_RESOURCE_STATES after);


    private:
        std::unique_ptr<UploadBuffer> m_UploadBuffer;

        inline virtual void Finalize() override
        {
            // Resource必须创建以后才可以完成初始化
            ASSERT(m_Resource != nullptr);
            // 仅缓冲资源可以获取 GPU 虚拟地址
            m_GpuVirtualAddress = m_Resource->GetGPUVirtualAddress();
        }

    };
}