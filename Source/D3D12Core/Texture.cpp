﻿#include "pch.h"

#include "GraphicsCore.h"
#include "Display.h"

#include "Texture.h"



using namespace Graphics;


RenderTexture::RenderTexture() : m_RtvDesc(nullptr)
{
    //ZeroMemory(this, sizeof(*this));
}

void RenderTexture::Create(const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, const DescriptorHandle& pDescriptorHandle, UINT width, UINT height)
{
    ASSERT(m_Resource != nullptr);

    SetAddressOrDescriptor(D3D12_GPU_VIRTUAL_ADDRESS_NULL, pDescriptorHandle);

    auto pRtvDesc = pDesc ? new D3D12_RENDER_TARGET_VIEW_DESC(*pDesc) : nullptr;
    m_RtvDesc = std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC>(pRtvDesc);

    g_Device->CreateRenderTargetView(m_Resource.get(), m_RtvDesc.get(), m_DescriptorHandle);
    m_ResourceDesc = m_Resource->GetDesc();

    m_Width = width;
    m_Height = height;
}

void RenderTexture::CreateFromSwapChain(UINT index, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, const DescriptorHandle& pDescriptorHandle)
{
    ASSERT(m_Resource == nullptr);

    CHECK_HRESULT(g_SwapChain->GetBuffer(index, IID_PPV_ARGS(PutD3D12Resource())));

    Create(pDesc, pDescriptorHandle, Display::GetScreenWidth(), Display::GetScreenHeight());
}
