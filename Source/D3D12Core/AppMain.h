#pragma once

namespace Application
{
    constexpr LPCWSTR WINDOW_TITLE = L"Rendering Learn";

    constexpr UINT DEFAULT_SCREEN_WIDTH = 1280u; // 初始窗口宽度
    constexpr UINT DEFAULT_SCREEN_HEIGHT = 720u; // 初始窗口高度

    extern HWND g_Hwnd;     // 当前程序窗口句柄


    std::wstring GetWindowTitle();
    void SetWindowTitle(LPCTSTR lpTitle);

    Path GetProjectPath();
    Path GetAssetPath();
    Path GetShaderPath();
}