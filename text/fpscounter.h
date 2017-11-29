#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include "font.h"
#include "text.h"
#include "timer.h"

class GLFWwindow;

class FPSCounter
{
public:
    FPSCounter();

    void setString(const std::string& text);
    void setPosition(float px, float py);
    void render();

    void setContext(GLFWwindow* context);
    void tick();

private:
    Font    m_font;
    Text    m_text;
    Timer   m_timer;
    double  m_elapsed       {0};
    int     m_count         {0};
    float   m_fps           {0};
};

#endif // FPSCOUNTER_H
