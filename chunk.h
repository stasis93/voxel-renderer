#ifndef CHUNK_H_INCLUDED
#define CHUNK_H_INCLUDED

#include <cstdint>
#include "constants.h"
#include "position3.h"

namespace Blocks
{
constexpr int CX = 16, CY = 16, CZ = 16;
enum class Type
{
    None = 0,
    Grass1,
    Grass2,
    Glass,
    Stone,
    Snow,
    Sand
};
}

class ChunkManager;

struct Chunk
{
                    Chunk(ChunkManager* manager, Position3 index);
                    ~Chunk();

    Blocks::Type    get(const Position3 &pos) const;
    uint8_t         getRaw(const Position3 &pos) const;
    void            set(const Position3 &pos, Blocks::Type type);
    void            setRaw(const Position3 &pos, uint8_t type);
    void            updateVBO();
    void            render();
    const Position3& getPosition() const;
    bool            empty();
    bool            changed();

private:
    bool            m_changed {false};
    bool            m_empty {true};
    uint8_t         m_blocks[Blocks::CX][Blocks::CY][Blocks::CZ];
    unsigned int    m_vao, m_vbo;
    int             m_elements {0};
    ChunkManager*   m_parent;
    Position3       m_pos;
};

#endif // CHUNK_H_INCLUDED
