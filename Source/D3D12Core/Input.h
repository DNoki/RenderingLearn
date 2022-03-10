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
        static void Initialize(HWND hwnd);
        static void KeyboardProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
        static void MouseProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
        static void RefreshBeforeUpdate();


        inline static bool KeyDown(KeyCode key) { return GetInstance().m_KbdTracker->IsKeyPressed(key); }
        inline static bool KeyState(KeyCode key) { return GetInstance().m_KbdTracker->lastState.IsKeyDown(key); }
        inline static bool KeyUp(KeyCode key) { return GetInstance().m_KbdTracker->IsKeyReleased(key); }

        inline static bool MouseButtonDown(MouseButtonType button) { return *(GetInstance().m_MouseButtonsState[static_cast<int>(button)]) == DirectX::Mouse::ButtonStateTracker::PRESSED; };
        inline static bool MouseButtonState(MouseButtonType button) { return *(GetInstance().m_MouseButtonsState[static_cast<int>(button)]) == DirectX::Mouse::ButtonStateTracker::HELD; };
        inline static bool MouseButtonUp(MouseButtonType button) { return *(GetInstance().m_MouseButtonsState[static_cast<int>(button)]) == DirectX::Mouse::ButtonStateTracker::RELEASED; };

        inline static Vector2 GetMousePosition() { return Vector2((float)GetInstance().m_LastMouseState.x, (float)GetInstance().m_LastMouseState.y); };
        inline static Vector2 GetMouseDeltaPos() { return GetInstance().m_MouseDeltaPos; }
        inline static float GetMouseDeltaScrollWheel() { return GetInstance().m_MouseDeltaScrollWheel; }


    private:
        Input() = default;
        inline static Input& GetInstance()
        {
            static Input instance{};
            return instance;
        }

        std::unique_ptr<DirectX::Keyboard> m_Keyboard{}; // 键盘消息处理
        std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker> m_KbdTracker{}; // 键盘消息追踪

        std::unique_ptr<DirectX::Mouse> m_Mouse{}; // 鼠标消息处理
        std::unique_ptr<DirectX::Mouse::ButtonStateTracker> m_MouseTracker{}; // 鼠标消息追踪
        std::vector<const DirectX::Mouse::ButtonStateTracker::ButtonState*> m_MouseButtonsState{};
        DirectX::Mouse::State m_LastMouseState{}; // 鼠标最后状态
        Vector2 m_MouseDeltaPos{};
        float m_MouseDeltaScrollWheel{ 0.0f };
    };
}
