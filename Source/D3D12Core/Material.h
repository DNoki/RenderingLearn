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

        inline Graphics::GraphicsPipelineState* GetPipelineState() const { return m_PipelineState.get(); }
        inline const Graphics::DescriptorHeap* GetResourceDescHeap() const { return m_ResourceDescHeap.get(); }

        inline bool IsChanged() const { return m_IsChanged; }
        void RefleshPipelineState();

        void ExecuteBindDescriptorHeap(const Graphics::CommandList* commandList) const;
        void ExecuteBindMaterial(const Graphics::CommandList* commandList) const;


        void SetFillMode(D3D12_FILL_MODE fillMode);

    private:
        const Shader* m_Shader; // 材质使用的着色器

        std::unique_ptr<Graphics::GraphicsPipelineState> m_PipelineState;   // 材质定义的管线状态
        std::unique_ptr<Graphics::DescriptorHeap> m_ResourceDescHeap;       // 材质绑定的资源

        bool m_IsChanged;

    };

}