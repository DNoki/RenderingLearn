#include "pch.h"

#include "AppConfig.h"

const UINT DEFAULT_SCREEN_WIDTH = 1280u >> 0;
const UINT DEFAULT_SCREEN_HEIGHT = 720u >> 0;

const WCHAR* WINDOW_TITLE = L"Rendering Learn";
std::wstring g_TitleFormat = L"%s  GPU(%s)  FPS:%.2f";
std::wstring g_TitleGPU = L"";

const UINT NODEMASK = 0; // TODO 多适配器系统 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/multi-engine

const bool VSYNC_ENABLE = true;
const bool USE_COMMAND_LIST_BUNDLE = false;

const UINT DIRECTIONAL_LIGHT_SHADOWMAP_SIZE = 2048;