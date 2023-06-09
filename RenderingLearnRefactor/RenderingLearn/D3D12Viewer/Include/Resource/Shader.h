#pragma once

#include "IGameResource.h"

namespace D3D12Viewer
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

    class ShaderDesc
    {
    public:
        int m_SemanticFlags{ 1 << (int)VertexSemantic::Position }; // 着色器使用语义
        Path m_ShaderFilePaths[static_cast<int>(ShaderType::Count)]{};
        int m_SrvCount{ 0 };
        int m_UavCount{ 0 };
        int m_CbvCount{ 0 };
        int m_SamplerCount{ 0 };

        int GetBindResourceCount() const { return m_SrvCount + m_UavCount; }

        ShaderDesc() = default;

    private:

    };

    class Shader : public IGameResource
    {
    public:
        Shader() = default;
        //~Shader() override = default;;
        //Shader(const Shader&) = delete;
        //Shader(Shader&&) = default;
        //Shader& operator = (const Shader&) = delete;
        //Shader& operator = (Shader&&) = default;

        void Create(const ShaderDesc* shaderDesc);

        const ShaderDesc& GetShaderDesc() const { return m_ShaderDesc; }
        const D3D12Core::RootSignature* GetRootSignature() const { return m_RootSignature.get(); }
        D3D12_INPUT_LAYOUT_DESC GetInputLayout() const { return { m_InputLayouts.data(), static_cast<UINT>(m_InputLayouts.size()) }; }
        int GetBindSemanticFlag() const { return m_ShaderDesc.m_SemanticFlags; }
        const ID3DBlob* GetShaderBuffer(ShaderType type) const { return m_ShaderBlobs[static_cast<int>(type)].get(); }

        void SetName(const String& name) override;

    private:
        ShaderDesc m_ShaderDesc{};
        winrt::com_ptr<ID3DBlob> m_ShaderBlobs[static_cast<int>(ShaderType::Count)]{}; // 着色器编译缓冲

        UniquePtr<D3D12Core::RootSignature> m_RootSignature{};   // 根签名
        Vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayouts{};       // 顶点输入结构

        String m_Name{};

        void ReadFromFile(ShaderType type, const Path& filePath);
    };

#if 0
    class ShaderUtility
    {
    public:
#if DEBUG
        static HRESULT CompileVSFromFile(Path& pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::VERTEX_SHADER, pFileName, ppCode); }
        static HRESULT CompilePSFromFile(Path& pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::PIXEL_SHADER, pFileName, ppCode); }
        static HRESULT CompileGSFromFile(Path& pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::GEOMETRY_SHADER, pFileName, ppCode); }
        static HRESULT CompileHSFromFile(Path& pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::HULL_SHADER, pFileName, ppCode); }
        static HRESULT CompileDSFromFile(Path& pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::DOMAIN_SHADER, pFileName, ppCode); }
#endif

        static HRESULT ReadVSFromFile(Path& pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::VERTEX_SHADER, pFileName, ppCode); }
        static HRESULT ReadPSFromFile(Path& pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::PIXEL_SHADER, pFileName, ppCode); }
        static HRESULT ReadGSFromFile(Path& pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::GEOMETRY_SHADER, pFileName, ppCode); }
        static HRESULT ReadHSFromFile(Path& pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::HULL_SHADER, pFileName, ppCode); }
        static HRESULT ReadDSFromFile(Path& pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::DOMAIN_SHADER, pFileName, ppCode); }

    private:
        enum class ShaderType
        {
            VERTEX_SHADER = 0,
            PIXEL_SHADER,
            GEOMETRY_SHADER,
            HULL_SHADER,
            DOMAIN_SHADER,
        };

#if DEBUG
        /**
         * @brief 运行期编译着色器代码
         * @param type
         * @param pFileName
         * @param ppCode
         * @return
        */
        static HRESULT CompileFromFile(ShaderType type, Path& pFileName, ID3DBlob** ppCode);
#endif
        static HRESULT ReadFromFile(ShaderType type, Path& pFileName, ID3DBlob** ppCode);

    };
#endif
}