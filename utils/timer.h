#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <chrono>

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

class Timer
{
public:
    float getElapsedSecs();
    int getElapsedMillis();
    void restart();

private:
    TimePoint m_start {Clock::now()};
};

#endif // TIMER_H_INCLUDED
