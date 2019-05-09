#include "chunk.h"
#include "chunkmanager.h"
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <cassert>

#include "utils/utils.h"
#include "utils/drawcalltrack.h"

using byte4 = glm::tvec4<GLbyte>;

using namespace Blocks;

static inline bool isTransparent(uint8_t block)
{
    return block == static_cast<uint8_t>(Blocks::Type::None) ||
           block == static_cast<uint8_t>(Blocks::Type::Water) ||
           block == static_cast<uint8_t>(Blocks::Type::Glass);
}
static inline bool isWater(uint8_t block)
{
    return block == static_cast<uint8_t>(Blocks::Type::Water);
}

Chunk::Chunk(ChunkManager* manager, Position3 index)
    : m_parent(manager), m_pos(index)
{
    memset(m_blocks, 0, sizeof m_blocks);
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
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
    return static_cast<Blocks::Type>(m_blocks[pos.x][pos.y][pos.z]);
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

    for (auto x = 0; x < CX; x++)
    for (auto y = 0; y < CY; y++)
    for (auto z = 0; z < CZ; z++)
    {
        uint8_t type = m_blocks[x][y][z];

        if (type == static_cast<uint8_t>(Blocks::Type::None))
            continue;
        // View from negative x
        if (shouldDrawFace({x, y, z}, Face::NegX))
        {
            vertices[i++] = byte4(x,     y,     z,     type);
            vertices[i++] = byte4(x,     y,     z + 1, type);
            vertices[i++] = byte4(x,     y + 1, z,     type);
            vertices[i++] = byte4(x,     y + 1, z,     type);
            vertices[i++] = byte4(x,     y,     z + 1, type);
            vertices[i++] = byte4(x,     y + 1, z + 1, type);
        }
        // View from positive x
        if (shouldDrawFace({x, y, z}, Face::PosX))
        {
            vertices[i++] = byte4(x + 1, y,     z,     type);
            vertices[i++] = byte4(x + 1, y + 1, z,     type);
            vertices[i++] = byte4(x + 1, y,     z + 1, type);
            vertices[i++] = byte4(x + 1, y + 1, z,     type);
            vertices[i++] = byte4(x + 1, y + 1, z + 1, type);
            vertices[i++] = byte4(x + 1, y,     z + 1, type);
        }
        // View from negative y
        if (shouldDrawFace({x, y, z}, Face::NegY))
        {
            vertices[i++] = byte4(x,     y,     z,     -type);  // negative values are used in fragment shader to obtain texture coords
            vertices[i++] = byte4(x + 1, y,     z,     -type);  // for +y and -y faces, positive for 4 others
            vertices[i++] = byte4(x,     y,     z + 1, -type);
            vertices[i++] = byte4(x,     y,     z + 1, -type);
            vertices[i++] = byte4(x + 1, y,     z,     -type);
            vertices[i++] = byte4(x + 1, y,     z + 1, -type);
        }
        // View from positive y
        if (shouldDrawFace({x, y, z}, Face::PosY))
        {
            vertices[i++] = byte4(x,     y + 1, z,     -type);
            vertices[i++] = byte4(x,     y + 1, z + 1, -type);
            vertices[i++] = byte4(x + 1, y + 1, z + 1, -type);
            vertices[i++] = byte4(x + 1, y + 1, z + 1, -type);
            vertices[i++] = byte4(x + 1, y + 1, z,     -type);
            vertices[i++] = byte4(x,     y + 1, z,     -type);
        }
        // View from negative z
        if (shouldDrawFace({x, y, z}, Face::NegZ))
        {
            vertices[i++] = byte4(x,     y,     z,     type);
            vertices[i++] = byte4(x,     y + 1, z,     type);
            vertices[i++] = byte4(x + 1, y + 1, z,     type);
            vertices[i++] = byte4(x + 1, y + 1, z,     type);
            vertices[i++] = byte4(x + 1, y,     z,     type);
            vertices[i++] = byte4(x,     y,     z,     type);
        }
        // View from positive z
        if (shouldDrawFace({x, y, z}, Face::PosZ))
        {
            vertices[i++] = byte4(x,     y,     z + 1, type);
            vertices[i++] = byte4(x + 1, y,     z + 1, type);
            vertices[i++] = byte4(x + 1, y + 1, z + 1, type);
            vertices[i++] = byte4(x + 1, y + 1, z + 1, type);
            vertices[i++] = byte4(x,     y + 1, z + 1, type);
            vertices[i++] = byte4(x,     y,     z + 1, type);
        }
    }
    m_elements = i;
    if (m_elements > 0)
    {
        m_empty = false;
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_elements * sizeof *vertices, vertices, GL_STATIC_DRAW);
    }
    else
        m_empty = true;
    m_changed = false;
}

void Chunk::render()
{
    if(!m_elements)
        return;

    glBindVertexArray(m_vao);
    glDrawArrays_(GL_TRIANGLES, 0, m_elements);
}

const Position3& Chunk::getIndex() const
{
    return m_pos;
}

bool Chunk::shouldDrawFace(Position3 p, Face face)
{
    auto neighbour = getAdjacentBlock(p, face);
    auto current = m_blocks[p.x][p.y][p.z];

    if (isWater(current) && isWater(neighbour))
        return false;
    if (isTransparent(neighbour))
        return true;
    return false;
}

uint8_t Chunk::getAdjacentBlock(Position3 p, Face face)
{
    auto block = static_cast<uint8_t>(Type::None);
    Chunk* n = nullptr;

    switch (face)
    {
    case Face::NegX:
        if (p.x == 0)
        {
            n = m_parent->getChunk({m_pos.x - 1, m_pos.y, m_pos.z});
            if (n)
                block = n->getRaw({CX - 1, p.y, p.z});
        }
        else
            block = m_blocks[p.x - 1][p.y][p.z];
        break;

    case Face::PosX:
        if (p.x == CX - 1)
        {
            n = m_parent->getChunk({m_pos.x + 1, m_pos.y, m_pos.z});
            if (n)
                block = n->getRaw({0, p.y, p.z});
        }
        else
            block = m_blocks[p.x + 1][p.y][p.z];
        break;

    case Face::NegY:
        if (p.y == 0)
        {
            n = m_parent->getChunk({m_pos.x, m_pos.y - 1, m_pos.z});
            if (n)
                block = n->getRaw({p.x, CY - 1, p.z});
        }
        else
            block = m_blocks[p.x][p.y - 1][p.z];
        break;

    case Face::PosY:
        if (p.y == CY - 1)
        {
            n = m_parent->getChunk({m_pos.x, m_pos.y + 1, m_pos.z});
            if (n)
                block = n->getRaw({p.x, 0, p.z});
        }
        else
            block = m_blocks[p.x][p.y + 1][p.z];
        break;

    case Face::NegZ:
        if (p.z == 0)
        {
            n = m_parent->getChunk({m_pos.x, m_pos.y, m_pos.z - 1});
            if (n)
                block = n->getRaw({p.x, p.y, CZ - 1});
        }
        else
            block = m_blocks[p.x][p.y][p.z - 1];
        break;

    case Face::PosZ:
        if (p.z == CZ - 1)
        {
            n = m_parent->getChunk({m_pos.x, m_pos.y, m_pos.z + 1});
            if (n)
                block = n->getRaw({p.x, p.y, 0});
        }
        else
            block = m_blocks[p.x][p.y][p.z + 1];
        break;

    default:
        break;
    }
    return block;
}
