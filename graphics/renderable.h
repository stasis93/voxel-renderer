#ifndef RENDERABLE_H_INCLUDED
#define RENDERABLE_H_INCLUDED

#include <memory>

#include <glm/mat4x4.hpp>

#include "texture.h"
#include "shader.h"

struct Renderable
{
    virtual ~Renderable() = default;
    virtual void render() = 0;
};

// ======================================================

struct Transformable
{
    virtual ~Transformable() = default;
    virtual void setTransform(const glm::mat4& transform) = 0;
};

// ======================================================

class WithTexture
{
public:
    void setTexture(Texture& texture);
protected:
    Texture* m_texture {nullptr};
};

// ======================================================

class WithShader
{
public:
    void setShader(Shader& shader);
protected:
    Shader* m_shader {nullptr};
};

#endif // RENDERABLE_H_INCLUDED
