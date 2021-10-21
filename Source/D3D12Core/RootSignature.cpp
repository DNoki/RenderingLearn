#include "pch.h"

#include "GraphicsCore.h"

#include "RootSignature.h"

// --------------------------------------------------------------------------
/*
    根签名
    https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/root-signatures-overview

    “根签名”类似于 API 函数签名，它确定着色器应期望的数据类型，但不定义实际的内存或数据。
*/
// --------------------------------------------------------------------------

using namespace winrt;

RootSignature::RootSignature(UINT numRootParams, UINT numStaticSamplers)
{
    Reset(numRootParams, numStaticSamplers);
}

void RootSignature::Reset(UINT numRootParams, UINT numStaticSamplers)
{
    m_IsFinalized = false;
    m_NumRootParams = numRootParams;
    m_NumStaticSamplers = numStaticSamplers;
    m_RootSignature = nullptr;

    m_ParamArray.clear();
    for (UINT i = 0; i < m_NumRootParams; i++)
    {
        m_ParamArray.push_back(D3D12_ROOT_PARAMETER{});
    }

    m_SamplerArray.clear();
    for (UINT i = 0; i < m_NumStaticSamplers; i++)
    {
        m_SamplerArray.push_back(D3D12_STATIC_SAMPLER_DESC{});
    }
}

void RootSignature::Finalize(D3D12_ROOT_SIGNATURE_FLAGS Flags)
{
    if (m_IsFinalized) return;
    m_IsFinalized = true;
    ASSERT(m_RootSignature == nullptr);

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Init(
        m_NumRootParams,
        m_NumRootParams > 0 ? m_ParamArray.data() : nullptr,
        m_NumStaticSamplers,
        m_NumStaticSamplers > 0 ? m_SamplerArray.data() : nullptr,
        Flags);

    com_ptr<ID3DBlob> pSignature;
    com_ptr<ID3DBlob> pErrorSignature;

    CHECK_HRESULT(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.put(), pErrorSignature.put()));
    CHECK_HRESULT(Graphics::g_Device->CreateRootSignature(1, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.put())));
}
