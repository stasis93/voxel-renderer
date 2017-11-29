#include "fpscounter.h"

constexpr double period = 0.5;

FPSCounter::FPSCounter()
{
    m_font.loadFromFile("fonts/arial.ttf");
    m_font.setSize(14);
    m_text.setFont(&m_font);
    m_text.setColor(1, 0, 1);
    m_text.setPosition(1, 16);
    m_text.setText("");
}

void FPSCounter::setString(const std::string& text)
{
    m_text.setText(text);
}

void FPSCounter::setPosition(float px, float py)
{
    m_text.setPosition(px, py);
}

void FPSCounter::render()
{
    m_text.render();
}

void FPSCounter::setContext(GLFWwindow* context)
{
    m_font.setContext(context);
}

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
