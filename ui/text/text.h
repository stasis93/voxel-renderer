#ifndef TEXT_H
#define TEXT_H

#include <string>

class Font;

class Text
{
public:
    Text() = default;
    Text(Font* font, const std::string& text = "Sample Text.");

    void setColor(float r, float g, float b);
    void setPosition(float px, float py);

    void setText(const std::string& text) {m_text = text;}
    std::string getText() const           {return m_text;}

    void setFont(Font* font)    {m_font = font;}
    const Font* getFont() const {return m_font;}

    void render();

private:
    Font*       m_font      {nullptr};
    std::string m_text      {"Sample Text."};
    float       m_color[3]  {0, 0, 0};
    float       m_px         {0.0f},
                m_py         {0.0f};
};

#endif // TEXT_H
