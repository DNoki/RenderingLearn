#pragma once

namespace D3D12Viewer
{
    enum EventFlag
    {
        Exit,       // 请求终止应用程序
        AdjWindow,  // 窗口进入移动或大小调整模式循环

        Count,
    };

    class WindowApplication
    {
    public:
        const String g_TitleFormat = L"%s  GPU(%s)  FPS:%.2f";
        const String g_TitleGPU = L"";

    public:
        HWND g_Hwnd; // 当前程序窗口句柄
        std::bitset<EventFlag::Count> g_AppEvent; // 应用程序事件集

        Path g_AppFullPath; // 程序全路径
        Path g_AppPath;     // 程序路径
        Path g_ProjectPath; // 项目路径 TODO 对应发布版本
        Path g_AssetPath;   // 资源路径
        Path g_ShaderPath;  // 着色器路径

        String WindowTitle = L"Rendering Learn";
        UINT ScreenWidth = 1280u >> 0;
        UINT ScreenHeight = 720u >> 0;

    public:
        static WindowApplication& GetInstance() { return s_Instance; }

        void Run(HINSTANCE hInstance, int nCmdShow);

    private:
        static WindowApplication s_Instance;

        WindowApplication() = default;

        void InitWindow(HINSTANCE hInstance);
        void InitPathes();

        static LRESULT CALLBACK WindowProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    };
}
