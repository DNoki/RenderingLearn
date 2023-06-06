#pragma once

#include "​GraphicsResource/DescriptorHandle.h"
#include "​GraphicsResource/IGraphicsResource.h"
//#include "IGraphicsResource.h"

namespace D3D12Core
{
    class Texture : public GraphicsResource
    {
    public:
        ~Texture() override = 0 {};

        DXGI_FORMAT GetFormat() const { return m_ResourceDesc.Format; }
        UINT GetWidth() const { return static_cast<UINT>(m_ResourceDesc.Width); }
        UINT GetHeight() const { return static_cast<UINT>(m_ResourceDesc.Height); }

        const DescriptorHandle& GetSRV() const { return m_SRV; }

        /**
         * @brief 改变资源状态
         * @param commandList 图形命令列表
         * @param after 要改变的状态
        */
        void DispatchTransitionStates(const class CommandList* commandList, D3D12_RESOURCE_STATES after);

    protected:
        DescriptorHandle m_SRV;

    };
}
