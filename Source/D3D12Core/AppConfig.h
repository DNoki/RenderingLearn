#pragma once

enum class VertexSemantic
{
    Position,
    Normal,
    Tangent,
    Color,
    Texcoord,

    Count
};

extern const UINT DEFAULT_SCREEN_WIDTH; // 初始窗口宽度
extern const UINT DEFAULT_SCREEN_HEIGHT; // 初始窗口高度

extern const WCHAR* WINDOW_TITLE;
extern std::wstring g_TitleFormat;
extern std::wstring g_TitleGPU;

extern const UINT NODEMASK;

extern const bool VSYNC_ENABLE;
extern const bool USE_COMMAND_LIST_BUNDLE;

extern const UINT DIRECTIONAL_LIGHT_SHADOWMAP_SIZE;