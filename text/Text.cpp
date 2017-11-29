#include "text.h"
#include "font.h"


Text::Text(Font* font, const std::string& text)
    : m_font(font), m_text(text)
{
}

void Text::render()
{
    if (m_font)
    {
        m_font->setColor(m_color);
        m_font->renderText(m_text, m_px, m_py);
    }
}

void Text::setColor(float r, float g, float b)
{
    m_color[0] = r; m_color[1] = g; m_color[2] = b;
}

void Text::setPosition(float px, float py)
{
    m_px = px; m_py = py;
}
