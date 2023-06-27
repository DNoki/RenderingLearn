#include "pch.h"
#include "Rendering/RenderPass.h"

using namespace D3D12Core;
using namespace D3D12Viewer;

void RenderPass::Create()
{
    m_CommandList = CommandListPool::Request<GraphicsCommandList>();

    m_Flags = D3D12_RENDER_PASS_FLAG_NONE;
}

void RenderPass::SetRenderTargets(std::initializer_list<IRenderTarget*> renderTargets)
{
    for (auto rt = renderTargets.begin(); rt != renderTargets.end(); ++rt)
    {
        if (!*rt)
            continue;

        if ((*rt)->GetType() == RenderTargetType::Color)
        {
            m_RenderTargets.push_back(*rt);
            m_RTDescArray.push_back(D3D12_RENDER_PASS_RENDER_TARGET_DESC());
            m_RTDescArray.back().cpuDescriptor = (*rt)->GetDescriptorHandle();
        }
        else if ((*rt)->GetType() == RenderTargetType::DepthStencil)
        {
            ASSERT(!m_DepthStencil);
            m_DepthStencil = *rt;
            ZeroMemory(&m_DSDesc, sizeof m_DSDesc);
            m_DSDesc.cpuDescriptor = m_DepthStencil->GetDescriptorHandle();
        }
    }
    ASSERT(0 < m_RenderTargets.size());
}

void RenderPass::SetRTBeginningAccess(int32 number, D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE type, DXGI_FORMAT clearFormat,
    const Color& clearColor)
{
    ASSERT(number < m_RTDescArray.size());
    m_RTDescArray[number].BeginningAccess.Type = type;
    m_RTDescArray[number].BeginningAccess.Clear.ClearValue.Format = clearFormat;
    reinterpret_cast<Color&>(m_RTDescArray[number].BeginningAccess.Clear.ClearValue.Color) = clearColor;
}

void RenderPass::SetRTEndingAccess(int32 number, D3D12_RENDER_PASS_ENDING_ACCESS_TYPE type,
    D3D12_RENDER_PASS_ENDING_ACCESS_RESOLVE_PARAMETERS resolve)
{
    m_RTDescArray[number].EndingAccess.Type = type;
    m_RTDescArray[number].EndingAccess.Resolve = resolve;
}

void RenderPass::SetViewports(uint32 topLeftX, uint32 topLeftY, uint32 width, uint32 height) const
{
    ASSERT(m_CommandList);
    m_CommandList->RSSetViewports(
        static_cast<float>(topLeftX),
        static_cast<float>(topLeftY),
        static_cast<float>(width),
        static_cast<float>(height));
    m_CommandList->RSSetScissorRects(topLeftX, topLeftY, width, height);
}

void RenderPass::BeginRenderPass()
{
    // 重置渲染队列
    m_CommandList->Reset();

    // 渲染目标资源状态转换
    ASSERT(m_RenderTargets.size() == m_RTDescArray.size());
    for (size_t i = 0; i < m_RenderTargets.size(); ++i)
    {
        m_CommandList->ResourceBarrier(m_RenderTargets[i], D3D12_RESOURCE_STATE_RENDER_TARGET);
    }

    // 深度模板资源状态转换
    if (m_DepthStencil)
    {
        m_CommandList->ResourceBarrier(m_DepthStencil, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    }

    // 开始渲染队列
    m_CommandList->GetD3D12CommandList()->BeginRenderPass(
        static_cast<uint32>(m_RenderTargets.size()),
        m_RTDescArray.data(),
        m_DepthStencil ? &m_DSDesc : nullptr,
        m_Flags);
}

void RenderPass::EndRenderPass()
{
    m_CommandList->GetD3D12CommandList()->EndRenderPass();

    // 渲染目标资源状态转换
    for (size_t i = 0; i < m_RenderTargets.size(); ++i)
    {
        m_CommandList->ResourceBarrier(m_RenderTargets[i], D3D12_RESOURCE_STATE_COMMON);
    }

    // 深度模板资源状态转换
    if (m_DepthStencil)
    {
        m_CommandList->ResourceBarrier(m_DepthStencil, D3D12_RESOURCE_STATE_DEPTH_READ);
    }
}

void RenderPass::DrawCall(const Mesh* mesh, const Material* material)
{
    ASSERT(mesh && material);

    m_CommandList->IASetPrimitiveTopology(mesh->GetPrimitiveTopology());

    Vector<DXGI_FORMAT> rtvFormats{};
    for (const auto rtv : m_RenderTargets)
    {
        rtvFormats.push_back(rtv->GetFormat());
    }

    material->GetPipelineState()->SetRenderTargetFormats(
        static_cast<uint32>(rtvFormats.size()),
        rtvFormats.data(),
        m_DepthStencil ? m_DepthStencil->GetFormat() : DXGI_FORMAT_UNKNOWN);
    material->GetPipelineState()->Finalize();

    //m_CommandList->SetGraphicsRootSignature(material->GetShader()->GetRootSignature());
    //m_CommandList->SetPipelineState(material->GetPipelineState());

    material->DispatchBindMaterial(m_CommandList, false);

    // TODO
    m_CommandList->IASetVertexBuffers(0, mesh->m_VBVs[static_cast<int32>(VertexSemantic::Position)].get());
    m_CommandList->IASetVertexBuffers(4, mesh->m_VBVs[static_cast<int32>(VertexSemantic::Texcoord)].get());
    m_CommandList->IASetIndexBuffer(mesh->m_IBV.get());

    m_CommandList->DrawIndexedInstanced(static_cast<uint32>(mesh->m_Indices.size()));

}
