#include "pch.h"
#include <iostream>

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
    HRESULT CheckHresult(HRESULT hr)
    {
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


HRESULT ShaderUtility::CompileFromFile(ShaderType type, LPCWSTR pFileName, ID3DBlob** ppCode)
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

    //CHECK_HRESULT(compileResult);

    return compileResult;
}
//// 编译Shader，创建渲染管线状态对象接口
//winrt::com_ptr<ID3DBlob> vertexShader;
//winrt::com_ptr<ID3DBlob> pixelShader;
//winrt::com_ptr<ID3DBlob> errorBlob;
//
//#ifdef _DEBUG
//// 使用图形调试工具启用着色器调试
//auto compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
//#else
//auto compileFlags = 0;
//#endif // _DEBUG
//
//HRESULT compileResult;
//compileResult = D3DCompileFromFile(
//    L"F:\\DxProject\\RenderingLearnProject\\RenderingLearn\\Source\\Shaders\\Sample.hlsl",
//    nullptr, nullptr, "VSMain", "vs_5_0",
//    compileFlags, 0,
//    vertexShader.put(), errorBlob.put()
//);
//if (FAILED(compileResult) && errorBlob)
//TRACE((char*)errorBlob->GetBufferPointer());
//errorBlob = nullptr;
//CHECK_HRESULT(compileResult);
//
//compileResult = D3DCompileFromFile(
//    L"F:\\DxProject\\RenderingLearnProject\\RenderingLearn\\Source\\Shaders\\Sample.hlsl",
//    nullptr, nullptr, "PSMain", "ps_5_0",
//    compileFlags, 0,
//    pixelShader.put(), errorBlob.put()
//);
//if (FAILED(compileResult) && errorBlob)
//TRACE((char*)errorBlob->GetBufferPointer());
//errorBlob = nullptr;
//CHECK_HRESULT(compileResult);