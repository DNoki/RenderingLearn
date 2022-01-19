#pragma once

namespace Utility
{
    void Print(const char* msg);
    void Print(const wchar_t* msg);

    void Trace(const char* format, ...);
    void Trace(const wchar_t* format, ...);
    inline void Trace(void) {}

    std::string Format(const char* format, ...);
    std::wstring Format(const wchar_t* format, ...);

    std::string wchar2string(const wchar_t* str);
    std::wstring char2wstring(const char* str);

    HRESULT CheckHresult(HRESULT hr);
}



class ShaderUtility
{
public:
    inline static HRESULT CompileVSFromFile(LPCTSTR pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::VERTEX_SHADER, pFileName, ppCode); }
    inline static HRESULT CompilePSFromFile(LPCTSTR pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::PIXEL_SHADER, pFileName, ppCode); }
    inline static HRESULT CompileGSFromFile(LPCTSTR pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::GEOMETRY_SHADER, pFileName, ppCode); }
    inline static HRESULT CompileHSFromFile(LPCTSTR pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::HULL_SHADER, pFileName, ppCode); }
    inline static HRESULT CompileDSFromFile(LPCTSTR pFileName, ID3DBlob** ppCode) { return CompileFromFile(ShaderType::DOMAIN_SHADER, pFileName, ppCode); }

    inline static HRESULT ReadVSFromFile(LPCTSTR pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::VERTEX_SHADER, pFileName, ppCode); }
    inline static HRESULT ReadPSFromFile(LPCTSTR pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::PIXEL_SHADER, pFileName, ppCode); }
    inline static HRESULT ReadGSFromFile(LPCTSTR pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::GEOMETRY_SHADER, pFileName, ppCode); }
    inline static HRESULT ReadHSFromFile(LPCTSTR pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::HULL_SHADER, pFileName, ppCode); }
    inline static HRESULT ReadDSFromFile(LPCTSTR pFileName, ID3DBlob** ppCode) { return ReadFromFile(ShaderType::DOMAIN_SHADER, pFileName, ppCode); }

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
    static HRESULT ReadFromFile(ShaderType type, LPCWSTR pFileName, ID3DBlob** ppCode);

};