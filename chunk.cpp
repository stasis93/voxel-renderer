#include "chunk.h"
#include "superchunk.h"
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <cassert>
#include "utils.h"

char Chunk::transp_bit = 0x40;
using byte4 = glm::tvec4<GLbyte>;

using namespace Blocks;

bool operator<(const Position3 &p1, const Position3 &p2)
{
    if (p1.z < p2.z)
        return true;
    if (p1.z > p2.z)
        return false;
    if (p1.y < p2.y)
        return true;
    if (p1.y > p2.y)
        return false;
    return p1.x < p2.x;
}

bool operator<(const Position2 &p1, const Position2 &p2)
{
    if (p1.y < p2.y)
        return true;
    if (p1.y > p2.y)
        return false;
    return p1.x < p2.x;
}




Chunk::Chunk(ChunkManager &manager, Position3 index)
    : m_parent(manager), m_pos(index)
{
    //PRINT_FUNC();
    memset(m_blocks, 0, sizeof(m_blocks));
    glGenBuffers(1, &m_vbo);
}

Chunk::~Chunk()
{
    //PRINT_FUNC();
    glDeleteBuffers(1, &m_vbo);
}

uint8_t Chunk::get(const Position3 &pos) const
{
    assert(pos.x < CX && pos.y < CY && pos.z < CZ);
    return m_blocks[pos.x][pos.y][pos.z];
}

void Chunk::set(const Position3 &pos, uint8_t type)
{
    assert(pos.x < CX && pos.y < CY && pos.z < CZ);
    m_blocks[pos.x][pos.y][pos.z] = type;
    m_changed = true;
}

void Chunk::update()
{
    m_changed = false;

    // fill VBO
    byte4 vertices[CX * CY * CZ * 6 * 6];
    int i = 0;

    for(auto x = 0; x < CX; x++)
    for(auto y = 0; y < CY; y++)
    {
        for(auto z = 0; z < CZ; z++)
        {
            Chunk *neighbour;
            uint8_t type = m_blocks[x][y][z];

            // Empty block?
            if(type == 0)
                continue;

            // View from negative x
            neighbour = m_parent.getChunk({m_pos.x - 1, m_pos.y, m_pos.z});

            if ((x > 0 && !m_blocks[x - 1][y][z]) ||                        // if previous block is empty
                (x > 0 && m_blocks[x - 1][y][z] & transp_bit) ||
                (x == 0 && neighbour && !neighbour->get({CX - 1, y, z})) ||   // if we're on the edge and adjacent block from neighbor chunk is empty
                (x == 0 && neighbour && neighbour->get({CX - 1, y, z}) & transp_bit) ||
                (x == 0 && !neighbour))                                     // if we're on the edge and neighbor _chunk_ is empty
            {
                vertices[i++] = byte4(x,     y,     z,     type & ~transp_bit);
                vertices[i++] = byte4(x,     y,     z + 1, type & ~transp_bit);
                vertices[i++] = byte4(x,     y + 1, z,     type & ~transp_bit);
                vertices[i++] = byte4(x,     y + 1, z,     type & ~transp_bit);
                vertices[i++] = byte4(x,     y,     z + 1, type & ~transp_bit);
                vertices[i++] = byte4(x,     y + 1, z + 1, type & ~transp_bit);
            }

            // View from positive x
            neighbour = m_parent.getChunk({m_pos.x + 1, m_pos.y, m_pos.z});

            if ((x < CX - 1 && !m_blocks[x + 1][y][z]) ||
                (x < CX - 1 && m_blocks[x + 1][y][z] & transp_bit) ||
                (x == CX - 1 && neighbour && !neighbour->get({0, y, z})) ||
                (x == CX - 1 && neighbour && neighbour->get({0, y, z}) & transp_bit) ||
                (x == CX - 1 && !neighbour))
            {
                vertices[i++] = byte4(x + 1, y,     z,     type & ~transp_bit);
                vertices[i++] = byte4(x + 1, y + 1, z,     type & ~transp_bit);
                vertices[i++] = byte4(x + 1, y,     z + 1, type & ~transp_bit);
                vertices[i++] = byte4(x + 1, y + 1, z,     type & ~transp_bit);
                vertices[i++] = byte4(x + 1, y + 1, z + 1, type & ~transp_bit);
                vertices[i++] = byte4(x + 1, y,     z + 1, type & ~transp_bit);
            }

            // View from negative y
            neighbour = m_parent.getChunk({m_pos.x, m_pos.y - 1, m_pos.z});

            if ((y > 0 && !m_blocks[x][y - 1][z]) ||
                (y > 0 && m_blocks[x][y - 1][z] & transp_bit) ||
                (y == 0 && neighbour && !neighbour->get({x, CY - 1, z})) ||
                (y == 0 && neighbour && neighbour->get({x, CY - 1, z}) & transp_bit) ||
                (y == 0 && !neighbour))
            {
                vertices[i++] = byte4(x,     y,     z,     -(type & ~transp_bit));  // negative values are used in fragment shader
                vertices[i++] = byte4(x + 1, y,     z,     -(type & ~transp_bit));  // for +y and -y faces, positive for 4 others
                vertices[i++] = byte4(x,     y,     z + 1, -(type & ~transp_bit));
                vertices[i++] = byte4(x,     y,     z + 1, -(type & ~transp_bit));
                vertices[i++] = byte4(x + 1, y,     z,     -(type & ~transp_bit));
                vertices[i++] = byte4(x + 1, y,     z + 1, -(type & ~transp_bit));
            }

            // View from positive y
            neighbour = m_parent.getChunk({m_pos.x, m_pos.y + 1, m_pos.z});

            if ((y < CY - 1 && !m_blocks[x][y + 1][z]) ||
                (y < CY - 1 && m_blocks[x][y + 1][z] & transp_bit) ||
                (y == CY - 1 && neighbour && !neighbour->get({x, 0, z})) ||
                (y == CY - 1 && neighbour && neighbour->get({x, 0, z}) & transp_bit) ||
                (y == CY - 1 && !neighbour))
            {
                vertices[i++] = byte4(x,     y + 1, z,     -(type & ~transp_bit));
                vertices[i++] = byte4(x,     y + 1, z + 1, -(type & ~transp_bit));
                vertices[i++] = byte4(x + 1, y + 1, z + 1, -(type & ~transp_bit));
                vertices[i++] = byte4(x + 1, y + 1, z + 1, -(type & ~transp_bit));
                vertices[i++] = byte4(x + 1, y + 1, z,     -(type & ~transp_bit));
                vertices[i++] = byte4(x,     y + 1, z,     -(type & ~transp_bit));
            }

            // View from negative z
            neighbour = m_parent.getChunk({m_pos.x, m_pos.y, m_pos.z - 1});

            if ((z > 0 && !m_blocks[x][y][z - 1]) ||
                (z > 0 && m_blocks[x][y][z - 1] & transp_bit) ||
                (z == 0 && neighbour && !neighbour->get({x, y, CZ - 1})) ||
                (z == 0 && neighbour && neighbour->get({x, y, CZ - 1}) & transp_bit) ||
                (z == 0 && !neighbour))
            {
                vertices[i++] = byte4(x,     y,     z,     type & ~transp_bit);
                vertices[i++] = byte4(x,     y + 1, z,     type & ~transp_bit);
                vertices[i++] = byte4(x + 1, y + 1, z,     type & ~transp_bit);
                vertices[i++] = byte4(x + 1, y + 1, z,     type & ~transp_bit);
                vertices[i++] = byte4(x + 1, y,     z,     type & ~transp_bit);
                vertices[i++] = byte4(x,     y,     z,     type & ~transp_bit);
            }

            // View from positive z
            neighbour = m_parent.getChunk({m_pos.x, m_pos.y, m_pos.z + 1});

            if ((z < CZ - 1 && !m_blocks[x][y][z + 1]) ||
                (z < CZ - 1 && m_blocks[x][y][z + 1] & transp_bit) ||
                (z == CZ - 1 && neighbour && !neighbour->get({x, y, 0})) ||
                (z == CZ - 1 && neighbour && neighbour->get({x, y, 0}) & transp_bit) ||
                (z == CZ - 1 && !neighbour))
            {
                vertices[i++] = byte4(x,     y,     z + 1, type & ~transp_bit);
                vertices[i++] = byte4(x + 1, y,     z + 1, type & ~transp_bit);
                vertices[i++] = byte4(x + 1, y + 1, z + 1, type & ~transp_bit);
                vertices[i++] = byte4(x + 1, y + 1, z + 1, type & ~transp_bit);
                vertices[i++] = byte4(x,     y + 1, z + 1, type & ~transp_bit);
                vertices[i++] = byte4(x,     y,     z + 1, type & ~transp_bit);
            }
        }
    }
    m_elements = i;

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_elements * sizeof *vertices, vertices, GL_STATIC_DRAW);
    //glVertexAttribPointer(0, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    Utils::glCheckError();
}

void Chunk::render()
{
    //PRINT_FUNC();
    if (m_changed)
        update();
    // render VBO
    if(!m_elements)
        return;

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(0, 4, GL_BYTE, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, m_elements);
}

const Position3& Chunk::getPosition() const
{
    return m_pos;
}
