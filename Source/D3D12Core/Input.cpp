#include "pch.h"

#include <dinput.h>

#include "Input.h"


// --------------------------------------------------------------------------
/*
    人机接口设备
	https://docs.microsoft.com/en-us/windows-hardware/drivers/hid/
    https://docs.microsoft.com/en-us/windows/win32/api/_hid/

*/
// --------------------------------------------------------------------------

static bool g_KeyDownFlag;

Input::Input()
{
}

Input::~Input()
{
}

bool Input::TestKeyDown(KeyCode key)
{
	auto result = g_KeyDownFlag;
	g_KeyDownFlag = false;
    return result;
}

void Input::OnKeyDown(WPARAM wParam)
{
	if (VK_SPACE == (wParam & 0xFF))
	{
		g_KeyDownFlag = true;
	}
}
