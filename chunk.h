#ifndef CHUNK_H_INCLUDED
#define CHUNK_H_INCLUDED

#include <cstdint>
#include <algorithm>
#include "constants.h"

namespace Blocks
{
constexpr int CX = 16, CY = 16, CZ = 16;
}


struct Position3
{
    Position3() = default;
    Position3(int x, int y, int z) : x(x), y(y), z(z) {}
    int x {0}, y {0}, z {0};
    friend bool operator<(const Position3 &p1, const Position3 &p2);
    bool operator==(const Position3 &other) const;
};


class ChunkManager;

struct Chunk
{
                    Chunk(ChunkManager &manager, Position3 index);
                    ~Chunk();

    uint8_t         get(const Position3 &pos) const;
    void            set(const Position3 &pos, uint8_t type);
    void            updateVBO();
    void            render();
    const Position3& getPosition() const;
    bool            empty();
    bool            changed();


    static char     transp_bit;

private:
    bool            m_changed {false};
    bool            m_empty {true};
    uint8_t         m_blocks[Blocks::CX][Blocks::CY][Blocks::CZ];
    unsigned int    m_vao, m_vbo;
    int             m_elements {0};
    ChunkManager    &m_parent;
    Position3       m_pos;
};

#endif // CHUNK_H_INCLUDED
