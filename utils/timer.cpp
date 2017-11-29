#include "timer.h"

using boost::chrono::duration;
using boost::ratio;

double Timer::getElapsedSecs() const noexcept
{
    return duration<double>{Clock::now() - m_start}.count();
}

void Timer::restart()
{
    m_start = Clock::now();
}
