#include "chunk.h"
#include "chunkmanager.h"
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <cassert>

#include "utils.h"

/* used in updateVBO (don't skip cube face if neighbor block has this bit set to 1) */
char const transp_bit = 0b10000000;

using byte4 = glm::tvec4<GLbyte>;

using namespace Blocks;

Chunk::Chunk(ChunkManager* manager, Position3 index)
    : m_parent(manager), m_pos(index)
{
    memset(m_blocks, 0, sizeof m_blocks);
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, CX * CY * CZ * 6 * 6, nullptr, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_BYTE, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

Chunk::~Chunk()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

bool Chunk::empty()
{
    return m_empty;
}

bool Chunk::changed()
{
    return m_changed;
}

Blocks::Type Chunk::get(const Position3 &pos) const
{
    assert(pos.x < CX && pos.y < CY && pos.z < CZ);
    return static_cast<Blocks::Type>(m_blocks[pos.x][pos.y][pos.z] & ~transp_bit);
}

uint8_t Chunk::getRaw(const Position3 &pos) const
{
    assert(pos.x < CX && pos.y < CY && pos.z < CZ);
    return m_blocks[pos.x][pos.y][pos.z];
}

void Chunk::set(const Position3 &pos, Blocks::Type type)
{
    assert(pos.x < CX && pos.y < CY && pos.z < CZ);
    m_blocks[pos.x][pos.y][pos.z] = static_cast<uint8_t>(type);
    m_changed = true;
    if (type != Blocks::Type::None)
        m_empty = false;
}

void Chunk::setRaw(const Position3& pos, uint8_t type)
{
    assert(pos.x < CX && pos.y < CY && pos.z < CZ);
    m_blocks[pos.x][pos.y][pos.z] = type;
    m_changed = true;
    if (type)
        m_empty = false;
}

void Chunk::updateVBO()
{
    byte4 vertices[CX * CY * CZ * 6 * 6];
    int i = 0;

    for(auto x = 0; x < CX; x++)
    for(auto y = 0; y < CY; y++)
    for(auto z = 0; z < CZ; z++)
    {
        Chunk *neighbour;
        uint8_t type = m_blocks[x][y][z];

        // Empty block?
        if (type == static_cast<uint8_t>(Blocks::Type::None))
            continue;
        if (type == static_cast<uint8_t>(Blocks::Type::Glass) ||
            type == static_cast<uint8_t>(Blocks::Type::Water))
            m_blocks[x][y][z] |= transp_bit;

        uint8_t type_ = type & ~transp_bit;

        // View from positive y
        neighbour = m_parent->getChunk({m_pos.x, m_pos.y + 1, m_pos.z});

        if ((y < CY - 1 && !m_blocks[x][y + 1][z]) ||
            (y < CY - 1 && m_blocks[x][y + 1][z] & transp_bit) ||
            (y == CY - 1 && neighbour && !neighbour->getRaw({x, 0, z})) ||
            (y == CY - 1 && neighbour && neighbour->getRaw({x, 0, z}) & transp_bit) ||
            (y == CY - 1 && !neighbour))
        {
            vertices[i++] = byte4(x,     y + 1, z,     -type_);
            vertices[i++] = byte4(x,     y + 1, z + 1, -type_);
            vertices[i++] = byte4(x + 1, y + 1, z + 1, -type_);
            vertices[i++] = byte4(x + 1, y + 1, z + 1, -type_);
            vertices[i++] = byte4(x + 1, y + 1, z,     -type_);
            vertices[i++] = byte4(x,     y + 1, z,     -type_);
        }
        if (type_ == static_cast<uint8_t>(Blocks::Type::Water))
            continue;

        // View from negative y
        neighbour = m_parent->getChunk({m_pos.x, m_pos.y - 1, m_pos.z});

        if ((y > 0 && !m_blocks[x][y - 1][z]) ||
            (y > 0 && m_blocks[x][y - 1][z] & transp_bit) ||
            (y == 0 && neighbour && !neighbour->getRaw({x, CY - 1, z})) ||
            (y == 0 && neighbour && neighbour->getRaw({x, CY - 1, z}) & transp_bit) ||
            (y == 0 && !neighbour))
        {
            vertices[i++] = byte4(x,     y,     z,     -type_);  // negative values are used in fragment shader
            vertices[i++] = byte4(x + 1, y,     z,     -type_);  // for +y and -y faces, positive for 4 others
            vertices[i++] = byte4(x,     y,     z + 1, -type_);
            vertices[i++] = byte4(x,     y,     z + 1, -type_);
            vertices[i++] = byte4(x + 1, y,     z,     -type_);
            vertices[i++] = byte4(x + 1, y,     z + 1, -type_);
        }

        // View from negative x
        neighbour = m_parent->getChunk({m_pos.x - 1, m_pos.y, m_pos.z});

        if ((x > 0 && !m_blocks[x - 1][y][z]) ||                        // if previous block is empty
            (x > 0 && m_blocks[x - 1][y][z] & transp_bit) ||
            (x == 0 && neighbour && neighbour->get({CX - 1, y, z}) == Blocks::Type::Water) ||
            (x == 0 && neighbour && !neighbour->getRaw({CX - 1, y, z})) ||   // if we're on the edge and adjacent block from neighbor chunk is empty
            (x == 0 && neighbour && neighbour->getRaw({CX - 1, y, z}) & transp_bit) ||
            (x == 0 && !neighbour))                                     // if we're on the edge and neighbor _chunk_ is empty
        {
            vertices[i++] = byte4(x,     y,     z,     type_);
            vertices[i++] = byte4(x,     y,     z + 1, type_);
            vertices[i++] = byte4(x,     y + 1, z,     type_);
            vertices[i++] = byte4(x,     y + 1, z,     type_);
            vertices[i++] = byte4(x,     y,     z + 1, type_);
            vertices[i++] = byte4(x,     y + 1, z + 1, type_);
        }

        // View from positive x
        neighbour = m_parent->getChunk({m_pos.x + 1, m_pos.y, m_pos.z});

        if ((x < CX - 1 && !m_blocks[x + 1][y][z]) ||
            (x < CX - 1 && m_blocks[x + 1][y][z] & transp_bit) ||
            (x == CX - 1 && neighbour && neighbour->get({0, y, z}) == Blocks::Type::Water) ||
            (x == CX - 1 && neighbour && !neighbour->getRaw({0, y, z})) ||
            (x == CX - 1 && neighbour && neighbour->getRaw({0, y, z}) & transp_bit) ||
            (x == CX - 1 && !neighbour))
        {
            vertices[i++] = byte4(x + 1, y,     z,     type_);
            vertices[i++] = byte4(x + 1, y + 1, z,     type_);
            vertices[i++] = byte4(x + 1, y,     z + 1, type_);
            vertices[i++] = byte4(x + 1, y + 1, z,     type_);
            vertices[i++] = byte4(x + 1, y + 1, z + 1, type_);
            vertices[i++] = byte4(x + 1, y,     z + 1, type_);
        }

        // View from negative z
        neighbour = m_parent->getChunk({m_pos.x, m_pos.y, m_pos.z - 1});

        if ((z > 0 && !m_blocks[x][y][z - 1]) ||
            (z > 0 && m_blocks[x][y][z - 1] & transp_bit) ||
            (z == 0 && neighbour && neighbour->get({x, y, CZ - 1}) == Blocks::Type::Water) ||
            (z == 0 && neighbour && !neighbour->getRaw({x, y, CZ - 1})) ||
            (z == 0 && neighbour && neighbour->getRaw({x, y, CZ - 1}) & transp_bit) ||
            (z == 0 && !neighbour))
        {
            vertices[i++] = byte4(x,     y,     z,     type_);
            vertices[i++] = byte4(x,     y + 1, z,     type_);
            vertices[i++] = byte4(x + 1, y + 1, z,     type_);
            vertices[i++] = byte4(x + 1, y + 1, z,     type_);
            vertices[i++] = byte4(x + 1, y,     z,     type_);
            vertices[i++] = byte4(x,     y,     z,     type_);
        }

        // View from positive z
        neighbour = m_parent->getChunk({m_pos.x, m_pos.y, m_pos.z + 1});

        if ((z < CZ - 1 && !m_blocks[x][y][z + 1]) ||
            (z < CZ - 1 && m_blocks[x][y][z + 1] & transp_bit) ||
            (z == CZ - 1 && neighbour && neighbour->get({x, y, 0}) == Blocks::Type::Water) ||
            (z == CZ - 1 && neighbour && !neighbour->getRaw({x, y, 0})) ||
            (z == CZ - 1 && neighbour && neighbour->getRaw({x, y, 0}) & transp_bit) ||
            (z == CZ - 1 && !neighbour))
        {
            vertices[i++] = byte4(x,     y,     z + 1, type_);
            vertices[i++] = byte4(x + 1, y,     z + 1, type_);
            vertices[i++] = byte4(x + 1, y + 1, z + 1, type_);
            vertices[i++] = byte4(x + 1, y + 1, z + 1, type_);
            vertices[i++] = byte4(x,     y + 1, z + 1, type_);
            vertices[i++] = byte4(x,     y,     z + 1, type_);
        }
    }

    m_elements = i;
    if (m_elements > 0)
    {
        m_empty = false;
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_elements * sizeof *vertices, vertices);
        Utils::glCheckError();
    }
    m_changed = false;
}

void Chunk::render()
{
    if(!m_elements)
        return;

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_elements);
}

const Position3& Chunk::getPosition() const
{
    return m_pos;
}
