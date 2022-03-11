#pragma once

namespace Application
{
    extern HWND g_Hwnd;     // 当前程序窗口句柄


    std::wstring GetWindowTitle();
    void SetWindowTitle(const std::wstring& lpTitle);

    Path GetProjectPath();
    Path GetAssetPath();
    Path GetShaderPath();
}