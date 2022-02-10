#pragma once

namespace Game
{
    typedef DirectX::Keyboard::Keys KeyCode;

    enum class MouseButtonType
    {
        LeftButton,
        MiddleButton,
        RightButton,
        XButton1,
        XButton2,

        Count,
    };

    class Input
    {
    public:
        ~Input();

        static void Initialize(HWND hwnd);
        static void KeyboardProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
        static void MouseProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
        static void BeforeUpdate();


        inline static bool KeyDown(KeyCode key) { return m_KbdTracker->IsKeyPressed(key); }
        inline static bool KeyState(KeyCode key) { return m_KbdTracker->lastState.IsKeyDown(key); }
        inline static bool KeyUp(KeyCode key) { return m_KbdTracker->IsKeyReleased(key); }

        inline static bool MouseButtonDown(MouseButtonType button) { return *m_MouseButtonsState[static_cast<int>(button)] == DirectX::Mouse::ButtonStateTracker::PRESSED; };
        inline static bool MouseButtonState(MouseButtonType button) { return *m_MouseButtonsState[static_cast<int>(button)] == DirectX::Mouse::ButtonStateTracker::HELD; };
        inline static bool MouseButtonUp(MouseButtonType button) { return *m_MouseButtonsState[static_cast<int>(button)] == DirectX::Mouse::ButtonStateTracker::RELEASED; };

        inline static Vector2 GetMousePosition() { return Vector2((float)m_LastMouseState.x, (float)m_LastMouseState.y); };
        inline static float GetMouseScrollWheel() { return (float)m_LastMouseState.scrollWheelValue; }


    private:
        Input();

        static std::unique_ptr<DirectX::Keyboard> m_Keyboard; // 键盘消息处理
        static std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker> m_KbdTracker; // 键盘消息追踪

        static std::unique_ptr<DirectX::Mouse> m_Mouse; // 鼠标消息处理
        static std::unique_ptr<DirectX::Mouse::ButtonStateTracker> m_MouseTracker; // 鼠标消息追踪
        static std::vector<const DirectX::Mouse::ButtonStateTracker::ButtonState*> m_MouseButtonsState;
        static DirectX::Mouse::State m_LastMouseState; // 鼠标最后状态
    };
}
