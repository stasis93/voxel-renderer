#ifndef SKYBOX_H
#define SKYBOX_H

#include <glm/mat4x4.hpp>
#include <memory>

#include "renderable.h"
#include "utils/noncopyable.h"

class Skybox : public Renderable, public WithTexture, public WithShader, Transformable, NonCopyable
{
public:
    Skybox() = default;
    ~Skybox();

    void initialize();
    void setTransform(const glm::mat4& transform);
    void render();

private:
    unsigned int m_vao {0}, m_vbo {0};
};

#endif // SKYBOX_H
