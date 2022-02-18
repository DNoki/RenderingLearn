#pragma once

namespace Application
{
    constexpr const WCHAR* WINDOW_TITLE = L"Rendering Learn";

    constexpr UINT DEFAULT_SCREEN_WIDTH = 1280u >> 1; // 初始窗口宽度
    constexpr UINT DEFAULT_SCREEN_HEIGHT = 720u >> 1; // 初始窗口高度

    extern HWND g_Hwnd;     // 当前程序窗口句柄


    std::wstring GetWindowTitle();
    void SetWindowTitle(const std::wstring& lpTitle);

    Path GetProjectPath();
    Path GetAssetPath();
    Path GetShaderPath();
}