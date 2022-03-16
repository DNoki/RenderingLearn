#pragma once

#include "GraphicsResource.h"
#include "IGameResource.h"

namespace Graphics
{
    class DescriptorHandle;
    class SwapChain;
}

namespace Resources
{
#if 0
    enum class RenderTextureType
    {
        RenderTarget,
        DepthStencil,
    };

    /**
     * @brief 渲染贴图（RTV、DSV）
    */
    class RenderTexture : public Graphics::Texture, public Game::IGameResource
    {
    public:
        RenderTexture() = default;
        virtual ~RenderTexture() override = default;
        RenderTexture(const RenderTexture&) = delete;
        RenderTexture(RenderTexture&&) = default;

        inline RenderTexture& operator = (const RenderTexture&) = delete;
        inline RenderTexture& operator = (RenderTexture&&) = default;

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
         * @brief 获取清空值
         * @return
        */
        inline const D3D12_CLEAR_VALUE* GetClearValue() const { return &m_ClearValue; }

        /**
         * @brief 以注册方式创建渲染贴图
         * @param type 渲染贴图类型
         * @param format 格式
         * @param width 宽度
         * @param height 高度
         * @param clearValue 清空值
        */
        void DirectCreate(RenderTextureType type, DXGI_FORMAT format, UINT64 width, UINT height, const D3D12_CLEAR_VALUE* clearValue = nullptr);
        /**
         * @brief 以放置方式创建渲染贴图
         * @param type 渲染贴图类型
         * @param format 格式
         * @param width 宽度
         * @param height 高度
         * @param clearValue 清空值
        */
        void PlacedCreate(RenderTextureType type, DXGI_FORMAT format, UINT64 width, UINT height, const D3D12_CLEAR_VALUE* clearValue = nullptr);

#if 0
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

        void DirectCreateRTV(DXGI_FORMAT format, UINT64 width, UINT height, Color clearColor = Color());
        void PlacedCreateRTV(DXGI_FORMAT format, UINT64 width, UINT height, Color clearColor = Color()); // TODO
#endif
        /**
         * @brief 从交换链获取 RTV 缓冲
         * @param swapChain
         * @param index
        */
        void CreateRtvFromSwapChain(const Graphics::SwapChain& swapChain, UINT index);

        inline virtual std::wstring GetName() const override { return Texture::GetName(); }
        inline virtual void SetName(const std::wstring& name) override
        {
            m_Name = std::wstring(name);
            if (m_Resource) SET_DEBUGNAME(m_Resource.get(), Application::Format(_T("%s (RenderTexture)"), m_Name.c_str()));
        }

    protected:
        RenderTextureType m_Type{};

        /**
         * @brief 渲染目标视图描述
        */
        std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC> m_RtvDesc{};
        /**
         * @brief 深度模板视图描述
        */
        std::unique_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC> m_DsvDesc{};

        D3D12_CLEAR_VALUE m_ClearValue{};

    };
#endif

    /**
     * @brief 渲染呈现贴图
    */
    class RenderTargetTexture : public Graphics::Texture, public Game::IGameResource
    {
    public:
        RenderTargetTexture() = default;

        void DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, Color optColor = Color());
        void PlacedCreate(DXGI_FORMAT format, UINT64 width, UINT height, Color optColor = Color());
        /**
         * @brief 从交换链获取 RTV 缓冲
         * @param swapChain
         * @param index
        */
        void CreateFromSwapChain(const Graphics::SwapChain& swapChain, UINT index);

        inline const Graphics::DescriptorHandle& GetRTV() const { return m_RTV; }
        /**
         * @brief 获取 RTV 描述
         * @return
        */
        inline const D3D12_RENDER_TARGET_VIEW_DESC* GetRtvDesc() const { return m_RtvDesc.get(); }
        /**
         * @brief 获取清空值
         * @return
        */
        inline const D3D12_CLEAR_VALUE* GetClearValue() const { return &m_ClearValue; }

        inline virtual std::wstring GetName() const override { return Texture::GetName(); }
        inline virtual void SetName(const std::wstring& name) override
        {
            m_Name = std::wstring(name);
            if (m_Resource) SET_DEBUGNAME(m_Resource.get(), Application::Format(_T("%s (RenderTarget)"), m_Name.c_str()));
        }

    private:
        Graphics::DescriptorHandle m_RTV{};
        /**
         * @brief 渲染目标视图描述
        */
        std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC> m_RtvDesc{};

        D3D12_CLEAR_VALUE m_ClearValue{};

        void InitDesc(DXGI_FORMAT format, UINT64 width, UINT height, Color optColor);
        void InitDescriptor();

    };

    /**
     * @brief 深度模板贴图
    */
    class DepthStencilTexture : public Graphics::Texture, public Game::IGameResource
    {
    public:
        DepthStencilTexture() = default;

        void DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth = 0.0f, UINT8 optStencil = 0);
        void PlacedCreate(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth = 0.0f, UINT8 optStencil = 0);

        inline const Graphics::DescriptorHandle& GetDSV() const { return m_SDV; }
        /**
         * @brief 获取 DSV 描述
         * @return
        */
        inline const D3D12_DEPTH_STENCIL_VIEW_DESC* GetDsvDesc() const { return m_DsvDesc.get(); }
        /**
         * @brief 获取清空值
         * @return
        */
        inline const D3D12_CLEAR_VALUE* GetClearValue() const { return &m_ClearValue; }

        inline virtual std::wstring GetName() const override { return Texture::GetName(); }
        inline virtual void SetName(const std::wstring& name) override
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
}