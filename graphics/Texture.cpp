#include "Texture.h"
#include <glad/glad.h>

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

void Texture::bind() const
{
    if (m_id)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(m_target, m_id);
    }
}

