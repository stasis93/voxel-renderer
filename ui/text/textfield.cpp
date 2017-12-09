#include "textfield.h"

TextField::TextField()
{
    m_font.loadFromFile("fonts/arial.ttf");
    m_font.setSize(14);
    m_text.setFont(&m_font);
    m_text.setColor(1, 0, 1);
    m_text.setPosition(1, 16);
    m_text.setText("");
}

TextField::~TextField()
{
}

void TextField::setPosition(float px, float py)
{
    m_text.setPosition(px, py);
}

void TextField::setContext(GLFWwindow* context)
{
    m_font.setContext(context);
}

void TextField::setString(const std::string& str)
{
    m_text.setText(str);
}

void TextField::render()
{
    if (m_active)
        m_text.render();
}

std::string TextField::string() const
{
    return m_text.getText();
}

void TextField::setFontSize(unsigned size)
{
    m_font.setSize(size);
}

void TextField::setColor(float r, float g, float b)
{
    m_text.setColor(r, g, b);
}

void TextField::toggleActive()
{
    m_active = !m_active;
}
