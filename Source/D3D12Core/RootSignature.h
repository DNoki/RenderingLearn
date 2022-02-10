#pragma once

namespace Graphics
{
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
        inline CD3DX12_STATIC_SAMPLER_DESC& GetStaticSamplerDesc(UINT index) { return m_SamplerArray[index]; }

        /*TODO const*/ CD3DX12_ROOT_PARAMETER1& operator [](UINT entryIndex)
        {
            ASSERT(entryIndex < m_NumRootParams);
            return m_ParamArray[entryIndex];
        }

    private:
        bool m_IsFinalized;
        winrt::com_ptr<ID3D12RootSignature> m_RootSignature;

        UINT m_NumRootParams;
        UINT m_NumStaticSamplers;
        std::vector<CD3DX12_ROOT_PARAMETER1> m_ParamArray;
        std::vector<CD3DX12_STATIC_SAMPLER_DESC> m_SamplerArray;
    };
}
