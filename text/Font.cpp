#include "font.h"

#include <iostream>
#include <cmath>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "text.h"
#include "ftlibrary.h"

#include "drawcalltrack.h"

Font::Font(GLFWwindow* context)
    : m_context(context)
{
}

Font::~Font()
{
    clearGlyphData();
    if (m_faceLoaded)
        FT_Done_Face(m_ftFace);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void Font::setContext(GLFWwindow* context)
{
    m_context = context;
}

void Font::setSize(unsigned size)
{
    if (FT_Set_Pixel_Sizes(m_ftFace, 0, size))
        onError("FT_Set_Pixel_Sizes failed");
    m_size = size;
    m_glyphDataNeedsUpdate = true;
}

unsigned Font::size() const
{
    return m_size;
}

void Font::initGLStuff()
{
    if (!m_glStuffInitialized)
    {
        std::cout << __FUNCTION__ << std::endl;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        m_shader.load("shaders/freetype_vs.glsl", "shaders/freetype_fs.glsl");
        m_glStuffInitialized = true;
    }
}

void Font::loadFromFile(const std::string& file)
{
    if (m_faceLoaded) {
        FT_Done_Face(m_ftFace);
        m_faceLoaded = false;
    }
    if (FT_New_Face(FTLibrary::get(), file.c_str(), 0, &m_ftFace))
        onError("FT_New_Face failed");
    m_faceLoaded = true;
    if (FT_Set_Pixel_Sizes(m_ftFace, 0, m_size))
        onError("FT_Set_Pixel_Sizes failed");
}

void Font::loadGlyphData()
{
    std::cout << __FUNCTION__ << std::endl;
    initGLStuff();
    clearGlyphData();

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(m_ftFace, c, FT_LOAD_RENDER)) {
            std::cerr << "FT_Load_Char failed" << std::endl;
            continue;
        }
        GLuint texture;
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_ftFace->glyph->bitmap.width,
                     m_ftFace->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                     m_ftFace->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GlyphInfo g = {
            texture,
            m_ftFace->glyph->bitmap.width,
            m_ftFace->glyph->bitmap.rows,
            m_ftFace->glyph->bitmap_left,
            m_ftFace->glyph->bitmap_top,
            m_ftFace->glyph->advance.x,
            m_ftFace->glyph->advance.y
        };
        m_glyphInfoMap.insert(std::pair<char, GlyphInfo>(c, g));
    }
}

void Font::clearGlyphData()
{
    for (auto& i : m_glyphInfoMap)
        glDeleteTextures(1, &i.second.textureID);
    m_glyphInfoMap.clear();
}

void Font::setColor(float* color)
{
    initGLStuff();
    m_shader.use();
    m_shader.setVec4("color", color);
}

void Font::renderText(const std::string& text, float px, float py)
{
    if (!m_context)
        return;

    int w, h;
    glfwGetFramebufferSize(m_context, &w, &h);

    float sx = 2.0 / w,
          sy = 2.0 / h;
    float x = -1.0 + px * sx,
          y =  1.0 - py * sy;

    if (m_glyphDataNeedsUpdate)
    {
        loadGlyphData();
        m_glyphDataNeedsUpdate = false;
    }
    m_shader.use();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(m_vao);

    for (auto c = text.begin(); c != text.end(); ++c)
    {
        GlyphInfo gi = m_glyphInfoMap[*c];

        float x2 = x + gi.left * sx;
        float y2 = -y - gi.top * sy;
        float w = gi.width * sx;
        float h = gi.height * sy;

        if (*c == '\n')
        {
            x = -1.0 + px * sx;
            y -= m_size * sy;
        }
        else
        {
            GLfloat box[4][4] = {
                /* position     | texture coord */
                {x2,     -y2 - h, 0, 1},
                {x2 + w, -y2 - h, 1, 1},
                {x2,     -y2,     0, 0},
                {x2 + w, -y2,     1, 0},
            };
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gi.textureID);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(box), box);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays_(GL_TRIANGLE_STRIP, 0, 4);

            x += (gi.advance_x >> 6) * sx;
            y += (gi.advance_y >> 6) * sy;
        }
    }
    glDisable(GL_BLEND);
}

void Font::onError(const std::string& message)
{
    std::cerr << message << std::endl;
    exit(EXIT_FAILURE);
}
