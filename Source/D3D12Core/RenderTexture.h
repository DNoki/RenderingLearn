#pragma once

#include "GraphicsResource.h"

namespace Graphics
{
    class SwapChain;

    /**
     * @brief 渲染贴图（RTV、DSV）
    */
    class RenderTexture : public Texture
    {
    public:
        RenderTexture() = default;
        virtual ~RenderTexture() override = default;
        RenderTexture(const RenderTexture & tex) = delete;
        RenderTexture(RenderTexture && tex) = default;

        inline RenderTexture& operator = (const RenderTexture & tex) = delete;
        inline RenderTexture& operator = (RenderTexture && tex) = default;

        inline DXGI_FORMAT GetFormat() const { return m_ResourceDesc.Format; }
        inline UINT GetWidth() const { return static_cast<UINT>(m_ResourceDesc.Width); }
        inline UINT GetHeight() const { return static_cast<UINT>(m_ResourceDesc.Height); }
        /**
         * @brief 获取 RTV 描述
         * @return
        */
        inline const D3D12_RENDER_TARGET_VIEW_DESC* GetRtvDesc() const { return m_RtvDesc.get(); }
        /**
         * @brief 获取 DSV 描述
         * @return
        */
        inline const D3D12_DEPTH_STENCIL_VIEW_DESC* GetDsvDesc() const { return m_DsvDesc.get(); }

        /**
         * @brief 注册方式创建深度模板视图
         * @param format
         * @param width
         * @param height
         * @param optDepth
         * @param optStencil
        */
        void DirectCreateDSV(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth = 1.0f, UINT8 optStencil = 0);
        void PlacedCreateDSV(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth = 1.0f, UINT8 optStencil = 0); // TODO

        /**
         * @brief 从交换链获取 RTV 缓冲
         * @param swapChain
         * @param index
        */
        void GetRtvFromSwapChain(const SwapChain& swapChain, UINT index);

        inline virtual void SetName(const std::wstring& name) override
        {
            m_Name = std::wstring(name);
            if (m_Resource) SET_DEBUGNAME(m_Resource.get(), Application::Format(_T("%s (RenderTexture)"), m_Name.c_str()));
        }

    protected:
        /**
         * @brief 渲染目标视图描述
        */
        std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC> m_RtvDesc;
        /**
         * @brief 深度模板视图描述
        */
        std::unique_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC> m_DsvDesc;

    };
}