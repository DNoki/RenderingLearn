#include "pch.h"

#include <iostream>

#include "GraphicsCore.h"
#include "Display.h"

#include "AppMain.h"

//#pragma comment(lib, "d3d12.lib")
//#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;


namespace Application
{
    HWND g_Hwnd; // 当前程序窗口句柄
    bool g_ExitFlag;

    Path g_AppFullPath; // 程序全路径
    Path g_AppPath;     // 程序路径
    Path g_ProjectPath; // 项目路径 TODO 对应发布版本
    Path g_AssetPath;   // 资源路径
    Path g_ShaderPath;  // 着色器路径

    wstring GetWindowTitle()
    {
        TCHAR pszWndTitle[MAX_PATH] = {};
        GetWindowText(g_Hwnd, pszWndTitle, MAX_PATH);
        return pszWndTitle;
    }

    void SetWindowTitle(LPCTSTR lpTitle)
    {
        SetWindowText(g_Hwnd, lpTitle);
    }

    Path GetProjectPath()
    {
        return g_ProjectPath;
    }
    Path GetAssetPath()
    {
        return g_AssetPath;
    }
    Path GetShaderPath()
    {
        return g_ShaderPath;
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_ACTIVATE: // 窗口正在激活或停用
            break;

        case WM_SIZE: // 在窗口的大小更改后发送到窗口
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            Display::Resize(width, height); // TODO 不应该再实时窗口改变时重置窗口大小，考虑放到每帧开始渲染时修改
        }
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

    void CreateGameWindow(HINSTANCE hInstance, LPCTSTR title, UINT width, UINT height)
    {
        // 第一个 Windows 程序
        // https://docs.microsoft.com/zh-cn/windows/win32/learnwin32/creating-a-window

        // --------------------------------------------------------------------------
        // 注册窗口类 
        // https://docs.microsoft.com/zh-cn/windows/win32/winmsg/using-window-classes
        WNDCLASSEX wcex{};
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
        RECT rc = { 0, 0, (LONG)width, (LONG)height };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        // 创建窗口
        g_Hwnd = CreateWindowEx(
            0,                      // Optional window styles
            title,                  // Window class
            title,                  // Window text
            WS_OVERLAPPEDWINDOW,    // top-level window style
            CW_USEDEFAULT,          // default horizontal position 
            CW_USEDEFAULT,          // default vertical position 
            rc.right - rc.left,     // 窗口宽度
            rc.bottom - rc.top,     // 窗口高度
            nullptr,                // Parent window
            nullptr,                // Menu
            hInstance,              // 应用程序实例的句柄
            nullptr                 // Additional application data
        );
    }

    int RunApplication(HINSTANCE hInstance, int nCmdShow)
    {
        // 程序路径
        {
            TCHAR path[MAX_PATH + 1];
            if (GetModuleFileName(NULL, path, MAX_PATH) == 0)
            {
                auto errorHR = HRESULT_FROM_WIN32(GetLastError());
                CHECK_HRESULT(errorHR);
                return errorHR;
            }

            g_AppFullPath = Path(path);
            g_AppPath = g_AppFullPath.parent_path();
            g_ProjectPath = g_AppFullPath;
            while (g_ProjectPath.filename() != "RenderingLearn")
            {
                if (!g_ProjectPath.has_parent_path())
                    ASSERT(0, L"未能找到项目路径");
                g_ProjectPath = g_ProjectPath.parent_path();
            }
            g_AssetPath = g_ProjectPath;
            g_AssetPath.append("Assets");
            //g_ShaderPath = g_ProjectPath;
            //g_ShaderPath.append("Source\\Shaders"); 
            g_ShaderPath = g_AppPath;
            g_ShaderPath.append("CompiledShaders");
        }

        CreateGameWindow(hInstance, WINDOW_TITLE, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
        if (g_Hwnd == NULL)
        {
            auto errorHR = HRESULT_FROM_WIN32(GetLastError());
            CHECK_HRESULT(errorHR);
            return errorHR;
        }

        // --------------------------------------------------------------------------
        Graphics::Initialize();

        // --------------------------------------------------------------------------
        ShowWindow(g_Hwnd, nCmdShow);

        MSG msg;
        while (!g_ExitFlag)
        {
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT)
                    g_ExitFlag = true;
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

using namespace Application;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPTCH /*lpCmdLine*/, _In_ int nCmdShow)
{
    wcout.imbue(locale("CHS"));

    auto result = RunApplication(hInstance, nCmdShow);

    //system("pause");

    return 0;
}