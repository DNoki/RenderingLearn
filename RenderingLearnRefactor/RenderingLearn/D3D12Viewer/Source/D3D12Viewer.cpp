#include "pch.h"
#include "D3D12Viewer.h"

#include "System/WindowApplication.h"

using namespace D3D12Viewer;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ TCHAR* /*lpCmdLine*/, _In_ int nCmdShow)
{
#if DEBUG
    std::cout.imbue(std::locale(".utf8"));
    std::wcout.imbue(std::locale(".utf8"));
    system("chcp 65001");
#endif

    WindowApplication::GetInstance().Run(hInstance, nCmdShow);

    //system("pause");



    return 0;
}


namespace D3D12Viewer
{
    using namespace D3D12Core;

    enum class RenderPassAccessType
    {
        RenderTarget,
        Depth,
        Stencil,
    };

    class RenderPass
    {
    public:
        RenderPass() = default;

        void Create()
        {
            m_CommandList = CommandListPool::Request<GraphicsCommandList>();

            m_Flags = D3D12_RENDER_PASS_FLAG_NONE;

            m_RTBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
            auto& rtClearValue = m_RTBeginningAccess.Clear.ClearValue;
            rtClearValue.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            memcpy(&rtClearValue.Color, Color(0.0f, 0.5f, 0.75f), sizeof(Color));

            m_RTEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
            m_RTEndingAccess.Resolve = {};

            m_DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
            auto& rtClearValue = m_RTBeginningAccess.Clear.ClearValue;
            rtClearValue.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            memcpy(&rtClearValue.Color, Color(0.0f, 0.5f, 0.75f), sizeof(Color));

            m_RTEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
            m_RTEndingAccess.Resolve = {};
        }
        void SetRenderTargets(std::initializer_list< IRenderTarget*> renderTargets)
        {
            for (auto rt = renderTargets.begin(); rt != renderTargets.end(); ++rt)
            {
                if (!*rt)
                    continue;

                if ((*rt)->GetType() == RenderTargetType::Color)
                {
                    m_RenderTargets.push_back(*rt);
                }
                else if ((*rt)->GetType() == RenderTargetType::DepthStencil)
                {
                    ASSERT(!m_DepthStencil);
                    m_DepthStencil = *rt;
                }
            }
            ASSERT(0 < m_RenderTargets.size());
        }

        void SetBeginningAccess(RenderPassAccessType accessType, D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE type, Color color = Color(0.0f, 0.5f, 0.75f))
        {
            switch (accessType)
            {
            case RenderPassAccessType::RenderTarget:
            {
                m_RTBeginningAccess.Type = type;
                m_RTBeginningAccess.Clear.ClearValue.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                reinterpret_cast<Color&> (m_RTBeginningAccess.Clear.ClearValue.Color) = color;
            }
            break;
            case RenderPassAccessType::Depth:
            {
                m_DepthBeginningAccess.Type = type;
                m_DepthBeginningAccess.Clear.ClearValue.Format = DXGI_FORMAT_r24_FLOAT;
                m_DepthBeginningAccess.Clear.ClearValue.DepthStencil.Depth = 0.0f;
                m_DepthBeginningAccess.Clear.ClearValue.DepthStencil.Depth = ;
            }
            break;
            case RenderPassAccessType::Stencil:
            {

            }
            break;
            default:;
            }
        }
        //void SetRenderTargetEndingAccess(RenderPassAccessType accessType, D3D12_RENDER_PASS_ENDING_ACCESS_TYPE type, )
        //{

        //}

        void BeginRenderPass()
        {
            // 重置渲染队列
            m_CommandList->Reset();

            // 设置渲染目标
            Vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> rtvDescArray{};
            for (auto rt = m_RenderTargets.begin(); rt != m_RenderTargets.end(); ++rt)
            {
                m_CommandList->ResourceBarrier(*rt, D3D12_RESOURCE_STATE_RENDER_TARGET);

                rtvDescArray.push_back(D3D12_RENDER_PASS_RENDER_TARGET_DESC());
                auto& rtvDesc = rtvDescArray.back();
                rtvDesc.cpuDescriptor = (*rt)->GetDescriptorHandle();
                rtvDesc.BeginningAccess = m_RTBeginningAccess;
                rtvDesc.EndingAccess = m_RTEndingAccess;
            }

            // 深度模板
            D3D12_RENDER_PASS_DEPTH_STENCIL_DESC dsvDesc{};
            if (m_DepthStencil)
            {
                m_CommandList->ResourceBarrier(m_DepthStencil, D3D12_RESOURCE_STATE_DEPTH_WRITE);

                dsvDesc.cpuDescriptor = m_DepthStencil->GetDescriptorHandle();
                dsvDesc.DepthBeginningAccess = m_DepthBeginningAccess;
                dsvDesc.DepthEndingAccess = m_DepthEndingAccess;
                dsvDesc.StencilBeginningAccess = m_StencilBeginningAccess;
                dsvDesc.StencilEndingAccess = m_StencilEndingAccess;
            }

            // 开始渲染队列
            m_CommandList->GetD3D12CommandList()->BeginRenderPass(
                m_RenderTargets.size(),
                rtvDescArray.data(),
                m_DepthStencil ? &dsvDesc : nullptr,
                m_Flags);
        }

    private:
        GraphicsCommandList* m_CommandList;

        Vector<IRenderTarget*> m_RenderTargets{};
        IRenderTarget* m_DepthStencil{};

        D3D12_RENDER_PASS_BEGINNING_ACCESS m_RTBeginningAccess{};
        D3D12_RENDER_PASS_ENDING_ACCESS m_RTEndingAccess{};
        D3D12_RENDER_PASS_BEGINNING_ACCESS m_DepthBeginningAccess{};
        D3D12_RENDER_PASS_ENDING_ACCESS m_DepthEndingAccess{};
        D3D12_RENDER_PASS_BEGINNING_ACCESS m_StencilBeginningAccess{};
        D3D12_RENDER_PASS_ENDING_ACCESS m_StencilEndingAccess{};

        D3D12_RENDER_PASS_FLAGS m_Flags{};
    };
}
