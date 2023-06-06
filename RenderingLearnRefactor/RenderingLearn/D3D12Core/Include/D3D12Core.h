#pragma once

#include <bitset>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <typeinfo>
#include <vector>

// -------------------------------------------
#define WIN32_LEAN_AND_MEAN // 从 Windows 头中排除极少使用的资料
#include <windows.h>

// -------------------------------------------
//#include <wrl.h>
// 从 WRL 移动到 C++/WinRT https://docs.microsoft.com/zh-cn/windows/uwp/cpp-and-winrt-apis/move-to-winrt-from-wrl
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>

// -------------------------------------------
// Direct3D https://docs.microsoft.com/zh-cn/windows/win32/direct3d
// Direct3D 12 图形 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/direct3d-12-graphics
#include <d3d12.h>
#ifdef DEBUG
#include <atltrace.h>   // 用于向VC窗口输出调试信息
#include <dxgidebug.h>  // DXGI 调试库
#endif

// -------------------------------------------
// DXGI https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/dx-graphics-dxgi
#include <dxgi1_6.h>

// -------------------------------------------
// Direct3D 12 的帮助程序结构 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/helper-structures-for-d3d12
#define D3DX12_NO_STATE_OBJECT_HELPERS
#define D3DX12_NO_CHECK_FEATURE_SUPPORT_CLASS
//#include <D3DX12/d3dx12.h> 


#ifndef TEXT
#define TEXT(x) _T(x)
#endif

// 重命名常用类型
typedef std::filesystem::path Path;

#ifdef UNICODE
typedef std::wstring String;
#else
typedef std::string String;
#endif

template <class T>
using UniquePtr = std::unique_ptr<T>;
template <class T>
using SharedPtr = std::shared_ptr<T>;
template <class T>
using WeakPtr = std::weak_ptr<T>;
template <class T>
using Vector = std::vector<T>;

#include "Utility/Utility.h"

#define FORMAT(x, ...) D3D12Core::Format(x, __VA_ARGS__)

#if DEBUG

// 输出调试信息到控制台
#define TRACE(...) D3D12Core::Trace(__VA_ARGS__)
// 输出调试信息到 VC 输出面板
//#define TRACE_VC(...) ATLTRACE(__VA_ARGS__)

// 当表达式为 False 时阻断程序
#define ASSERT(isFalse, ...) \
        if (!(bool)(isFalse)) { \
            TRACE(L"ERROR::\'" #isFalse "\' is false."); \
            TRACE(__VA_ARGS__); \
            TRACE(L"\n"); \
            DebugBreak(); \
        }

// 检查 HR 返回代码
#define CHECK_HRESULT(hr) D3D12Core::CheckHresult(hr)

#else

#define TRACE(...) (void)()
#define ASSERT(isFalse, ...) (void)(isFalse)
#define CHECK_HRESULT(hr) (void)(hr)

#endif

#include "​GraphicsContext/GraphicsContext.h"
#include "​GraphicsContext/SwapChain.h"
#include "​GraphicsCommand/CommandQueue.h"
#include "​GraphicsCommand/CommandList.h"
#include "​GraphicsResource/ITexture.h"