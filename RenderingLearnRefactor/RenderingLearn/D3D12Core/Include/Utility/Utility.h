#pragma once

namespace D3D12Core
{
    String Format(const TCHAR* format, ...);

    //#pragma region CodePage Convert
    //    /**
    //     * @brief UTF8字符串转化为Ansi字符串
    //    */
    //    std::string ToAnsi(const char* str);
    //    /**
    //     * @brief UTF8字符串转化为Ansi字符串
    //    */
    //    std::string ToAnsi(const char8_t* str);
    //    /**
    //     * @brief 宽字符串转化为Ansi字符串
    //    */
    //    std::string ToAnsi(const wchar_t* str);
    //
    //    /**
    //     * @brief Ansi字符串转化为UTF8字符串
    //    */
    //    std::string ToUtf8(const char* str);
    //    /**
    //     * @brief 宽字符串转化为UTF8字符串
    //    */
    //    std::string ToUtf8(const wchar_t* str);
    //
    //    /**
    //     * @brief 多字节字符串转宽字节字符串
    //     * @param str
    //     * @param codePage CP_ACP, CP_UTF8
    //     * @return
    //    */
    //    std::wstring ToUnicode(const char* str, UINT codePage);
    //#pragma endregion

        //HRESULT CheckHresult(HRESULT hr);
        //void SetDebugName(ID3D12Object* pObj, const std::wstring& name);
        //void SetDebugName(ID3D12Object* pObj, const std::string& name);
        //std::wstring GetDebugName(ID3D12Object* pObj);
        //void SetDebugName(IDXGIObject* pObj, const std::wstring& name);
        //void SetDebugName(IDXGIObject* pObj, const std::string& name);
        //std::wstring GetDebugName(IDXGIObject* pObj);


#if DEBUG

    extern void Print(const TCHAR* msg);

    extern void Trace(const TCHAR* format, ...);
    inline void Trace(void) {}

    extern HRESULT CheckHresult(HRESULT hr);


#endif
}

