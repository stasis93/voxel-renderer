#ifndef SUPERCHUNK_H_INCLUDED
#define SUPERCHUNK_H_INCLUDED

#include "chunk.h"
#include <unordered_map>
#include <vector>
#include <queue>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

constexpr int CY_MAX = 6;

using ChunkColumn = std::vector<Chunk>;
using ChunkColumnMap = std::unordered_map<Position3, ChunkColumn>;

class Frustrum;

namespace std
{
template<>
struct hash<Position3>
{
    size_t operator()(const Position3 &pos) const
    {
        // http://stackoverflow.com/a/1646913/126995
        size_t res = 17;
        res = res * 31 + std::hash<int>()(pos.x);
        res = res * 31 + std::hash<int>()(pos.y);
        res = res * 31 + std::hash<int>()(pos.z);
        return res;
    }
};
}


class Shader;

struct ChunkManager
{
    ChunkManager(Shader& shader);

    uint8_t         get(const Position3& pos);
    void            set(const Position3& pos, uint8_t type);

    void            update(const Position3 &playerPosition);
    void            render(const glm::mat4 &proj_view);
    void            setFrustrum(const Frustrum& frustrum);

    Chunk*          getChunk(const Position3& index);


private:
    ChunkColumn*    getColumn(const Position3 &index);
    bool            tryUnloadAtPosition(const Position3 &pos);
    void            unloadSpareChunkColumns();
    void            updateAdjacent();


    static int      MAX_CHUNK_COLUMNS_LOADED;
    static int      MAX_CHUNK_COLS_PER_FRAME;
    static int      MAX_UPDATES_PER_FRAME;
    static int      MAX_EXTRA_UPDATES_PER_FRAME;

private:
    ChunkColumnMap              m_chunkColumns;
    std::vector<ChunkColumn*>   m_renderList;
    std::queue<ChunkColumn*>    m_loadedQueue;
    std::queue<Position3>       m_adjacentUpdateQueue;
    int                         m_loadRadius,
                                m_chunkColsLoaded {0};
    Position3                   m_oldPlayerPos;

    Shader                      &m_shader;

    bool                        m_loadingDone {false};
    const Frustrum*             m_frustrum {nullptr};
};

#endif // SUPERCHUNK_H_INCLUDED
