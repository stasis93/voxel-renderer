#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include "textfield.h"
#include "timer.h"

class FPSCounter : public TextField
{
public:
    FPSCounter() = default;
    void tick();

private:
    Timer   m_timer;
    double  m_elapsed       {0};
    int     m_count         {0};
    float   m_fps           {0};
};

#endif // FPSCOUNTER_H
