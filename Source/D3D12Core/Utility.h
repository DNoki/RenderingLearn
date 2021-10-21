#pragma once

namespace Utility
{
    void Print(const char* msg);
    void Print(const wchar_t* msg);

    void Trace(const char* format, ...);
    void Trace(const wchar_t* format, ...);
    inline void Trace(void) {}

    HRESULT CheckHresult(HRESULT hr);
}



class ShaderUtility
{
public:
    inline static HRESULT CompileVSFromFile(LPCWSTR pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::VERTEX_SHADER, pFileName, ppCode); }
    inline static HRESULT CompilePSFromFile(LPCWSTR pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::PIXEL_SHADER, pFileName, ppCode); }
    inline static HRESULT CompileGSFromFile(LPCWSTR pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::GEOMETRY_SHADER, pFileName, ppCode); }
    inline static HRESULT CompileHSFromFile(LPCWSTR pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::HULL_SHADER, pFileName, ppCode); }
    inline static HRESULT CompileDSFromFile(LPCWSTR pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::DOMAIN_SHADER, pFileName, ppCode); }

private:
    enum class ShaderType
    {
        VERTEX_SHADER = 0,
        PIXEL_SHADER,
        GEOMETRY_SHADER,
        HULL_SHADER,
        DOMAIN_SHADER,
    };

    /**
     * @brief 运行期编译着色器代码
     * @param type
     * @param pFileName
     * @param ppCode
     * @return
    */
    static HRESULT CompileFromFile(ShaderType type, LPCWSTR pFileName, ID3DBlob** ppCode);

};