#pragma once

namespace Graphics
{
    class RootSignature;
    class DescriptorHeap;
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

        inline const Graphics::RootSignature* GetRootSignature() const { return m_RootSignature.get(); }
        inline const ID3DBlob* GetShaderBuffer(ShaderType type) const { return m_ShaderBlobs[static_cast<int>(type)].get(); }

    private:
        winrt::com_ptr<ID3DBlob> m_ShaderBlobs[static_cast<int>(ShaderType::Count)]; // 着色器编译缓冲

        std::unique_ptr<Graphics::RootSignature> m_RootSignature; // 根签名

        void ReadFromFile(ShaderType type, const Path& filePath);
    };

}