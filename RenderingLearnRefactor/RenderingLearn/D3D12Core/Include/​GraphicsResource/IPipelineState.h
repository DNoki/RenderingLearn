#pragma once

namespace D3D12Core
{
    /**
     * @brief 管线状态
    */
    class IPipelineState
    {
    public:
        IPipelineState() = default;
        virtual ~IPipelineState() = 0 {}
        IPipelineState(const IBufferResource& buffer) = delete;
        IPipelineState(IPipelineState&& buffer) = default;
        IPipelineState& operator = (const IPipelineState& buffer) = delete;
        IPipelineState& operator = (IPipelineState&& buffer) = default;

        /**
         * @brief 获取D3D12管线状态对象
         * @return
        */
        ID3D12PipelineState* GetD3D12PSO() const { return m_PSO; }

    protected:
        ID3D12PipelineState* m_PSO{};  // 管线状态对象
        const RootSignature* m_RootSignature{};

        virtual void Finalize() = 0;
    };
}
