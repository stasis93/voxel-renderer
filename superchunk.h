#ifndef SUPERCHUNK_H_INCLUDED
#define SUPERCHUNK_H_INCLUDED

#include "chunk.h"
#include <map>
#include <vector>

constexpr int CY_MAX = 4;

using ChunkColumn = std::vector<Chunk>;
using ChunkColumnMap = std::map<Position3, ChunkColumn>;

class Shader;

struct ChunkManager
{
    ChunkManager(Shader& shader);

    uint8_t         get(const Position3& pos);
    void            set(const Position3& pos, uint8_t type);

    void            update(const Position3 &playerPosition);
    void            render();

    Chunk*          getChunk(const Position3& index);


private:
    ChunkColumn*    getColumn(const Position3 &index);
    bool            tryUnloadAtPosition(const Position3 &pos);
    void            unloadSpareChunkColumns();

    static int      MAX_CHUNKS_LOADED;
    static int      MAX_CHUNKS_PER_FRAME;

private:
    ChunkColumnMap              m_chunkColumns;
    std::vector<ChunkColumn*>   m_renderList;
    int                         m_loadRadius {30},
                                m_chunksLoaded {0};

    Shader                      &m_shader;


    // TODO: unloading chunks if loaded count > MAX_CHUNKS_LOADED
};

#endif // SUPERCHUNK_H_INCLUDED
