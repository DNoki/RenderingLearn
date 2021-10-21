#pragma once

/**
 * @brief 根签名
*/
class RootSignature
{
public:
    RootSignature(UINT numRootParams = 0, UINT numStaticSamplers = 0);

    /**
     * @brief 重置根签名
     * @param numRootParams 
     * @param numStaticSamplers 
    */
    void Reset(UINT numRootParams, UINT numStaticSamplers = 0);
    /**
     * @brief 最终确认
     * @param Flags 
    */
    void Finalize(D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

    inline ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.get(); }

    /*TODO const*/ D3D12_ROOT_PARAMETER& operator [](UINT entryIndex)
    {
        ASSERT(entryIndex < m_NumRootParams);
        return m_ParamArray[entryIndex];
    }

private:
    bool m_IsFinalized;
    winrt::com_ptr<ID3D12RootSignature> m_RootSignature;

    UINT m_NumRootParams;
    UINT m_NumStaticSamplers;
    std::vector<D3D12_ROOT_PARAMETER> m_ParamArray;
    std::vector<D3D12_STATIC_SAMPLER_DESC> m_SamplerArray;
};
