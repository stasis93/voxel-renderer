#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <boost/chrono.hpp>

using Clock = boost::chrono::high_resolution_clock;
using TimePoint = boost::chrono::time_point<Clock>;

class Timer
{
public:
    double getElapsedSecs() const noexcept;
    void restart();

private:
    TimePoint m_start {Clock::now()};
};

#endif // TIMER_H_INCLUDED
