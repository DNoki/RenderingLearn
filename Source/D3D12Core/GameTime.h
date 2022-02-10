#pragma once


namespace TimeSystem
{
    extern UINT64 FrameCount;
    extern UINT64 SwapFrameCount;
    extern float RunTime;
    extern float DeltaTime;

    void InitTimeSystem();
    void UpdateTimeSystem();
    inline void AddFrameCompleted() { FrameCount++; }
    inline void AddSwapFrameCompleted() { SwapFrameCount++; }
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
        inline static UINT64 GetFrameCount() { return TimeSystem::FrameCount; }
        /**
         * @brief 已在屏幕上呈现的帧数量
         * @return
        */
        inline static UINT64 GetSwapFrameCount() { return TimeSystem::SwapFrameCount; }
        /**
         * @brief 自游戏启动以来的时间
         * @return
        */
        inline static float GetRunTime() { return TimeSystem::RunTime; }
        /**
         * @brief 上一帧所用时间
         * @return
        */
        inline static float GetDeltaTime() { return TimeSystem::DeltaTime; }
        /**
         * @brief 实时帧速率
         * @return
        */
        inline static float GetFPS() { return 1.0f / TimeSystem::DeltaTime; }



    private:
    };
}