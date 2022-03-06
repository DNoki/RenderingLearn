#pragma once


namespace TimeSystem
{
    inline UINT64 g_FrameCount{ 0 };
    inline UINT64 g_SwapFrameCount{ 0 };
    inline float g_RunTime{ 0.0 };
    inline float g_DeltaTime{ 0.0 };
    inline float g_AverageFps{ 0.0 };

    void InitTimeSystem();
    void RefreshTimeSystem();
    inline void AddFrameCompleted() { g_FrameCount++; }
    inline void AddSwapFrameCompleted() { g_SwapFrameCount++; }
    void ProcessMsg(UINT msg, WPARAM wParam, LPARAM lParam);
}

namespace Game
{
    class Time
    {
    public:
        /**
         * @brief 已渲染帧数量
         * @return
        */
        inline static UINT64 GetFrameCount() { return TimeSystem::g_FrameCount; }
        /**
         * @brief 已在屏幕上呈现的帧数量
         * @return
        */
        inline static UINT64 GetSwapFrameCount() { return TimeSystem::g_SwapFrameCount; }
        /**
         * @brief 自游戏启动以来的时间
         * @return
        */
        inline static float GetRunTime() { return TimeSystem::g_RunTime; }
        /**
         * @brief 上一帧所用时间
         * @return
        */
        inline static float GetDeltaTime() { return TimeSystem::g_DeltaTime; }
        /**
         * @brief 实时帧速率
         * @return
        */
        inline static float GetFPS() { return 1.0f / TimeSystem::g_DeltaTime; }
        /**
         * @brief 平均帧速率
         * @return
        */
        inline static float GetAverageFPS() { return TimeSystem::g_AverageFps; }

    private:

    };
}