#pragma once

#include <functional>
#include <queue>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <bitset>
#include <typeinfo>
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
#include <winrt/base.h>
//#include <winrt/Windows.Foundation.h>

// --------------------------------------------------------------------------
// Direct3D https://docs.microsoft.com/zh-cn/windows/win32/direct3d
// Direct3D 12 图形 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/direct3d-12-graphics
#include <d3d12.h>
#ifdef DEBUG
#include <atltrace.h>   // 用于向VC窗口输出调试信息
#include <dxgidebug.h>  // DXGI 调试库
#endif

// --------------------------------------------------------------------------
// DXGI https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/dx-graphics-dxgi
#include <dxgi1_6.h>

// --------------------------------------------------------------------------
// Direct3D 12 的帮助程序结构 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/helper-structures-for-d3d12
#define D3DX12_NO_STATE_OBJECT_HELPERS
#define D3DX12_NO_CHECK_FEATURE_SUPPORT_CLASS
#include <d3dx12.h> 


// --------------------------------------------------------------------------
// DirectXTK12 工具包 https://github.com/Microsoft/DirectXTK12
#include <SimpleMath.h> // 简单数学库封装
#include <Keyboard.h>
#include <Mouse.h>

#include <GeometricPrimitive.h> // 用于创建预置模型



#pragma region DEFINE
// 重新解释为char字符串
#define RP_CHAR(str) reinterpret_cast<const char*>(str)
// 重新解释为char8_t字符串
#define RP_CHAR8(str) reinterpret_cast<const char8_t*>(str)

// 边界对齐，B为2的指数倍
#define UINT_UPPER(A, B) (((UINT)A + (UINT)B - 1u)&~((UINT)B - 1u))
#define UINT64_UPPER(A, B) (((UINT64)A + (UINT64)B - 1u)&~((UINT64)B - 1u))

typedef std::filesystem::path Path;

// 空描述符地址
constexpr D3D12_GPU_VIRTUAL_ADDRESS D3D12_GPU_VIRTUAL_ADDRESS_NULL = 0;
// 未知描述符地址
constexpr D3D12_GPU_VIRTUAL_ADDRESS D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN = -1;

#pragma endregion


// --------------------------------------------------------------------------
// 共通头文件
#include "GlobalUtility.h"
#define USE_COLUMN_MAJOR
#include "MathCommon.h"


#if DEBUG

#pragma warning(disable:26812) // 禁用 Enum 警告

// 输出调试信息到控制台
#define TRACE(...) Application::Trace(__VA_ARGS__)
// 输出调试信息到 VC 输出面板
#define TRACE_VC(...) ATLTRACE(__VA_ARGS__)

// 当表达式为 False 时阻断程序
#if 0
#define ASSERT(isFalse, ...) assert(isFalse);
#else
#define ASSERT(isFalse, ...) \
        if (!(bool)(isFalse)) { \
            TRACE(L"ERROR::\'" #isFalse "\' is false."); \
            TRACE(__VA_ARGS__); \
            TRACE(L"\n"); \
            DebugBreak(); \
        }
#endif

// 检查 HR 返回代码
#define CHECK_HRESULT(hr) Application::CheckHresult(hr);

// 添加调试名称
#define SET_DEBUGNAME(obj, name) Application::SetDebugName(obj, name)

#else

#define TRACE(...) ((void)0)
#define ASSERT(isFalse, ...) ((void)0)
#define CHECK_HRESULT(hr) hr

#endif // DEBUG

