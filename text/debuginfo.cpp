#include "debuginfo.h"
#include <sstream>
#include <iomanip>

DebugInfo::DebugInfo()
{
    m_text.setPosition(1, 50);
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
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) <<
    "pos: " << m_pos[0] << "; " << m_pos[1] << "; " << m_pos[2] <<
    ";\ndir: " << m_dir[0] << "; " << m_dir[1] << "; " << m_dir[2] <<
    ";\ndraw calls: " << m_drawCalls << "\ntriangles: " << m_triangles;
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
