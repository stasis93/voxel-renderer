#ifndef TEXTFIELD_H
#define TEXTFIELD_H

#include <string>

#include "font.h"
#include "text.h"
#include "renderable.h"

class TextField : public Renderable
{
public:
    TextField();
    virtual ~TextField();

    void setPosition(float px, float py);
    void setContext(GLFWwindow* context);
    void setString(const std::string& str);
    std::string string() const;
    void render();
    void toggleActive();

    void setFontSize(unsigned size);
    void setColor(float r, float g, float b);

protected:
    Font        m_font;
    Text        m_text;
    bool        m_active {true};
};

#endif // TEXTFIELD_H
