#ifndef CHUNK_H_INCLUDED
#define CHUNK_H_INCLUDED

#include <cstdint>
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
};

struct Position2
{
    Position2() = default;
    Position2(int x, int y) : x(x), y(y) {}
    int x {0}, y {0};
    friend bool operator<(const Position2 &p1, const Position2 &p2);
};

struct Chunk;

//struct ChunkNeighbours
//{
//    Chunk *x_neg {nullptr}, *x_pos {nullptr};
//    Chunk *y_neg {nullptr}, *y_pos {nullptr};
//    Chunk *z_neg {nullptr}, *z_pos {nullptr};
//};

class ChunkManager;

struct Chunk
{
                    Chunk(ChunkManager &manager, Position3 index);
                    ~Chunk();

    uint8_t         get(const Position3 &pos) const;
    void            set(const Position3 &pos, uint8_t type);
    void            update();
    void            render();
    const Position3& getPosition() const;


    static char     transp_bit;

private:
    bool            m_changed {false};
    uint8_t         m_blocks[Blocks::CX][Blocks::CY][Blocks::CZ];
    unsigned int    m_vbo;
    int             m_elements {0};
    ChunkManager    &m_parent;
//    ChunkNeighbours m_nbs;
    Position3       m_pos;
};

#endif // CHUNK_H_INCLUDED
