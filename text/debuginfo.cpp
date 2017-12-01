#include "debuginfo.h"
#include <sstream>
#include <iomanip>

DebugInfo::DebugInfo()
{
    m_font.loadFromFile("fonts/arial.ttf");
    m_font.setSize(14);
    m_text.setColor(1, 0, 1);
    m_text.setFont(&m_font);
    m_text.setPosition(1, 50);
}

void DebugInfo::setContext(GLFWwindow* context)
{
    m_font.setContext(context);
}

void DebugInfo::setPositionInfo(float x, float y, float z)
{
    m_pos[0] = x;
    m_pos[1] = y;
    m_pos[2] = z;
    m_textNeedsUpdate = true;
}

void DebugInfo::setViewDirectionInfo(float x, float y, float z)
{
    m_dir[0] = x;
    m_dir[1] = y;
    m_dir[2] = z;
    m_textNeedsUpdate = true;
}

void DebugInfo::setDrawCallCount(int count)
{
    m_drawCalls = count;
    m_textNeedsUpdate = true;
}

void DebugInfo::setTriangleCount(int count)
{
    m_triangles = count;
}

void DebugInfo::updateText()
{
    std::string dc, tr;

    if (m_drawCalls)
        dc = std::to_string(m_drawCalls);
    else
        dc = "N/A";

    if (m_triangles)
        tr = std::to_string(m_triangles);
    else
        tr = "N/A";

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) <<
    "pos: " << m_pos[0] << "; " << m_pos[1] << "; " << m_pos[2] <<
    ";\ndir: " << m_dir[0] << "; " << m_dir[1] << "; " << m_dir[2] <<
    ";\ndraw calls: " << dc << ";\nTriangles: " << tr << ";";
    m_text.setText(ss.str());
}

void DebugInfo::render()
{
    if (m_textNeedsUpdate)
    {
        updateText();
        m_textNeedsUpdate = false;
    }
    TextField::render();
}
