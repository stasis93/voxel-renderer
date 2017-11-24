#ifndef SKYBOX_H
#define SKYBOX_H

#include <glm/mat4x4.hpp>
#include <memory>

#include "Texture.h"
#include "shader.h"
#include "NonCopyable.h"

class Skybox : NonCopyable
{
public:
    Skybox() = default;
    ~Skybox();

    void initialize();
    void setShader(std::unique_ptr<Shader> pShader);
    void setTexture(std::unique_ptr<Texture> pTexture);
    void render(const glm::mat4& transform);

private:
    std::unique_ptr<Shader> m_shader {nullptr};
    std::unique_ptr<Texture> m_texture {nullptr};
    unsigned int m_vao {0}, m_vbo {0};
};

#endif // SKYBOX_H
