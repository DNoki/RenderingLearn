#include "pch.h"

#include "GraphicsCore.h"

#include "AppMain.h"

//#pragma comment(lib, "d3d12.lib")
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "dxguid.lib")


namespace Application
{
    HWND g_Hwnd; // 当前程序窗口句柄
    bool ExitFlag;


    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_ACTIVATE: // 窗口正在激活或停用
            break;

        case WM_SIZE: // 在窗口的大小更改后发送到窗口

            break;

        case WM_CLOSE: // 窗口将要关闭时
            DestroyWindow(g_Hwnd);
            break;

        case WM_DESTROY: // 窗口已关闭
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    }

    int InitializeApplication(HINSTANCE hInstance, int nCmdShow)
    {
        // 第一个 Windows 程序
        // https://docs.microsoft.com/zh-cn/windows/win32/learnwin32/creating-a-window
        auto title = L"TestClass";
        ExitFlag = false;
        auto screenWidth = DEFAULT_SCREEN_WIDTH;
        auto screenHeight = DEFAULT_SCREEN_HEIGHT;

        // --------------------------------------------------------------------------
        // 注册窗口类 
        // https://docs.microsoft.com/zh-cn/windows/win32/winmsg/using-window-classes
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
        g_Hwnd = CreateWindowEx(
            0,                      // Optional window styles
            title,                  // Window class
            title,                  // Window text
            WS_OVERLAPPEDWINDOW,    // top-level window style
            CW_USEDEFAULT,          // default horizontal position 
            CW_USEDEFAULT,          // default vertical position 
            rc.right - rc.left,
            rc.bottom - rc.top,
            nullptr,                // Parent window
            nullptr,                // Menu
            hInstance,              // 应用程序实例的句柄
            nullptr                 // Additional application data
        );
        if (g_Hwnd == NULL)
        {
            DWORD dwError = GetLastError();
            ASSERT(0);
            return HRESULT_FROM_WIN32(dwError);
        }

        // --------------------------------------------------------------------------
        Graphics::Initialize();


        // --------------------------------------------------------------------------
        ShowWindow(g_Hwnd, nCmdShow);

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

            Graphics::OnRender();
        }

        // 一般向程序消息循环
        //while (GetMessage(&msg, NULL, 0, 0))
        //{
        //    TranslateMessage(&msg);
        //    DispatchMessage(&msg);
        //}

        Graphics::OnDestroy();

        return 0;
    }

}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR /*lpCmdLine*/, _In_ int nCmdShow)
{

    auto result = Application::InitializeApplication(hInstance, nCmdShow);

    return 0;
}