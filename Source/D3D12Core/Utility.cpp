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
    // ʹ��ͼ�ε��Թ���������ɫ������
    auto compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    auto compileFlags = 0;
#endif // _DEBUG

    HRESULT compileResult;
    com_ptr<ID3DBlob> errorBlob;

    // #define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)
    // D3D_COMPILE_STANDARD_FILE_INCLUDE �������κ� API ��Ϊ pInclude ���ݣ���ָʾӦ��ʹ�ü򵥵�Ĭ�ϰ���������� ����������򽫰����뵱ǰĿ¼��ص��ļ������ʼԴ�ļ�Ŀ¼��ص��ļ��� ���� D3DCompile ֮��� API һ��ʹ��ʱ��pSourceName �������ļ��������ҳ�ʼ���Ŀ¼������������

    compileResult = D3DCompileFromFile(
        pFileName,                              // HLSL�ļ�·��
        nullptr,                                // ������ɫ����
        D3D_COMPILE_STANDARD_FILE_INCLUDE,      // Ĭ�ϰ�������
        entryPoints[static_cast<int>(type)],    // ��ɫ����ں���
        compileTarget[static_cast<int>(type)],  // ��ɫ��Ŀ��
        compileFlags,                           // ��ɫ������ѡ��
        0,                                      // Ч������ѡ��
        ppCode,                                 // �����ѱ���Ĵ���
        errorBlob.put()                         // ���ձ��������Ϣ
    );
    if (FAILED(compileResult) && errorBlob)
        TRACE((char*)errorBlob->GetBufferPointer());

    //CHECK_HRESULT(compileResult);

    return compileResult;
}
//// ����Shader��������Ⱦ����״̬����ӿ�
//winrt::com_ptr<ID3DBlob> vertexShader;
//winrt::com_ptr<ID3DBlob> pixelShader;
//winrt::com_ptr<ID3DBlob> errorBlob;
//
//#ifdef _DEBUG
//// ʹ��ͼ�ε��Թ���������ɫ������
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