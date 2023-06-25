#pragma once

namespace D3D12Core
{
    String Format(const TCHAR* format, ...);

#if DEBUG

    extern void Print(const TCHAR* msg);

    extern void Trace(const TCHAR* format, ...);
    inline void Trace(void) {}

    extern HRESULT CheckHresult(HRESULT hr);


#endif

    extern int32 MB2WC_Impl(uint32 codePage, const ANSICHAR* src, WIDECHAR* dest);
    extern int32 WC2MB_Impl(uint32 codePage, const WIDECHAR* src, ANSICHAR* dest);

}

