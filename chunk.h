#ifndef CHUNK_H_INCLUDED
#define CHUNK_H_INCLUDED

#include <cstdint>
#include "constants.h"

constexpr int CX = 16, CY = 16, CZ = 16;
//extern const char transp_bit;

class Superchunk;

struct Chunk
{
                    Chunk(Superchunk *parent, int x = 0, int y = 0, int z = 0);
                    ~Chunk();

    uint8_t         get(unsigned int x, unsigned int y, unsigned int z) const;
    void            set(unsigned int x, unsigned int y, unsigned int z, uint8_t type);
    void            update();
    void            render();
    static int      trianglesAdded;
    static char     transp_bit;

private:
    bool            m_changed {false};
    uint8_t         m_blocks[CX][CY][CZ];
    unsigned int    m_vbo;
    int             m_elements {0};
    Superchunk*     m_parent;
    int             m_x, m_y, m_z;
};

#endif // CHUNK_H_INCLUDED
