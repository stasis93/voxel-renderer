#include "crosshair.h"
#include "utils/drawcalltrack.h"

void Crosshair::initialize()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, nullptr, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

Crosshair::~Crosshair()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void Crosshair::setTransform(const glm::mat4& transform)
{
    m_shader->use();
    m_shader->setMat4("projection", &transform[0][0]);
}

void Crosshair::setPosition(float px, float py)
{
    float vertices[4][4] = {
        // position      | texture coords
        {px     , py     , 0, 0},
        {px + 16, py     , 1, 0},
        {px + 16, py + 16, 1, 1},
        {px     , py + 16, 0, 1}
    };
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof vertices, vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Crosshair::render()
{
    m_shader->use();
    m_texture->bind();

    glBindVertexArray(m_vao);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays_(GL_TRIANGLE_FAN, 0, 4);
    glDisable(GL_BLEND);
}
