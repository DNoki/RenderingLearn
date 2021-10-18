#include "pch.h"
#include "AppMain.h"




// --------------------------------------------------------------------------
// Direct3D https://docs.microsoft.com/zh-cn/windows/win32/direct3d
#include <d3d12.h>
//#include <d3dcompiler.h>
//#include <d3d12shader.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#include <d3dx12.h> // Direct3D 12 的帮助程序结构 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/helper-structures-for-d3d12

#include <DirectXMath.h> // 数学库


// --------------------------------------------------------------------------
// DXGI https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/dx-graphics-dxgi
#include <dxgi1_6.h>


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")





// 在调用COM接口时简化出错时处理时使用
//#define GRS_THROW_IF_FAILED(hr) if (FAILED(hr)){ throw CGRSCOMException(hr); }
//class CGRSCOMException
//{
//public:
//    CGRSCOMException(HRESULT hr) :_hrError(hr) {}
//    HRESULT Error() const { return _hrError; }
//
//private:
//    const HRESULT _hrError;
//};

using namespace std;

using namespace Microsoft;
using namespace Microsoft::WRL;

using namespace DirectX;

HWND g_hwnd;
bool ExitFlag;
int ScreenWidth = 1280;
int ScreenHeight = 720;
ID3D12Device6* g_Device;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATE: // 窗口正在激活或停用
        break;

    case WM_SIZE: // 在窗口的大小更改后发送到窗口

        break;

    case WM_CLOSE: // 窗口将要关闭时
        DestroyWindow(g_hwnd);
        break;

    case WM_DESTROY: // 窗口已关闭
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void InitializeApplication()
{
    // --------------------------------------------------------------------------
    // D3D12的COM接口
    winrt::com_ptr<ID3D12Device6> pID3D12Device;

    winrt::com_ptr<IDXGIAdapter4> pIDXGIAdapter;
    winrt::com_ptr<IDXGIFactory7> pIDXGIFactory;

    winrt::com_ptr<ID3D12CommandQueue> pID3D12CommandQueue;
    winrt::com_ptr<IDXGISwapChain1> pIDXGISwapChain1;

    //D3D12_VERTEX_BUFFER_VIEW stVertexBufferView={};

    //CD3DX12_VIEWPORT stViewPort(0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight));

    // --------------------------------------------------------------------------
    // 创建 DirectX 图形基础设施 (DXGI) 
    UINT nDXGIFactoryFlags = 0U;
    winrt::check_hresult(CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&pIDXGIFactory)));
    //GRS_THROW_IF_FAILED(pIDXGIFactory->MakeWindowAssociation(g_hwnd, DXGI_MWA_NO_ALT_ENTER)); // 防止 DXGI 响应 Alt+Enter 

    // --------------------------------------------------------------------------
    // 创建D3D12设备对象接口
    SIZE_T MaxSize = 0;
    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pIDXGIFactory->EnumAdapters1(adapterIndex, (IDXGIAdapter1**)(pIDXGIAdapter.put())); adapterIndex++)
    {
        // 遍历所有适配器
        DXGI_ADAPTER_DESC3 desc;
        pIDXGIAdapter->GetDesc3(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {} // 软件虚拟适配器，跳过
        else if (desc.DedicatedVideoMemory > MaxSize) // 选择最大专用适配器内存（显存）
        {
            auto hr = D3D12CreateDevice(pIDXGIAdapter.get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(pID3D12Device.put()));
            if (SUCCEEDED(hr))
            {
                TRACE(L"D3D12-capable hardware found:  %s (%u MB)\n", desc.Description, desc.DedicatedVideoMemory >> 20);
                MaxSize = desc.DedicatedVideoMemory;
            }
        }
        // 重新设定智能指针
        pIDXGIAdapter = nullptr;
    }
    if (MaxSize > 0)
        g_Device = pID3D12Device.detach();
    ASSERT(g_Device != nullptr, L"D3D12设备对象创建失败");

    // --------------------------------------------------------------------------
    // 创建D3D12命令队列接口
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    winrt::check_hresult(g_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pID3D12CommandQueue)));


}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
    // 第一个 Windows 程序
    // https://docs.microsoft.com/zh-cn/windows/win32/learnwin32/creating-a-window
    auto title = L"TestClass";
    ExitFlag = false;
    auto screenWidth = ScreenWidth;
    auto screenHeight = ScreenHeight;

    // --------------------------------------------------------------------------
    // 注册窗口类 
    // https://docs.microsoft.com/en-us/windows/win32/winmsg/using-window-classes
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_GLOBALCLASS; // 指示窗口类是应用程序全局类 https://docs.microsoft.com/zh-cn/windows/win32/winmsg/window-class-styles
    //wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH); // 防止背景重绘
    //wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = title;
    wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
    ASSERT(0 != RegisterClassEx(&wcex), "Unable to register a window");

    // --------------------------------------------------------------------------
    // 调整窗口大小
    RECT rc = { 0, 0, (LONG)screenWidth, (LONG)screenHeight };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    // 创建窗口
    g_hwnd = CreateWindowEx(
        0,                      // Optional window styles
        title,                  // Window class
        title,                  // Window text
        WS_OVERLAPPEDWINDOW,    // top-level window style
        CW_USEDEFAULT,          // default horizontal position 
        CW_USEDEFAULT,          // default vertical position 
        rc.right - rc.left,
        rc.bottom - rc.top,
        NULL,                   // Parent window
        NULL,                   // Menu
        hInstance,              // 应用程序实例的句柄
        NULL                    // Additional application data
    );
    if (g_hwnd == NULL)
    {
        DWORD dwError = GetLastError();
        ASSERT(0);
        return HRESULT_FROM_WIN32(dwError);
    }

    // --------------------------------------------------------------------------
    InitializeApplication();




    // --------------------------------------------------------------------------
    ShowWindow(g_hwnd, nCmdShow);

    MSG msg;
    while (!ExitFlag)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                ExitFlag = true;
        }
    }

    // 一般向程序消息循环
    //while (GetMessage(&msg, NULL, 0, 0))
    //{
    //    TranslateMessage(&msg);
    //    DispatchMessage(&msg);
    //}

    return 0;
}