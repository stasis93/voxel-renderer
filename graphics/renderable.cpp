#include "renderable.h"

void WithShader::setShader(Shader& shader)
{
    m_shader = &shader;
}

void WithTexture::setTexture(Texture& texture)
{
    m_texture = &texture;
}
