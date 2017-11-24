#include "timer.h"

double Timer::getElapsedSecs()
{
    auto dur = std::chrono::duration_cast<std::chrono::duration<double>>
                                        (Clock::now() - m_start);
    return dur.count();
}

int Timer::getElapsedMillis()
{
    auto dur = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>
                                        (Clock::now() - m_start);
    return dur.count();
}

void Timer::restart()
{
    m_start = Clock::now();
}
