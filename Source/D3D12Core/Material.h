#pragma once

namespace Graphics
{
    class DescriptorHandle;
    class DescriptorHeap;
    class Texture;
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
        /**
         * @brief 执行绑定材质
         * @param commandList 命令列表
         * @param isOnlyBindDescriptorHeap 仅绑定描述符堆
        */
        void ExecuteBindMaterial(const Graphics::CommandList* commandList, bool isOnlyBindDescriptorHeap) const;

        /**
         * @brief 绑定常量缓冲
         * @param slot
         * @param buffer
        */
        void BindBuffer(int slot, const Graphics::IBufferResource& buffer);
        /**
         * @brief 绑定贴图
         * @param slot
         * @param texture
        */
        void BindTexture(int slot, const Graphics::Texture& texture);
        /**
         * @brief 绑定采样器
         * @param sampler
        */
        inline void BindSampler(const Graphics::DescriptorHandle* sampler)
        {
            m_SamplerDescriptorHandle = sampler;
            m_Version++;
        }

        /**
         * @brief 设置渲染目标
         * @param mrt
        */
        void SetRenderTargets(const Graphics::MultiRenderTargets* mrt);


        // --------------------------------------------------------------------------
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

        /**
         * @brief 设置应用深度测试
         * @param enable
        */
        void SetDepthEnable(bool enable);
        bool GetDepthEnable() const;
        /**
         * @brief 设置允许写入深度值
         * @param enable
        */
        void SetDepthWriteEnable(bool enable);
        bool GetDepthWriteEnable() const;
        /**
         * @brief 设置深度测试比较模式
         * @param func
        */
        void SetDepthFunc(D3D12_COMPARISON_FUNC func);
        D3D12_COMPARISON_FUNC GetDepthFunc() const;

    private:
        const Shader* m_Shader; // 材质使用的着色器

        std::unique_ptr<Graphics::GraphicsPipelineState> m_PipelineState;   // 材质定义的管线状态
        std::unique_ptr<Graphics::DescriptorHeap> m_ResourceDescHeap;       // 材质绑定的资源 // TODO 考虑存储资源对象，对资源对象添加版本号管理
        const Graphics::DescriptorHandle* m_SamplerDescriptorHandle; // TODO 考虑多个采样器存在的情况

        UINT64 m_Version; // 每当更新绑定的资源时，版本号加1

    };

}