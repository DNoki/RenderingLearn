#include "pch.h"


#include "GameTime.h"

using namespace std;

namespace TimeSystem
{
    class RealTimer
    {
    public:
        RealTimer()
        {
            LARGE_INTEGER frequency;
            BOOL result = QueryPerformanceFrequency(&frequency);
            ASSERT(result, L"ERROR::计时器初始化失败。");

            m_RecFrequencyMilli = 1000.0 / (frequency.QuadPart);
            m_RecFrequency = 1.0 / (frequency.QuadPart);
        }

        inline void Restart()
        {
            m_StartTime.QuadPart = 0;
            m_Counter.QuadPart = 0;
            Start();
        }
        inline void Start()
        {
            if (m_StartTime.QuadPart != 0)
                Stop();
            QueryPerformanceCounter(&m_StartTime);
        }
        inline void Stop()
        {
            LARGE_INTEGER endTime;
            QueryPerformanceCounter(&endTime);
            m_Counter.QuadPart += endTime.QuadPart - m_StartTime.QuadPart;
            m_StartTime.QuadPart = 0;
        }

        inline double GetElapsedSecond()
        {
            if (m_StartTime.QuadPart == 0)
                return m_Counter.QuadPart * m_RecFrequency;
            else
            {
                LARGE_INTEGER currTime;
                QueryPerformanceCounter(&currTime);
                return (currTime.QuadPart - m_StartTime.QuadPart + m_Counter.QuadPart) * m_RecFrequency;
            }
        }
        inline double GetElapsedMillisecond()
        {
            if (m_StartTime.QuadPart == 0)
                return m_Counter.QuadPart * m_RecFrequencyMilli;
            else
            {
                LARGE_INTEGER currTime;
                QueryPerformanceCounter(&currTime);
                return (currTime.QuadPart - m_StartTime.QuadPart + m_Counter.QuadPart) * m_RecFrequencyMilli;
            }
        }

    private:
        double m_RecFrequencyMilli{ 0.0 }; // 每毫秒运行次数的倒数
        double m_RecFrequency{ 0.0 }; // 每秒运行次数的倒数

        LARGE_INTEGER m_StartTime{};  // 起始时间
        LARGE_INTEGER m_Counter{};    // 记录时间
    };

    static double g_RunTimeMilli = 0.0f;
    static float g_FpsDeltaTimeSum = 0.0f;
    static float g_FpsPrevTime = 0.0f;
    static UINT64 g_FpsPrevCount = 0;

    static RealTimer g_RunTimer;

    void InitTimeSystem()
    {
        g_RunTimer.Restart();
    }

    void RefreshTimeSystem()
    {
        // 线程等待
        //this_thread::sleep_for(chrono::microseconds(static_cast<UINT>((frameMinTime - deltaTimeMilli) * 1000)));

        double nowTimeMilli = g_RunTimer.GetElapsedMillisecond();
        g_RunTimeMilli = nowTimeMilli;

        float nowTime = static_cast<float>(nowTimeMilli * 0.001);
        g_DeltaTime = nowTime - g_RunTime;
        g_RunTime = nowTime;

        // 计算平均 FPS
        {
            g_FpsDeltaTimeSum += g_DeltaTime;
            if (g_RunTime - g_FpsPrevTime >= 0.1f)
            {
                g_FpsPrevTime = g_RunTime;
                g_AverageFps = 1.0f / (g_FpsDeltaTimeSum / (g_FrameCount - g_FpsPrevCount));
                g_FpsDeltaTimeSum = 0.0f;
                g_FpsPrevCount = g_FrameCount;
            }
        }
    }
    void ProcessMsg(UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_ACTIVATEAPP:
            break;
        case WM_ENTERSIZEMOVE: // 窗口进入调整模式（位置，大小）
            g_RunTimer.Stop();
            break;
        case WM_EXITSIZEMOVE: // 窗口退出调整模式（位置，大小）
            g_RunTimer.Start();
            break;
        default: break;
        }
    }
}
