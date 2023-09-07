#pragma once

#include "​GraphicsResource/IRenderTexture.h"

namespace D3D12Viewer
{
    /**
     * @brief 渲染呈现贴图
    */
    class RenderTexture : public D3D12Core::IRenderTarget, public IGameResource
    {
    public:
        RenderTexture() = default;
        ~RenderTexture() override = 0 {};
        RenderTexture(const RenderTexture& buffer) = delete;
        RenderTexture(RenderTexture&& buffer) = default;
        RenderTexture& operator = (const RenderTexture& buffer) = delete;
        RenderTexture& operator = (RenderTexture&& buffer) = default;

        //void DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, Color optColor = Color::Black);
        void Create(D3D12Core::RenderTargetType type, DXGI_FORMAT format, UINT64 width, UINT height, D3D12_CLEAR_VALUE clearValue);


        //virtual std::wstring GetName() const override { return Texture::GetName(); }
        virtual void SetName(const std::wstring& name) override
        {
            m_Name = std::wstring(name);
            if (m_Resource)
                D3D12Core::GraphicsContext::SetDebugName(m_Resource.get(), FORMAT(TEXT("%s (RenderTarget)"), m_Name.c_str()));
        }

    private:
        void InitDesc(DXGI_FORMAT format, UINT64 width, UINT height, Color optColor);
        void InitDescriptor();

    };

#if false
    /**
     * @brief 深度模板贴图
    */
    class DepthStencilTexture : public D3D12Core::IRenderTarget, public IGameResource
    {
    public:
        DepthStencilTexture() = default;
        ~DepthStencilTexture() override = 0 {};
        DepthStencilTexture(const DepthStencilTexture& buffer) = delete;
        DepthStencilTexture(DepthStencilTexture&& buffer) = default;
        DepthStencilTexture& operator = (const DepthStencilTexture& buffer) = delete;
        DepthStencilTexture& operator = (DepthStencilTexture&& buffer) = default;

        //void DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth = 0.0f, UINT8 optStencil = 0);
        //void PlacedCreate(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth = 0.0f, UINT8 optStencil = 0);
        void Create(D3D12Core::RenderTargetType type, DXGI_FORMAT format, UINT64 width, UINT height, D3D12_CLEAR_VALUE clearValue);
        
        /**
         * @brief 获取 DSV 描述
         * @return
        */
        const D3D12_DEPTH_STENCIL_VIEW_DESC* GetDsvDesc() const { return m_DsvDesc.get(); }
        /**
         * @brief 获取清空值
         * @return
        */
        const D3D12_CLEAR_VALUE* GetClearValue() const { return &m_ClearValue; }

        virtual std::wstring GetName() const override { return Texture::GetName(); }
        virtual void SetName(const std::wstring& name) override
        {
            m_Name = std::wstring(name);
            if (m_Resource) SET_DEBUGNAME(m_Resource.get(), Application::Format(_T("%s (DepthStencil)"), m_Name.c_str()));
        }

    private:
        Graphics::DescriptorHandle m_SDV{};
        /**
         * @brief 深度模板视图描述
        */
        std::unique_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC> m_DsvDesc{};

        D3D12_CLEAR_VALUE m_ClearValue{};

        void InitDesc(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth, UINT8 optStencil);
        void InitDescriptor();
        DXGI_FORMAT GetSrvFormat(DXGI_FORMAT dsFormat) const;

    };
#endif

}