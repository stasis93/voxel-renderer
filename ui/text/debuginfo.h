#ifndef DEBUGINFO_H
#define DEBUGINFO_H

#include "textfield.h"

class DebugInfo : public TextField
{
public:
    DebugInfo();

    void setPositionInfo(float x, float y, float z);
    void setViewDirectionInfo(float x, float y, float z);
    void setDrawCallCount(int count);
    void setTriangleCount(int count);
    void render();

private:
    void updateText();

private:
    float       m_pos[3];
    float       m_dir[3];
    int         m_drawCalls {0}, m_triangles {0};
    bool        m_textNeedsUpdate {true};
};

#endif // DEBUGINFO_H
