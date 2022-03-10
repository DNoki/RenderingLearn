#include "pch.h"

//#include <dinput.h>

#include "Input.h"


// --------------------------------------------------------------------------
/*
    人机接口设备
    https://docs.microsoft.com/en-us/windows-hardware/drivers/hid/
    https://docs.microsoft.com/en-us/windows/win32/api/_hid/

*/
// --------------------------------------------------------------------------

using namespace std;
using namespace DirectX;
using namespace Game;

constexpr float MIN_MOUSE_SCROLL_WHEEL_DELTA = 1.0f / 120.0f; // 鼠标滚轮最小变化量归一化系数


void Input::Initialize(HWND hwnd)
{
    ASSERT(hwnd != NULL);
    auto& instance = GetInstance();

    // 键盘输入初始化
    instance.m_Keyboard.reset(new  Keyboard());
    instance.m_KbdTracker.reset(new  Keyboard::KeyboardStateTracker());

    // 鼠标输入初始化
    instance.m_Mouse.reset(new Mouse());
    instance.m_Mouse->SetWindow(hwnd);
    instance.m_MouseTracker.reset(new Mouse::ButtonStateTracker());
    instance.m_MouseButtonsState =
    {
        &(instance.m_MouseTracker->leftButton),
        &(instance.m_MouseTracker->middleButton),
        &(instance.m_MouseTracker->rightButton),
        &(instance.m_MouseTracker->xButton1),
        &(instance.m_MouseTracker->xButton2),
    };
}

void Input::KeyboardProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    GetInstance().m_Keyboard->ProcessMessage(message, wParam, lParam);
}
void Input::MouseProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    GetInstance().m_Mouse->ProcessMessage(message, wParam, lParam);
}

void Input::RefreshBeforeUpdate()
{
    auto& instance = GetInstance();

    auto kbdState = instance.m_Keyboard->GetState();
    instance.m_KbdTracker->Update(kbdState);

    // 存储用于计算微变量
    instance.m_MouseDeltaPos = GetMousePosition();
    instance.m_MouseDeltaScrollWheel = (float)instance.m_LastMouseState.scrollWheelValue;

    instance.m_LastMouseState = instance.m_Mouse->GetState();
    instance.m_MouseTracker->Update(instance.m_LastMouseState);

    instance.m_MouseDeltaPos = GetMousePosition() - instance.m_MouseDeltaPos;
    instance.m_MouseDeltaScrollWheel = ((float)instance.m_LastMouseState.scrollWheelValue - instance.m_MouseDeltaScrollWheel) * MIN_MOUSE_SCROLL_WHEEL_DELTA;
}
