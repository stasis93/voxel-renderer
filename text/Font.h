#ifndef FONT_H
#define FONT_H

#include <map>
#include <string>
#include "shader.h"
#include "noncopyable.h"

// Just copying from FT headers to make forward-declaring possible
class FT_LibraryRec_;
using FT_Library = FT_LibraryRec_*;
class FT_FaceRec_;
using FT_Face = FT_FaceRec_*;

class GLFWwindow;
class Text;

class Font : NonCopyable
{
public:
    Font() = default;
    Font(GLFWwindow* context);
    ~Font();

    void loadFromFile(const std::string& file);
    void setContext(GLFWwindow* context);
    void setSize(unsigned size);
    unsigned size() const;

private:
    void initGLStuff();
    void loadGlyphData();
    void clearGlyphData();
    void setColor(float* color);
    void renderText(const std::string& text, float px, float py);
    void onError(const std::string& message);

private:
    bool m_glyphDataNeedsUpdate {true};
    bool m_glStuffInitialized   {false};
    bool m_faceLoaded           {false};

    GLFWwindow* m_context {nullptr};
    FT_Face     m_ftFace;

    unsigned    m_vao {0}, m_vbo {0};
    Shader      m_shader;

    unsigned    m_size {24};

    struct GlyphInfo {
        unsigned textureID, width, height;
        int left, top, advance_x, advance_y;
    };
    std::map<char, GlyphInfo> m_glyphInfoMap;

    friend class Text;
};

#endif // FONT_H
