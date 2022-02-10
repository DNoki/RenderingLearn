#include "pch.h"

#include <d3dcompiler.h>
//#include <AtlConv.h> // ATL 和 MFC 字符串转换宏 https://docs.microsoft.com/zh-cn/previous-versions/87zae4a3(v=vs.140)?redirectedfrom=MSDN

#include "GraphicsCore.h"

#include "Utility.h"

// --------------------------------------------------------------------------
/*
    字符串格式转换符、标识符、格式符
        转换说明符
            %a(%A)     浮点数、十六进制数字和p-(P-)记数法(C99)
            %c         字符
            %d         有符号十进制整数
            %f         浮点数(包括float和doulbe)
            %e(%E)     浮点数指数输出[e-(E-)记数法]
            %g(%G)     浮点数不显无意义的零"0"
            %i         有符号十进制整数(与%d相同)
            %u         无符号十进制整数
            %o         八进制整数
            %x(%X)     十六进制整数0f(0F)   e.g.   0x1234
            %p         指针
            %s         字符串
            %%         输出字符%
        标志
            左对齐："-"   比如："%-20s"
            右对齐："+"   比如："%+20s"
            空格：若符号为正，则显示空格，负则显示"-"  比如："% 6.2f"
            #：对c,s,d,u类无影响；对o类，在输出时加前缀o；对x类，在输出时加前缀0x；对e,g,f 类当结果有小数时才给出小数点。

        printf的格式控制的完整格式：
            % - 0 m.n l或h 格式字符

            ①%：表示格式说明的起始符号，不可缺少。
            ②-：有-表示左对齐输出，如省略表示右对齐输出。
            ③0：有0表示指定空位填0,如省略表示指定空位不填。
            ④m.n：m指域宽，即对应的输出项在输出设备上所占的字符数。N指精度。用于说明输出的实型数的小数位数。未指定n时，隐含的精度为n=6位。
            ⑤l或h:l对整型指long型，对实型指double型。h用于将整型的格式字符修正为short型。

*/
// --------------------------------------------------------------------------

using namespace std;

namespace Utility
{
#if DEBUG
    void Print(const char* msg)
    {
        //OutputDebugStringA(msg); 
        cout << msg << endl;
    }
    void Print(const wchar_t* msg)
    {
        //OutputDebugStringW(msg); 
        wcout << msg << endl;
    }
    void Trace(const char* format, ...)
    {
        char buffer[256];
        va_list ap;
        va_start(ap, format);
        vsprintf_s(buffer, 256, format, ap);
        va_end(ap);
        Print(buffer);
    }
    void Trace(const wchar_t* format, ...)
    {
        wchar_t buffer[256];
        va_list ap;
        va_start(ap, format);
        vswprintf(buffer, 256, format, ap);
        va_end(ap);
        Print(buffer);
    }
#endif

    string Format(const char* format, ...)
    {
        char buffer[256];
        va_list ap;
        va_start(ap, format);
        vsprintf_s(buffer, 256, format, ap);
        va_end(ap);
        return buffer;
    }
    wstring Format(const wchar_t* format, ...)
    {
        wchar_t buffer[256];
        va_list ap;
        va_start(ap, format);
        vswprintf(buffer, 256, format, ap);
        va_end(ap);
        return buffer;
    }

#pragma region CodePage Convert
    /**
     * @brief 多字节字符串转宽字节字符串
     * @param codePage
     * @param src
     * @param dest
     * @return
    */
    static inline int MB2WC_Impl(UINT codePage, const char* src, wchar_t* dest)
    {
        auto len = static_cast<int>(strnlen(src, MAX_PATH)) + 1;
        return MultiByteToWideChar(codePage, 0, src, len, dest, MAX_PATH);
    }
    /**
     * @brief 宽字节字符串转多字节字符串
     * @param codePage
     * @param src
     * @param dest
     * @return
    */
    static inline int WC2MB_Impl(UINT codePage, const wchar_t* src, char* dest)
    {
        auto len = static_cast<int>(wcsnlen(src, MAX_PATH)) + 1;
        return WideCharToMultiByte(codePage, 0, src, len, dest, MAX_PATH, NULL, NULL);
    }

    std::string ToAnsi(const char* str)
    {
        wchar_t result[MAX_PATH]{};
        MB2WC_Impl(CP_UTF8, RP_CHAR(str), result);
        return ToAnsi(result);
    }
    string ToAnsi(const char8_t* str)
    {
        return ToAnsi(RP_CHAR(str));
    }
    string ToAnsi(const wchar_t* str)
    {
        char result[MAX_PATH]{};
        WC2MB_Impl(CP_ACP, str, result);
        return result;
    }

    string ToUtf8(const char* str)
    {
        wchar_t result[MAX_PATH]{};
        MB2WC_Impl(CP_ACP, RP_CHAR(str), result);
        return ToUtf8(result);
    }
    string ToUtf8(const wchar_t* str)
    {
        char result[MAX_PATH]{};
        WC2MB_Impl(CP_UTF8, str, result);
        return result;
    }

    std::wstring ToUnicode(const char* str, UINT codePage)
    {
        wchar_t result[MAX_PATH]{};
        MB2WC_Impl(codePage, str, result);
        return result;
    }
#pragma endregion

    HRESULT CheckHresult(HRESULT hr)
    {
        // 查询错误信息 https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/dxgi-error
        if (hr == 0x887a0005)
        {
            auto error = Graphics::g_Device->GetDeviceRemovedReason();
            // 在即时窗口窗口查看设备移除原因
            ASSERT(0);
        }
        return winrt::check_hresult(hr);
    }
}


#if DEBUG
HRESULT ShaderUtility::CompileFromFile(ShaderType type, Path& pFileName, ID3DBlob** ppCode)
{
    static const string ShaderModel = "5_1";
    static const string entryPoints[] =
    {
        "VSMain",
        "PSMain",
        "GSMain",
        "HSMain",
        "DSMain",
    };
    static const string compileTarget[] =
    {
        Utility::Format("vs_%s", ShaderModel),
        Utility::Format("ps_%s", ShaderModel),
        Utility::Format("gs_%s", ShaderModel),
        Utility::Format("hs_%s", ShaderModel),
        Utility::Format("ds_%s", ShaderModel),
    };

    // CompileFlags https://docs.microsoft.com/zh-cn/windows/win32/direct3dhlsl/d3dcompile-constants
#ifdef DEBUG
    // 使用图形调试工具启用着色器调试
    auto compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    auto compileFlags = 0;
#endif // DEBUG

    HRESULT compileResult;
    winrt::com_ptr<ID3DBlob> errorBlob;

    // #define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)
    // D3D_COMPILE_STANDARD_FILE_INCLUDE 可以在任何 API 中为 pInclude 传递，并指示应该使用简单的默认包含处理程序。 包含处理程序将包含与当前目录相关的文件和与初始源文件目录相关的文件。 当与 D3DCompile 之类的 API 一起使用时，pSourceName 必须是文件名，并且初始相对目录将从它派生。

    compileResult = D3DCompileFromFile(
        pFileName.c_str(),                              // HLSL文件路径
        nullptr,                                // 定义着色器宏
        D3D_COMPILE_STANDARD_FILE_INCLUDE,      // 默认包含处理
        entryPoints[static_cast<int>(type)].c_str(),    // 着色器入口函数
        compileTarget[static_cast<int>(type)].c_str(),  // 着色器目标
        compileFlags,                           // 着色器编译选项
        0,                                      // 效果编译选项
        ppCode,                                 // 接收已编译的代码
        errorBlob.put()                         // 接收编译错误信息
    );
    if (FAILED(compileResult) && errorBlob)
        TRACE((char*)errorBlob->GetBufferPointer());

    CHECK_HRESULT(compileResult);

    return compileResult;
}
#endif

HRESULT ShaderUtility::ReadFromFile(ShaderType type, Path& pFileName, ID3DBlob** ppCode)
{
    // C++ 文件读写操作
    //#include <ifstream> // 从文件读取
    //#include <ofstream> // 从文件读取
    //#include <fstream> // 打开文件供读写

    // 文件流打开模式
    // ios::in          只读
    // ios::out         只写
    // ios::app         从文件末尾开始写入
    // ios::binary      二进制模式
    // ios::nocreate    打开文件时若文件不存在，不创建文件
    // ios::noreplace   打开文件时若文件不存在，则创建文件
    // ios::trunc       打开文件并清空内容
    // ios::ate         打开文件并移动到文件尾

    // 状态标志服
    // is_open() 文件是否正常打开
    // bad() 读写过程中发生错误
    // fail() 读写过程中发生错误 或格式错误
    // eof() 读文件到达末尾
    // good() 以上任何一个返回true，则返回false

    // 文件流指针位置
    // ios::beg         文件头
    // ios::end         文件尾
    // ios::cur         当前位置

    // 文件流指针
    // tellg() 返回输入流指针的位置(类型为long)
    // tellp() 返回输出流指针的位置(类型为long)
    // seekg() 设置输入流指针位置，可偏移指定量
    // seekp() 设置输出流指针位置，可偏移指定量

    ifstream file;
    file.open(pFileName, ios::binary);
    ASSERT(file.good(), L"ERROR::Shader文件打开失败。");

    file.seekg(0, ios_base::end);
    auto size = file.tellg();
    file.seekg(0, ios_base::beg);
    
    auto hresult = D3DCreateBlob(size, ppCode);
    CHECK_HRESULT(hresult);

    file.read(reinterpret_cast<char*>((*ppCode)->GetBufferPointer()), size);
    file.close();

    return hresult;
}

