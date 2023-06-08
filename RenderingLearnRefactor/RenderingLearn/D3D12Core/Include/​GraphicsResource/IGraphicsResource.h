#pragma once

#include "PlacedHeap.h"
//#include "DescriptorHandle.h"

namespace D3D12Core
{
    //class DescriptorHandle;
    //class PlacedHeap;
    //class CommandList;
    class PlacedResourceDesc;
    
    /**
     * @brief D3D12 资源抽象类
     * @return
    */
    class IGraphicsResource
    {
    public:
        // --------------------------------------------------------------------------
        IGraphicsResource() = default;
        virtual ~IGraphicsResource() = 0 {};

        IGraphicsResource(const IGraphicsResource& res) = delete;
        IGraphicsResource(IGraphicsResource&& res) = default;

        IGraphicsResource& operator = (const IGraphicsResource& res) = delete;
        IGraphicsResource& operator = (IGraphicsResource&& res) = default;

        // --------------------------------------------------------------------------
        const D3D12_RESOURCE_DESC& GetResourceDesc() const noexcept
        {
            return m_ResourceDesc;
        }
        ID3D12Resource1* GetD3D12Resource() const noexcept
        {
            return m_Resource.get();
        }
        D3D12_RESOURCE_STATES GetResourceStates() const noexcept { return m_ResourceStates; }
        void SetResourceStates(D3D12_RESOURCE_STATES states)
        {
            m_ResourceStates = states;
        }
        PlacedResourceDesc* GetPlacedResourceDesc() noexcept
        {
            return &m_PlacedResourceDesc;
        }

        ID3D12Resource1** PutD3D12Resource() noexcept
        {
            return m_Resource.put();
        }

        void DispatchTransitionStates(const GraphicsCommandList* commandList, D3D12_RESOURCE_STATES after);

        //inline virtual std::wstring GetName() const { return m_Name; }
        //inline virtual void SetName(const std::wstring& name)
        //{
        //    m_Name = std::wstring(name);
        //    if (m_Resource) SET_DEBUGNAME(m_Resource.get(), Application::Format(_T("%s (Resource)"), m_Name.c_str()));
        //}

    protected:
        // 资源对象
        // 封装了 CPU 和 GPU 读取和写入物理内存或堆的通用能力。
        ComPtr<ID3D12Resource1> m_Resource{};
        // 资源描述
        D3D12_RESOURCE_DESC m_ResourceDesc{};
        D3D12_RESOURCE_STATES m_ResourceStates{}; // 资源状态

        // GPU 内存中的虚拟地址
        // IBV、VBV 等直接调用资源类型时使用
        D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress{};

        // 放置资源描述
        PlacedResourceDesc m_PlacedResourceDesc{};

        // 资源名称
        //std::wstring m_Name{};

    };
}
