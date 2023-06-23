#pragma once

namespace D3D12Core
{
    /**
     * @brief 根签名
    */
    class RootSignature
    {
    public:
        RootSignature() = default;

        /**
         * @brief 重置根签名
         * @param numRootParams
         * @param numStaticSamplers
        */
        void Reset(uint32 numRootParams, uint32 numStaticSamplers = 0);

        /**
         * @brief 最终确认
         * @param Flags
        */
        void Finalize(D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

        ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.get(); }
        CD3DX12_STATIC_SAMPLER_DESC& GetStaticSamplerDesc(uint32 index) { return m_SamplerArray[index]; }

        /*TODO const*/ CD3DX12_ROOT_PARAMETER1& operator [](uint32 entryIndex)
        {
            ASSERT(entryIndex < m_NumRootParams);
            return m_ParamArray[entryIndex];
        }

    private:
        bool m_IsFinalized{};
        ComPtr<ID3D12RootSignature> m_RootSignature{};

        uint32 m_NumRootParams{};
        uint32 m_NumStaticSamplers{};
        Vector<CD3DX12_ROOT_PARAMETER1> m_ParamArray{};
        Vector<CD3DX12_STATIC_SAMPLER_DESC> m_SamplerArray{};
    };
}
