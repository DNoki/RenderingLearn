#pragma once

#include "DescriptorHandle.h"
#include "IGraphicsResource.h"

namespace D3D12Core
{
    // TODO 重命名为ShaderResource （SRV）
    class ITexture : public IGraphicsResource
    {
    public:
        ITexture() = default;
        ~ITexture() override = 0 {};
        ITexture(const ITexture& buffer) = delete;
        ITexture(ITexture&& buffer) = default;
        ITexture& operator = (const ITexture& buffer) = delete;
        ITexture& operator = (ITexture&& buffer) = default;


        DXGI_FORMAT GetFormat() const { return m_ResourceDesc.Format; }
        UINT GetWidth() const { return static_cast<UINT>(m_ResourceDesc.Width); }
        UINT GetHeight() const { return static_cast<UINT>(m_ResourceDesc.Height); }

        const DescriptorHandle& GetSRV() const { return m_SRV; }

        /**
         * @brief 改变资源状态
         * @param commandList 图形命令列表
         * @param after 要改变的状态
        */
        //void DispatchTransitionStates(const class GraphicsCommandList* commandList, D3D12_RESOURCE_STATES after);

    protected:
        DescriptorHandle m_SRV;

    };
}
