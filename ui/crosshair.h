#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include "renderable.h"

class Crosshair : public Renderable, public WithTexture, public WithShader, Transformable
{
public:
    Crosshair() = default;
    ~Crosshair();

    void initialize();
    void setTransform(const glm::mat4& transform);
    void setPosition(float px, float py);
    void render();

private:
    unsigned int m_vao {0}, m_vbo {0};
};

#endif // CROSSHAIR_H
