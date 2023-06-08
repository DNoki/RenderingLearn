#include "pch.h"
#include "D3D12Viewer.h"

#include "System/WindowApplication.h"

using namespace D3D12Viewer;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ TCHAR* /*lpCmdLine*/, _In_ int nCmdShow)
{
#if DEBUG
    std::cout.imbue(std::locale(".utf8"));
    std::wcout.imbue(std::locale(".utf8"));
    system("chcp 65001");
#endif

    WindowApplication::GetInstance().Run(hInstance, nCmdShow);

    //system("pause");



    return 0;
}
