#include "timer.h"

#ifdef WIN
struct Frequency
{
    Frequency() {QueryPerformanceFrequency(&frequency);}
    LARGE_INTEGER frequency;
};
#endif

Timer::Timer()
{
#ifdef WIN
    QueryPerformanceCounter(&m_start);
#else
    m_start = Clock::now();
#endif
}

#ifdef WIN
LARGE_INTEGER& Timer::freq()
{
    static Frequency f;
    return f.frequency;
}
#endif

double Timer::getElapsedSecs()
{
#ifdef WIN
    LARGE_INTEGER end, elapsed;
    QueryPerformanceCounter(&end);
    elapsed.QuadPart = end.QuadPart - m_start.QuadPart;
    return elapsed.QuadPart / static_cast<double>(freq().QuadPart);
#else
    return std::chrono::duration<double>{Clock::now() - m_start}.count();
#endif
}

void Timer::restart()
{
#ifdef WIN
    QueryPerformanceCounter(&m_start);
#else
    m_start = Clock::now();
#endif
}
