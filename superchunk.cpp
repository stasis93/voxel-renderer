#include "superchunk.h"
#include <memory>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "shader.h"

Superchunk::Superchunk(Shader& shader)
    : m_shader(shader)
{
    memset(m_chunks, 0, sizeof m_chunks);
}

Superchunk::~Superchunk()
{
    for (auto x = 0; x < SCX; x++)
    for (auto y = 0; y < SCY; y++)
    for (auto z = 0; z < SCZ; z++)
        if (m_chunks[x][y][z])
            delete m_chunks[x][y][z];
}

Chunk* Superchunk::getChunk(unsigned int cx, unsigned int cy, unsigned int cz)
{
    assert(cx < SCX && cy < SCY && cz < SCZ);

    if (!m_chunks[cx][cy][cz])
        return nullptr;
    return m_chunks[cx][cy][cz];
}

uint8_t Superchunk::get(unsigned int x, unsigned int y, unsigned int z) const
{
    int cx, cy, cz;

    cx = x / CX; x %= CX;
    cy = y / CY; y %= CY;
    cz = z / CZ; z %= CZ;

    assert(cx < SCX && cy < SCY && cz < SCZ);

    if (!m_chunks[cx][cy][cz])
        return 0;

    return m_chunks[cx][cy][cz]->get(x, y, z);
}

void Superchunk::set(unsigned int x, unsigned int y, unsigned int z, uint8_t type)
{
    int cx, cy, cz;

    cx = x / CX; x %= CX;
    cy = y / CY; y %= CY;
    cz = z / CZ; z %= CZ;

    assert(cx < SCX && cy < SCY && cz < SCZ);

    if (!m_chunks[cx][cy][cz])
        m_chunks[cx][cy][cz] = new Chunk(this, cx, cy, cz);

    m_chunks[cx][cy][cz]->set(x, y, z, type);
}

void Superchunk::render()
{
    m_shader.use();
    glm::mat4 ident{1};

    for (auto x = 0; x < SCX; x++)
    for (auto y = 0; y < SCY; y++)
    for (auto z = 0; z < SCZ; z++)
    {
        if (m_chunks[x][y][z])
        {
            glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(x * CX, y * CY, z * CZ));

            glUniformMatrix4fv(glGetUniformLocation(m_shader.id(), "model"),
                               1, GL_FALSE, glm::value_ptr(model));

            m_chunks[x][y][z]->render();
        }
    }
}

