#ifndef SUPERCHUNK_H_INCLUDED
#define SUPERCHUNK_H_INCLUDED

#include "chunk.h"


constexpr int SCX = 32, SCY = 12, SCZ = 32;

class Shader;

struct Superchunk
{
    Superchunk(Shader& shader);
    ~Superchunk();

    uint8_t         get(unsigned int x, unsigned int y, unsigned int z) const;
    void            set(unsigned int x, unsigned int y, unsigned int z, uint8_t type);
    void            render();
    Chunk*          getChunk(unsigned int cx, unsigned int cy, unsigned int cz);

private:
    Chunk           *m_chunks[SCX][SCY][SCZ];
    Shader          &m_shader;
};

#endif // SUPERCHUNK_H_INCLUDED
