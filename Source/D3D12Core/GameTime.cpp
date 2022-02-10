#include "pch.h"


#include "GameTime.h"

using namespace std;


class RealTimer
{
public:
    RealTimer()
    {
        LARGE_INTEGER frequency;
        auto result = QueryPerformanceFrequency(&frequency);
        ASSERT(result, L"ERROR::计时器初始化失败。");

        m_RecFrequency = 1.0 / static_cast<float>(frequency.QuadPart);
    }

    inline void Restart()
    {
        m_Counter.QuadPart = 0;
        Start();
    }
    inline void Start()
    {
        QueryPerformanceCounter(&m_StartTime);
    }
    inline void Stop()
    {
        LARGE_INTEGER endTime;
        QueryPerformanceCounter(&endTime);
        m_Counter.QuadPart += endTime.QuadPart - m_StartTime.QuadPart;
        m_StartTime.QuadPart = 0;
    }

    inline float GetElapsedSecond()
    {
        if (m_StartTime.QuadPart == 0)
            return static_cast<float>(static_cast<double>(m_Counter.QuadPart) * m_RecFrequency);
        else
        {
            LARGE_INTEGER currTime;
            QueryPerformanceCounter(&currTime);
            return static_cast<float>(static_cast<double>(currTime.QuadPart - m_StartTime.QuadPart + m_Counter.QuadPart) * m_RecFrequency);
        }
    }


private:
    double m_RecFrequency; // 每秒运行次数的倒数

    LARGE_INTEGER m_StartTime;  // 起始时间
    LARGE_INTEGER m_Counter;    // 记录时间
};

namespace TimeSystem
{
    UINT64 FrameCount = 0;
    UINT64 SwapFrameCount = 0;
    float RunTime = 0.0f;
    float DeltaTime = 0.0f;

    static RealTimer g_RunTimer;

    void InitTimeSystem()
    {
        g_RunTimer.Restart();
    }

    void UpdateTimeSystem()
    {
        //auto* frameCount = const_cast<UINT64*>(&Time::FrameCount);
        //frameCount++;

        float nowTime = g_RunTimer.GetElapsedSecond();
        DeltaTime = nowTime - RunTime;
        RunTime = nowTime;
    }
}
