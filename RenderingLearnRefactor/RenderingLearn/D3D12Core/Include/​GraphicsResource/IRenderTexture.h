#pragma once

#include "ITexture.h"

namespace D3D12Core
{
    enum class RenderTargetType
    {
        Color,
        DepthStencil,
    };

    /**
     * 渲染目标
     */
    class IRenderTarget : public ITexture
    {
    public:
        IRenderTarget() = default;
        ~IRenderTarget() override = 0 {};
        IRenderTarget(const IRenderTarget& buffer) = delete;
        IRenderTarget(IRenderTarget&& buffer) = default;
        IRenderTarget& operator = (const IRenderTarget& buffer) = delete;
        IRenderTarget& operator = (IRenderTarget&& buffer) = default;


        RenderTargetType GetType() const { return m_Type; }
        const DescriptorHandle& GetRtvOrDsv() const { return m_RtvOrDsv; }
        /**
         * @brief 获取 RTV 描述
         * @return
        */
        const D3D12_RENDER_TARGET_VIEW_DESC* GetRtvDesc() const { return m_RtvDesc.get(); }
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

    protected:
        RenderTargetType m_Type{}; // 渲染目标类型
        DescriptorHandle m_RtvOrDsv{}; // RTV 或 DSV 描述符句柄
        UniquePtr<D3D12_RENDER_TARGET_VIEW_DESC> m_RtvDesc{};
        UniquePtr<D3D12_DEPTH_STENCIL_VIEW_DESC> m_DsvDesc{};

        D3D12_CLEAR_VALUE m_ClearValue{};

    };
}
