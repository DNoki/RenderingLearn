#pragma once

namespace Graphics
{
    class DescriptorHandle;
    class DescriptorHeap;
    class IResource;
    class GraphicsPipelineState;
    class CommandList;
    class MultiRenderTargets;
}

namespace Game
{
    class Shader;

    class Material
    {
    public:
        // --------------------------------------------------------------------------
        Material() = default;

        void Create(const Shader* shader);

        // --------------------------------------------------------------------------
        inline const Shader* GetShader() const { return m_Shader; }
        inline Graphics::GraphicsPipelineState* GetPipelineState() const { return m_PipelineState.get(); }
        inline const Graphics::DescriptorHeap* GetResourceDescHeap() const { return m_ResourceDescHeap.get(); }

        inline UINT64 GetVersion() const { return m_Version; }

        // --------------------------------------------------------------------------
        void ExecuteBindMaterial(const Graphics::CommandList* commandList, bool isOnlyBindDescriptorHeap) const;

        void BindBuffer(int slot, const Graphics::IBufferResource& buffer);
        void BindTexture(int slot, const Graphics::ITexture& texture);
        inline void BindSampler(const Graphics::DescriptorHandle* sampler)
        {
            m_SamplerDescriptorHandle = sampler;
            m_Version++;
        }

        void SetRenderTargetsFormat(const Graphics::MultiRenderTargets* mrt);

        /**
         * @brief 设置填充模式
         * @param fillMode
        */
        void SetFillMode(D3D12_FILL_MODE fillMode);
        D3D12_FILL_MODE GetFillMode() const;
        /**
         * @brief 设置剔除模式
         * @param cullMode
        */
        void SetCullMode(D3D12_CULL_MODE cullMode);
        D3D12_CULL_MODE GetCullMode() const;
        /**
         * @brief 设置三角形绕序是否为逆时针
         * @param isCounterClockwise
        */
        void SetFrontCounterClockwise(bool isCounterClockwise);
        bool GetFrontCounterClockwise() const;

        void SetDepthEnable(bool enable);
        bool GetDepthEnable() const;
        void SetDepthWriteEnable(bool enable);
        bool GetDepthWriteEnable() const;
        void SetDepthFunc(D3D12_COMPARISON_FUNC func);
        D3D12_COMPARISON_FUNC GetDepthFunc() const;

    private:
        const Shader* m_Shader; // 材质使用的着色器

        std::unique_ptr<Graphics::GraphicsPipelineState> m_PipelineState;   // 材质定义的管线状态
        std::unique_ptr<Graphics::DescriptorHeap> m_ResourceDescHeap;       // 材质绑定的资源
        const Graphics::DescriptorHandle* m_SamplerDescriptorHandle; // TODO 考虑多个采样器存在的情况

        UINT64 m_Version;

    };

}