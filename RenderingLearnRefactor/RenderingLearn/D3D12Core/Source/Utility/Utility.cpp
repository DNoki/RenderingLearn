#include "pch.h"
#include "Utility/Utility.h"

//#include <d3dcompiler.h>
//#include <AtlConv.h> // ATL 和 MFC 字符串转换宏 https://docs.microsoft.com/zh-cn/previous-versions/87zae4a3(v=vs.140)?redirectedfrom=MSDN

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
            %lld(I64d) 有符号长整型
            %llu(I64u) 无符号长整型
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
        Print(buffer);
    }

    HRESULT CheckHresult(HRESULT hr)
    {
        // 查询错误信息 https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/dxgi-error
        if (hr == 0x887a0005)
        {
            //auto error = Graphics::GraphicsManager::GetDevice()->GetDeviceRemovedReason();
            // 在即时窗口窗口查看设备移除原因
            ASSERT(0);
        }
        ASSERT(hr == 0);
        winrt::check_hresult(hr);
        return hr;
    }

#pragma region CodePage Convert
    /**
     * @brief 多字节字符串转宽字节字符串
     * @param codePage
     * @param src
     * @param dest
     * @return
    */
    int32 MB2WC_Impl(uint32 codePage, const ANSICHAR* src, WIDECHAR* dest)
    {
        auto len = static_cast<int32>(strnlen(src, MAX_PATH)) + 1;
        return MultiByteToWideChar(codePage, 0, src, len, dest, MAX_PATH);
    }
    /**
     * @brief 宽字节字符串转多字节字符串
     * @param codePage
     * @param src
     * @param dest
     * @return
    */
    int32 WC2MB_Impl(uint32 codePage, const WIDECHAR* src, ANSICHAR* dest)
    {
        auto len = static_cast<int32>(wcsnlen(src, MAX_PATH)) + 1;
        return WideCharToMultiByte(codePage, 0, src, len, dest, MAX_PATH, nullptr, nullptr);
    }
#pragma endregion
}
