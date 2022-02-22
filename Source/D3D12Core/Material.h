#pragma once
namespace Graphics
{
    class GraphicsPipelineState;
    class CommandList;
}

namespace Game
{
    class Shader;

    class Material
    {
    public:
        Material() = default;

        void Create(const Shader* shader);

        inline const Shader* GetShader() const { return m_Shader; }
        inline const Graphics::GraphicsPipelineState* GetPipelineState() const { return m_PipelineState.get(); }
        inline const Graphics::DescriptorHeap* GetResourceDescHeap() const { return m_ResourceDescHeap.get(); }

        inline bool IsChanged() const { return m_IsChanged; }
        void RefleshPipelineState();

        void ExecuteBindDescriptorHeap(const Graphics::CommandList* commandList) const;
        void ExecuteBindMaterial(const Graphics::CommandList* commandList) const;


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

        bool m_IsChanged;
        bool m_Version; // TODO

    };

}