#include "chunk.h"
#include "superchunk.h"
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <cassert>
#include "utils.h"

/*
    bool            m_changed {false};
    uint8_t         m_blocks[CX][CY][CZ];
    unsigned int    m_vbo;
    int             m_elements;
*/

//const char transp_bit = 0x40;
char Chunk::transp_bit = 0x40;

using byte4 = glm::tvec4<GLbyte>;

int Chunk::trianglesAdded = 0;

Chunk::Chunk(Superchunk *parent, int x, int y, int z)
    : m_parent(parent), m_x(x), m_y(y), m_z(z)
{
    //PRINT_FUNC();
    assert(m_parent);
    memset(m_blocks, 0, sizeof(m_blocks));
    glGenBuffers(1, &m_vbo);
}

Chunk::~Chunk()
{
    //PRINT_FUNC();
    glDeleteBuffers(1, &m_vbo);
}

uint8_t Chunk::get(unsigned int x, unsigned int y, unsigned int z) const
{
    assert(x < CX && y < CY && z < CZ);
    return m_blocks[x][y][z];
}

void Chunk::set(unsigned int x, unsigned int y, unsigned int z, uint8_t type)
{
    assert(x < CX && y < CY && z < CZ);
    m_blocks[x][y][z] = type;
    m_changed = true;
}

void Chunk::update()
{
    //PRINT_FUNC(); std::cout << m_x << " " << m_y << " " << m_z << std::endl;

    Chunk::trianglesAdded -= m_elements;
    m_changed = false;

    // fill VBO
    byte4 vertices[CX * CY * CZ * 6 * 6];
    int i = 0;

    for(auto x = 0u; x < CX; x++)
    for(auto y = 0u; y < CY; y++)
    {
        //bool visible { false }; // if previous voxel was visible
        for(auto z = 0u; z < CZ; z++)
        {
    //        std::cout << x << " " << y << " " << z << std::endl;
            Chunk *neighbour;
            uint8_t type = m_blocks[x][y][z];

            // Empty block?
            if(type == 0)
            {
                //visible = false;
                continue;
            }

            // View from negative x
            neighbour = m_x > 0 ? m_parent->getChunk(m_x - 1, m_y, m_z) : nullptr;

            if ((x > 0 && !m_blocks[x - 1][y][z]) ||                        // if previous block is empty
                (x > 0 && m_blocks[x - 1][y][z] & transp_bit) ||
                (x == 0 && neighbour && !neighbour->get(CX - 1, y, z)) ||   // if we're on the edge and adjacent block from neighbor chunk is empty
                (x == 0 && neighbour && neighbour->get(CX - 1, y, z) & transp_bit) ||
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
            neighbour = m_x < SCX - 1 ? m_parent->getChunk(m_x + 1, m_y, m_z) : nullptr;

            if ((x < CX - 1 && !m_blocks[x + 1][y][z]) ||
                (x < CX - 1 && m_blocks[x + 1][y][z] & transp_bit) ||
                (x == CX - 1 && neighbour && !neighbour->get(0, y, z)) ||
                (x == CX - 1 && neighbour && neighbour->get(0, y, z) & transp_bit) ||
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
            neighbour = m_y > 0 ? m_parent->getChunk(m_x, m_y - 1, m_z) : nullptr;

            if ((y > 0 && !m_blocks[x][y - 1][z]) ||
                (y > 0 && m_blocks[x][y - 1][z] & transp_bit) ||
                (y == 0 && neighbour && !neighbour->get(x, CY - 1, z)) ||
                (y == 0 && neighbour && neighbour->get(x, CY - 1, z) & transp_bit) ||
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
            neighbour = m_y < SCY - 1 ? m_parent->getChunk(m_x, m_y + 1, m_z) : nullptr;

            if ((y < CY - 1 && !m_blocks[x][y + 1][z]) ||
                (y < CY - 1 && m_blocks[x][y + 1][z] & transp_bit) ||
                (y == CY - 1 && neighbour && !neighbour->get(x, 0, z)) ||
                (y == CY - 1 && neighbour && neighbour->get(x, 0, z) & transp_bit) ||
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
            neighbour = m_z > 0 ? m_parent->getChunk(m_x, m_y, m_z - 1) : nullptr;

            if ((z > 0 && !m_blocks[x][y][z - 1]) ||
                (z > 0 && m_blocks[x][y][z - 1] & transp_bit) ||
                (z == 0 && neighbour && !neighbour->get(x, y, CZ - 1)) ||
                (z == 0 && neighbour && neighbour->get(x, y, CZ - 1) & transp_bit) ||
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
            neighbour = m_z < SCZ - 1 ? m_parent->getChunk(m_x, m_y, m_z + 1) : nullptr;

            if ((z < CZ - 1 && !m_blocks[x][y][z + 1]) ||
                (z < CZ - 1 && m_blocks[x][y][z + 1] & transp_bit) ||
                (z == CZ - 1 && neighbour && !neighbour->get(x, y, 0)) ||
                (z == CZ - 1 && neighbour && neighbour->get(x, y, 0) & transp_bit) ||
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

    Chunk::trianglesAdded += m_elements / 3;
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
