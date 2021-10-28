#pragma once

#include <functional>
//#include <iostream>
//#include <string>
//#include <sstream>
//#include <ostream>

//#include <shellapi.h>
//#include <stdio.h>

// --------------------------------------------------------------------------
// 更新 WINVER 和 _WIN32_WINNT https://docs.microsoft.com/zh-cn/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160
#include <winsdkver.h>
#define _WIN32_WINNT 0x0A00 // Windows 10
#include <sdkddkver.h>

#define WIN32_LEAN_AND_MEAN // 从 Windows 头中排除极少使用的资料
#include <windows.h>
#include <tchar.h>

// --------------------------------------------------------------------------
//#include <wrl.h>
// 从 WRL 移动到 C++/WinRT https://docs.microsoft.com/zh-cn/windows/uwp/cpp-and-winrt-apis/move-to-winrt-from-wrl
#include "winrt/base.h"
//#include <winrt/Windows.Foundation.h>

// --------------------------------------------------------------------------
// Direct3D https://docs.microsoft.com/zh-cn/windows/win32/direct3d
// Direct3D 12 图形 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/direct3d-12-graphics
#include <d3d12.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif
// Direct3D 12 的帮助程序结构 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/helper-structures-for-d3d12
#include <d3dx12.h> 

// --------------------------------------------------------------------------
// DXGI https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/dx-graphics-dxgi
#include <dxgi1_6.h>


// --------------------------------------------------------------------------
#include "Utility.h"


#pragma region DEFINE

#if _DEBUG

// 输出调试信息
#define TRACE(...) Utility::Trace(__VA_ARGS__);
//#define TRACE_FORMAT(msg, ...) Utility::TraceFormat(msg, ...);

// 当表达式为 False 时阻断程序
#define ASSERT(isFalse, ...) \
        if (!(bool)(isFalse)) { \
            TRACE(L"\'" #isFalse "\' is false \n"); \
            TRACE(__VA_ARGS__); \
            TRACE(L"\n"); \
            DebugBreak(); \
        }

#define CHECK_HRESULT(hr) Utility::CheckHresult(hr);

// 边界对齐，B为2的指数倍
#define UINT_UPPER(A, B) (((UINT)A + (UINT)B - 1u)&~((UINT)B - 1u))
#define UINT64_UPPER(A, B) (((UINT64)A + (UINT64)B - 1u)&~((UINT64)B - 1u))
#else

#define TRACE(...)
#define ASSERT(isFalse, ...)
#define CHECK_HRESULT(hr)

#endif // _DEBUG

#pragma endregion
