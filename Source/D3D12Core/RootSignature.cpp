#include "pch.h"

#include "GraphicsCore.h"

#include "RootSignature.h"

// --------------------------------------------------------------------------
/*
    根签名
    https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/root-signatures-overview

    “根签名”类似于 API 函数签名，它确定着色器应期望的数据类型，但不定义实际的内存或数据。

    GPU渲染需要数据（比如：纹理、网格、索引、世界变换矩阵、动画矩阵调色板、采样器等等），而这些数据必须是GPU能够理解的专有格式，这些数据始终是CPU负责加载并传递给GPU的，此时就需要提前告诉GPU要传递那些数据，以及传递到什么地方，传递多少个等信息，而这些信息最终就被封装在了根签名里。
    具体的讲，根签名实际是描述了常量（类似默认参数）、常量缓冲区（CBV）、资源（SRV，纹理）、无序访问缓冲（UAV，随机读写缓冲）、采样器（Sample）等的寄存器（Register）存储规划的一个结构体。同时它还描述了每种资源针对每个阶段Shader的可见性。

    在命令列表的开头，根签名是未定义的。 图形着色器具有与计算着色器不同的根签名，每个签名单独分配在命令列表中。
*/
// --------------------------------------------------------------------------

using namespace winrt;
using namespace Graphics;

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
        m_ParamArray.push_back(CD3DX12_ROOT_PARAMETER1{});
    }

    m_SamplerArray.clear();
    for (UINT i = 0; i < m_NumStaticSamplers; i++)
    {
        m_SamplerArray.push_back(CD3DX12_STATIC_SAMPLER_DESC{});
    }
}

void RootSignature::Finalize(D3D12_ROOT_SIGNATURE_FLAGS Flags)
{
    if (m_IsFinalized) return;
    m_IsFinalized = true;
    ASSERT(m_RootSignature == nullptr);

    // 检测是否支持 V1.1 版本的根签名
    D3D12_FEATURE_DATA_ROOT_SIGNATURE rsFeatureData{};
    rsFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(g_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rsFeatureData, sizeof(rsFeatureData))))
        rsFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

    // 初始化根签名描述
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC verRootSignDesc{};
    verRootSignDesc.Init_1_1(
        m_NumRootParams, m_NumRootParams > 0 ? m_ParamArray.data() : nullptr,
        m_NumStaticSamplers, m_NumStaticSamplers > 0 ? m_SamplerArray.data() : nullptr,
        Flags);

    com_ptr<ID3DBlob> pSignature;
    com_ptr<ID3DBlob> pErrorSignature;

    CHECK_HRESULT(D3DX12SerializeVersionedRootSignature(
        &verRootSignDesc,
        rsFeatureData.HighestVersion,
        pSignature.put(),
        pErrorSignature.put()
    ));
    CHECK_HRESULT(g_Device->CreateRootSignature(1, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.put())));
}
