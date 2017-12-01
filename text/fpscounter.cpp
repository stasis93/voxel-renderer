#include "fpscounter.h"

constexpr double period = 0.5;

void FPSCounter::tick()
{
    m_count++;
    m_elapsed = m_timer.getElapsedSecs();
    if (m_elapsed > period)
    {
        m_fps = m_count / m_elapsed;
        m_timer.restart();
        m_count = 0;
        m_text.setText("FPS: " + std::to_string((int)m_fps));
    }
}
