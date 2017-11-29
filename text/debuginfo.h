#ifndef DEBUGINFO_H
#define DEBUGINFO_H

#include "text.h"
#include "font.h"

class DebugInfo
{
public:
    DebugInfo();

    void setContext(GLFWwindow* context);
    void setPositionInfo(float x, float y, float z);
    void setViewDirectionInfo(float x, float y, float z);

    void render();

private:
    void updateText();

private:
    Font        m_font;
    Text        m_text;

    float       m_pos[3];
    float       m_dir[3];

    bool        m_textNeedsUpdate {true};
};

#endif // DEBUGINFO_H
