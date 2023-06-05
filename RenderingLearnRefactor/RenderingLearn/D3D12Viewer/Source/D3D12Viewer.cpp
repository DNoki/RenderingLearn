#include "pch.h"
#include "D3D12Viewer.h"

#pragma comment(lib, "D3D12Core.lib")


#include <iostream>

#define WIN32_LEAN_AND_MEAN // 从 Windows 头文件中排除极少使用的内容
#include <windows.h>
#include <tchar.h>

using namespace std;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ TCHAR* /*lpCmdLine*/, _In_ int nCmdShow)
{
#if DEBUG
    cout.imbue(locale(".utf8"));
    wcout.imbue(locale(".utf8"));
    system("chcp 65001");
#endif
    
    wcout << _T("你好啊") << endl;
    
    system("pause");

    return 0;
}