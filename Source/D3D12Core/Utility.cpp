#include "pch.h"
#include <fstream>
#include <iostream>

#include <AtlConv.h> // ATL 和 MFC 字符串转换宏 https://docs.microsoft.com/zh-cn/previous-versions/87zae4a3(v=vs.140)?redirectedfrom=MSDN

#include "GraphicsCore.h"

#include "Utility.h"

using namespace std;

namespace Utility
{
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

    std::string wchar2string(const wchar_t* str)
    {
        USES_CONVERSION;
        return W2A(str);
    }
    std::wstring char2wstring(const char* str)
    {
        USES_CONVERSION;
        return A2W(str);
    }

    HRESULT CheckHresult(HRESULT hr)
    {
        // 查询错误信息 https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/dxgi-error
        if (hr == 0x887a0005)
        {
            auto error = Graphics::g_Device->GetDeviceRemovedReason();
            ASSERT(0);
        }
        return winrt::check_hresult(hr);
    }
}




#include <d3dcompiler.h>

using namespace winrt;


HRESULT ShaderUtility::CompileFromFile(ShaderType type, LPCTSTR pFileName, ID3DBlob** ppCode)
{
    const LPCSTR entryPoints[] =
    {
        "VSMain",
        "PSMain",
        "GSMain",
        "HSMain",
        "DSMain",
    };
    const LPCSTR compileTarget[] =
    {
        "vs_5_0",
        "ps_5_0",
        "gs_5_0",
        "hs_5_0",
        "ds_5_0",
    };

    // CompileFlags https://docs.microsoft.com/zh-cn/windows/win32/direct3dhlsl/d3dcompile-constants
#ifdef _DEBUG
    // 使用图形调试工具启用着色器调试
    auto compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    auto compileFlags = 0;
#endif // _DEBUG

    HRESULT compileResult;
    com_ptr<ID3DBlob> errorBlob;

    // #define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)
    // D3D_COMPILE_STANDARD_FILE_INCLUDE 可以在任何 API 中为 pInclude 传递，并指示应该使用简单的默认包含处理程序。 包含处理程序将包含与当前目录相关的文件和与初始源文件目录相关的文件。 当与 D3DCompile 之类的 API 一起使用时，pSourceName 必须是文件名，并且初始相对目录将从它派生。

    compileResult = D3DCompileFromFile(
        pFileName,                              // HLSL文件路径
        nullptr,                                // 定义着色器宏
        D3D_COMPILE_STANDARD_FILE_INCLUDE,      // 默认包含处理
        entryPoints[static_cast<int>(type)],    // 着色器入口函数
        compileTarget[static_cast<int>(type)],  // 着色器目标
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

HRESULT ShaderUtility::ReadFromFile(ShaderType type, LPCTSTR pFileName, ID3DBlob** ppCode)
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
    ASSERT(file.good(), L"Shader文件打开失败。");
    
    file.seekg(0, ios_base::end);
    auto size = file.tellg();
    file.seekg(0, ios_base::beg);

    auto hresult = D3DCreateBlob(size, ppCode);
    CHECK_HRESULT(hresult);
    
    file.read(reinterpret_cast<char*>((*ppCode)->GetBufferPointer()), size);
    file.close();

    return hresult;
}

