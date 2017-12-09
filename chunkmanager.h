#ifndef SUPERCHUNK_H_INCLUDED
#define SUPERCHUNK_H_INCLUDED

#include <unordered_map>
#include <vector>
#include <queue>
#include <glm/mat4x4.hpp>

#include "chunk.h"
#include "timer.h"
#include "renderable.h"


using ChunkColumn = std::vector<Chunk>;
using ChunkColumnMap = std::unordered_map<Position3, ChunkColumn>;

class Frustrum;
class Settings;

struct ChunkManager : public Renderable, public WithTexture, public WithShader, Transformable
{
    ChunkManager(Frustrum& frustrum);

    uint8_t         get(const Position3& pos);
    void            set(const Position3& pos, uint8_t type);

    void            update(const Position3 &playerPosition);
    void            render();

    void            setTransform(const glm::mat4& transform);

    Chunk*          getChunk(const Position3& index);

private:
    ChunkColumn*    getColumn(const Position3 &index);
    bool            tryUnloadAtPosition(const Position3 &pos);
    void            unloadSpareChunkColumns();
    void            updateAdjacent();

    void            fillLookupIndexBuffer();

private:
    ChunkColumnMap              m_chunkColumns;
    std::vector<ChunkColumn*>   m_renderList;
    std::queue<ChunkColumn*>    m_loadedQueue;
    std::queue<Position3>       m_adjacentUpdateQueue;
    int                         m_loadRadius,
                                m_chunkColsLoaded {0};
    Position3                   m_oldPlayerPos;

    bool                        m_loadingDone {false};
    Frustrum&                   m_frustrum;
    Settings&                   m_config;
    Timer                       m_timer;

    std::vector<std::pair<int, int>> m_lookupIndexBuffer;
};

#endif // SUPERCHUNK_H_INCLUDED
