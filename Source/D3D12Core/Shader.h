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
        inline D3D12_INPUT_LAYOUT_DESC GetInputLayout() const { return { m_InputLayouts.data(), static_cast<UINT>(m_InputLayouts.size()) }; }
        inline int GetBindSemanticFlag() const { return m_BindSemanticFlag; }
        inline int GetBindResourceCount() const { return m_BindResourceCount; }
        inline const ID3DBlob* GetShaderBuffer(ShaderType type) const { return m_ShaderBlobs[static_cast<int>(type)].get(); }

    private:
        winrt::com_ptr<ID3DBlob> m_ShaderBlobs[static_cast<int>(ShaderType::Count)]; // 着色器编译缓冲

        std::unique_ptr<Graphics::RootSignature> m_RootSignature;   // 根签名
        std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayouts;       // 顶点输入结构
        int m_BindSemanticFlag; // 着色器使用语义
        int m_BindResourceCount; // 着色器使用资源数量
        // TODO 着色器使用采样器数量

        void ReadFromFile(ShaderType type, const Path& filePath);
    };

}