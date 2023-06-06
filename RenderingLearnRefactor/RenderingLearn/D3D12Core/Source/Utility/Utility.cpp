#include "pch.h"
#include "Utility/Utility.h"

//#include <d3dcompiler.h>
//#include <AtlConv.h> // ATL 和 MFC 字符串转换宏 https://docs.microsoft.com/zh-cn/previous-versions/87zae4a3(v=vs.140)?redirectedfrom=MSDN

//#include "GraphicsManager.h"


// 重新解释为char字符串
#define RP_CHAR(str) reinterpret_cast<const char*>(str)
// 重新解释为char8_t字符串
#define RP_CHAR8(str) reinterpret_cast<const char8_t*>(str)

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

namespace D3D12Core
{
    String Format(const TCHAR* format, ...)
    {
        TCHAR buffer[256];
        va_list ap;
        va_start(ap, format);
#ifdef UNICODE
        vswprintf(buffer, 256, format, ap);
#else
        vsprintf_s(buffer, 256, format, ap);
#endif
        va_end(ap);
        return buffer;
    }

    void Print(const TCHAR* msg)
    {
        using namespace std;

        OutputDebugString(Format(TEXT("%s\r\n"), msg).c_str());
        //ATLTRACE(Format(TEXT("%s\r\n"), msg).c_str());

#ifdef UNICODE
        wcout << msg << endl;
#else
        cout << msg << endl;
#endif
    }

    void Trace(const TCHAR* format, ...)
    {
        TCHAR buffer[256];
        va_list ap;
        va_start(ap, format);
#ifdef UNICODE
        vswprintf(buffer, 256, format, ap);
#else
        vsprintf_s(buffer, 256, format, ap);
#endif
        va_end(ap);
        Print(format);
    }

    void CheckHresult(HRESULT hr)
    {
        // 查询错误信息 https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/dxgi-error
        if (hr == 0x887a0005)
        {
            //auto error = Graphics::GraphicsManager::GetDevice()->GetDeviceRemovedReason();
            // 在即时窗口窗口查看设备移除原因
            ASSERT(0);
        }
        return winrt::check_hresult(hr);
    }

    //#pragma region CodePage Convert
    //    /**
    //     * @brief 多字节字符串转宽字节字符串
    //     * @param codePage
    //     * @param src
    //     * @param dest
    //     * @return
    //    */
    //    static inline int MB2WC_Impl(UINT codePage, const char* src, wchar_t* dest)
    //    {
    //        auto len = static_cast<int>(strnlen(src, MAX_PATH)) + 1;
    //        return MultiByteToWideChar(codePage, 0, src, len, dest, MAX_PATH);
    //    }
    //    /**
    //     * @brief 宽字节字符串转多字节字符串
    //     * @param codePage
    //     * @param src
    //     * @param dest
    //     * @return
    //    */
    //    static inline int WC2MB_Impl(UINT codePage, const wchar_t* src, char* dest)
    //    {
    //        auto len = static_cast<int>(wcsnlen(src, MAX_PATH)) + 1;
    //        return WideCharToMultiByte(codePage, 0, src, len, dest, MAX_PATH, NULL, NULL);
    //    }
    //
    //    string ToAnsi(const char* str)
    //    {
    //        wchar_t result[MAX_PATH]{};
    //        MB2WC_Impl(CP_UTF8, RP_CHAR(str), result);
    //        return ToAnsi(result);
    //    }
    //    string ToAnsi(const char8_t* str)
    //    {
    //        return ToAnsi(RP_CHAR(str));
    //    }
    //    string ToAnsi(const wchar_t* str)
    //    {
    //        char result[MAX_PATH]{};
    //        WC2MB_Impl(CP_ACP, str, result);
    //        return result;
    //    }
    //
    //    string ToUtf8(const char* str)
    //    {
    //        wchar_t result[MAX_PATH]{};
    //        MB2WC_Impl(CP_ACP, RP_CHAR(str), result);
    //        return ToUtf8(result);
    //    }
    //    string ToUtf8(const wchar_t* str)
    //    {
    //        char result[MAX_PATH]{};
    //        WC2MB_Impl(CP_UTF8, str, result);
    //        return result;
    //    }
    //
    //    wstring ToUnicode(const char* str, UINT codePage)
    //    {
    //        wchar_t result[MAX_PATH]{};
    //        MB2WC_Impl(codePage, str, result);
    //        return result;
    //    }
    //#pragma endregion

    //int g_GlobalDebugIndex = 0;
    //void SetDebugName(ID3D12Object* pObj, const wstring& name)
    //{
    //    wstring indexedName = Format(_T("%s_%06d"), name.c_str(), g_GlobalDebugIndex++);
    //    D3D12Core::CheckHresult(pObj->SetName(indexedName.c_str()));
    //}
    //void SetDebugName(ID3D12Object* pObj, const string& name)
    //{
    //    //SetDebugName(pObj, ToUnicode(name.c_str(), CP_UTF8));
    //}
    //wstring GetDebugName(ID3D12Object* pObj)
    //{
    //    UINT size = MAX_PATH;
    //    wstring text;
    //    text.resize(MAX_PATH);
    //    pObj->GetPrivateData(WKPDID_D3DDebugObjectNameW, &size, text.data());
    //    return text;
    //}
    //void SetDebugName(IDXGIObject* pObj, const wstring& name)
    //{
    //    wstring indexedName = Format(_T("%s_%06d"), name.c_str(), g_GlobalDebugIndex++);
    //    D3D12Core::CheckHresult(pObj->SetPrivateData(WKPDID_D3DDebugObjectNameW, static_cast<UINT>((indexedName.size() + 1) * sizeof(wchar_t)), indexedName.c_str()));
    //}
    //void SetDebugName(IDXGIObject* pObj, const string& name)
    //{
    //    //SetDebugName(pObj, ToUnicode(name.c_str(), CP_UTF8));
    //}
    //wstring GetDebugName(IDXGIObject* pObj)
    //{
    //    UINT size = MAX_PATH;
    //    wstring text;
    //    text.resize(MAX_PATH);
    //    pObj->GetPrivateData(WKPDID_D3DDebugObjectNameW, &size, text.data());
    //    return text;
    //}
    }
