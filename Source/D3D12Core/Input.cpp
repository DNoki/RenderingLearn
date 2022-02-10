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

static bool g_KeyDownFlag;

unique_ptr<Keyboard>                        Input::m_Keyboard;
unique_ptr<Keyboard::KeyboardStateTracker>  Input::m_KbdTracker;
unique_ptr<Mouse>                           Input::m_Mouse;
unique_ptr<Mouse::ButtonStateTracker>       Input::m_MouseTracker;
std::vector<const DirectX::Mouse::ButtonStateTracker::ButtonState*> Input::m_MouseButtonsState;
DirectX::Mouse::State                       Input::m_LastMouseState;

Input::Input()
{
}

Input::~Input()
{
}

void Input::Initialize(HWND hwnd)
{
    ASSERT(hwnd != NULL);

    // 键盘输入初始化
    m_Keyboard = unique_ptr<Keyboard>(new  Keyboard());
    m_KbdTracker = unique_ptr<Keyboard::KeyboardStateTracker>(new  Keyboard::KeyboardStateTracker());

    // 鼠标输入初始化
    m_Mouse = unique_ptr<Mouse>(new Mouse());
    m_Mouse->SetWindow(hwnd);
    m_MouseTracker = unique_ptr<Mouse::ButtonStateTracker>(new Mouse::ButtonStateTracker());
    m_MouseButtonsState =
    {
        &(m_MouseTracker->leftButton),
        &(m_MouseTracker->middleButton),
        &(m_MouseTracker->rightButton),
        &(m_MouseTracker->xButton1),
        &(m_MouseTracker->xButton2),
    };
}

void Input::KeyboardProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    m_Keyboard->ProcessMessage(message, wParam, lParam);
}
void Input::MouseProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    m_Mouse->ProcessMessage(message, wParam, lParam);
}

void Input::BeforeUpdate()
{
    auto kbdState = m_Keyboard->GetState();
    m_KbdTracker->Update(kbdState);

    m_LastMouseState = m_Mouse->GetState();
    m_MouseTracker->Update(m_LastMouseState);

    auto mousePos = GetMousePosition();
    if (Input::MouseButtonState(MouseButtonType::LeftButton))
        TRACE(L"%f, %f\n", mousePos.x, mousePos.y);
}
