#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #include <windows.h>
    #define WIN
#endif

#ifdef WIN
    using TimePoint = LARGE_INTEGER;
#else
    #include <chrono>
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
#endif


class Timer
{
public:
    Timer();
    double getElapsedSecs();
    void restart();

private:
#ifdef WIN
    LARGE_INTEGER& freq();
#endif
    TimePoint m_start;
};

#endif // TIMER_H_INCLUDED
