#pragma once

#include "IGameResource.h"

namespace Graphics
{
    class RootSignature;
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

    class ShaderDesc
    {
    public:
        int m_SemanticFlags{ 1 << (int)VertexSemantic::Position }; // 着色器使用语义
        Path m_ShaderFilePaths[static_cast<int>(ShaderType::Count)]{};
        int m_SrvCount{ 0 };
        int m_UavCount{ 0 };
        int m_CbvCount{ 0 };
        int m_SamplerCount{ 0 };

        inline int GetBindResourceCount() const { return m_SrvCount + m_UavCount; }

        ShaderDesc() = default;

    private:

    };

    class Shader final : public IGameResource
    {
    public:
        Shader() = default;
        virtual ~Shader() override = default;;
        Shader(const Shader&) = delete;
        Shader(Shader&&) = default;

        inline Shader& operator = (const Shader&) = delete;
        inline Shader& operator = (Shader&&) = default;

        void Create(const ShaderDesc* shaderDesc);

        inline const ShaderDesc& GetShaderDesc() const { return m_ShaderDesc; }
        inline const Graphics::RootSignature* GetRootSignature() const { return m_RootSignature.get(); }
        inline D3D12_INPUT_LAYOUT_DESC GetInputLayout() const { return { m_InputLayouts.data(), static_cast<UINT>(m_InputLayouts.size()) }; }
        inline int GetBindSemanticFlag() const { return m_ShaderDesc.m_SemanticFlags; }
        inline const ID3DBlob* GetShaderBuffer(ShaderType type) const { return m_ShaderBlobs[static_cast<int>(type)].get(); }

        inline virtual std::wstring GetName() const override { return m_Name; }
        inline virtual void SetName(const std::wstring& name) override
        {
            m_Name = std::wstring(name);
            if (m_RootSignature)
                SET_DEBUGNAME(m_RootSignature->GetD3D12RootSignature(), Application::Format(_T("%s (RootSignature)"), m_Name.c_str()));
        }

    private:
        ShaderDesc m_ShaderDesc{};
        winrt::com_ptr<ID3DBlob> m_ShaderBlobs[static_cast<int>(ShaderType::Count)]{}; // 着色器编译缓冲

        std::unique_ptr<Graphics::RootSignature> m_RootSignature{};   // 根签名
        std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayouts{};       // 顶点输入结构

        std::wstring m_Name{};

        void ReadFromFile(ShaderType type, const Path& filePath);
    };

#if 0
    class ShaderUtility
    {
    public:
#if DEBUG
        inline static HRESULT CompileVSFromFile(Path& pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::VERTEX_SHADER, pFileName, ppCode); }
        inline static HRESULT CompilePSFromFile(Path& pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::PIXEL_SHADER, pFileName, ppCode); }
        inline static HRESULT CompileGSFromFile(Path& pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::GEOMETRY_SHADER, pFileName, ppCode); }
        inline static HRESULT CompileHSFromFile(Path& pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::HULL_SHADER, pFileName, ppCode); }
        inline static HRESULT CompileDSFromFile(Path& pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::DOMAIN_SHADER, pFileName, ppCode); }
#endif

        inline static HRESULT ReadVSFromFile(Path& pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::VERTEX_SHADER, pFileName, ppCode); }
        inline static HRESULT ReadPSFromFile(Path& pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::PIXEL_SHADER, pFileName, ppCode); }
        inline static HRESULT ReadGSFromFile(Path& pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::GEOMETRY_SHADER, pFileName, ppCode); }
        inline static HRESULT ReadHSFromFile(Path& pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::HULL_SHADER, pFileName, ppCode); }
        inline static HRESULT ReadDSFromFile(Path& pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::DOMAIN_SHADER, pFileName, ppCode); }

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