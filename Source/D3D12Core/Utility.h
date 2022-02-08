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

#pragma region CodePage Convert
    /**
     * @brief UTF8字符串转化为Ansi字符串
    */
    std::string ToAnsi(const char* str);
    /**
     * @brief UTF8字符串转化为Ansi字符串
    */
    std::string ToAnsi(const char8_t* str);
    /**
     * @brief 宽字符串转化为Ansi字符串
    */
    std::string ToAnsi(const wchar_t* str);

    /**
     * @brief Ansi字符串转化为UTF8字符串
    */
    std::string ToUtf8(const char* str);
    /**
     * @brief 宽字符串转化为UTF8字符串
    */
    std::string ToUtf8(const wchar_t* str);

    /**
     * @brief 多字节字符串转宽字节字符串
     * @param str
     * @param codePage CP_ACP, CP_UTF8
     * @return
    */
    std::wstring ToUnicode(const char* str, UINT codePage);
#pragma endregion

    HRESULT CheckHresult(HRESULT hr);
}



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