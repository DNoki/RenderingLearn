#pragma once

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



//#include <iostream>
//#include <string>
//#include <sstream>
//#include <ostream>

//#include <shellapi.h>
//#include <stdio.h>

#include "Utility.h"


#pragma region DEFINE

#define TRACE(...) Utility::Trace(__VA_ARGS__);
//#define TRACE_FORMAT(msg, ...) Utility::TraceFormat(msg, ...);

#define ASSERT( isFalse, ... ) \
        if (!(bool)(isFalse)) { \
            TRACE(L"\'" #isFalse "\' is false \n"); \
            TRACE(__VA_ARGS__); \
            TRACE(L"\n"); \
            __debugbreak(); \
        }

#define CHECK_HRESULT(hr) Utility::CheckHresult(hr);

#pragma endregion

