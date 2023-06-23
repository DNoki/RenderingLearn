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
}

