#pragma once

namespace Graphics
{
    class RootSignature;
    class GraphicsPipelineState;
    class CommandList;
}

namespace Game
{
    enum class ShaderType
    {
        VertexShader = 0,
        PixelShader,
        GeometryShader,
        HullShader,
        DomainShader,

        Count,
    };

    class Shader
    {
    public:
        Shader() = default;

        void Create();

        inline const Graphics::DescriptorHeap* GetResourceDescHeap() const { return m_ResourceDescHeap.get(); }

        void ExecuteBindDescriptorHeap(Graphics::CommandList* commandList) const;
        void ExecuteBindShader(Graphics::CommandList* commandList) const;

    private:
        winrt::com_ptr<ID3DBlob> m_ShaderBlobs[static_cast<int>(ShaderType::Count)];

        std::unique_ptr<Graphics::RootSignature> m_RootSignature;
        std::unique_ptr<Graphics::GraphicsPipelineState> m_PipelineState;
        std::unique_ptr<Graphics::DescriptorHeap> m_ResourceDescHeap;

        void ReadFromFile(ShaderType type, const Path& filePath);
    };

}