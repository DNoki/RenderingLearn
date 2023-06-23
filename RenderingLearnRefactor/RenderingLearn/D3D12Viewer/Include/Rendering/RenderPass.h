#pragma once

namespace D3D12Viewer
{
    class RenderPass
    {
    public:
        RenderPass() = default;

        void Create();
        void SetRenderTargets(std::initializer_list<D3D12Core::IRenderTarget*> renderTargets);

        void SetRTBeginningAccess(int32 number, D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE type, DXGI_FORMAT clearFormat = DXGI_FORMAT_UNKNOWN, const Color& clearColor = Color());
        void SetRTEndingAccess(int32 number, D3D12_RENDER_PASS_ENDING_ACCESS_TYPE type, D3D12_RENDER_PASS_ENDING_ACCESS_RESOLVE_PARAMETERS resolve = {});

        void SetViewports(uint32 topLeftX, uint32 topLeftY, uint32 width, uint32 height) const;

        void BeginRenderPass();
        void EndRenderPass();

        void DrawCall(const Mesh* mesh, const Material* material);

    public:
        D3D12Core::GraphicsCommandList* m_CommandList{};

        Vector<D3D12Core::IRenderTarget*> m_RenderTargets{};
        D3D12Core::IRenderTarget* m_DepthStencil{};

        Vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> m_RTDescArray{};
        D3D12_RENDER_PASS_DEPTH_STENCIL_DESC m_DSDesc{};

        D3D12_RENDER_PASS_FLAGS m_Flags{};
    };
}
