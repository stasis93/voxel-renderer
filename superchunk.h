#ifndef SUPERCHUNK_H_INCLUDED
#define SUPERCHUNK_H_INCLUDED

#include <unordered_map>
#include <vector>
#include <memory>
#include <queue>
#include <glm/mat4x4.hpp>

#include "chunk.h"
#include "shader.h"
#include "Texture.h"

constexpr int CY_MAX = 6;

using ChunkColumn = std::vector<Chunk>;
using ChunkColumnMap = std::unordered_map<Position3, ChunkColumn>;

class Frustrum;
class Settings;

namespace std
{
template<>
struct hash<Position3>
{
    size_t operator()(const Position3 &pos) const
    {
        // http://stackoverflow.com/a/1646913/126995
//        size_t res = 17;
//        res = res * 31 + std::hash<int>()(pos.x);
//        res = res * 31 + std::hash<int>()(pos.y);
//        res = res * 31 + std::hash<int>()(pos.z);
        int16_t x = static_cast<int16_t>(pos.x);
        int16_t z = static_cast<int16_t>(pos.z);
        return x ^ (z << 16);
    }
};
}


class Shader;

struct ChunkManager
{
    ChunkManager(Frustrum& frustrum);

    uint8_t         get(const Position3& pos);
    void            set(const Position3& pos, uint8_t type);

    void            update(const Position3 &playerPosition);
    void            render(const glm::mat4 &proj_view);

    void            setShader(std::unique_ptr<Shader> pShader);
    void            setBlockTexture(std::unique_ptr<Texture> pTexture);

    Chunk*          getChunk(const Position3& index);

private:
    ChunkColumn*    getColumn(const Position3 &index);
    bool            tryUnloadAtPosition(const Position3 &pos);
    void            unloadSpareChunkColumns();
    void            updateAdjacent();

private:
    ChunkColumnMap              m_chunkColumns;
    std::vector<ChunkColumn*>   m_renderList;
    std::queue<ChunkColumn*>    m_loadedQueue;
    std::queue<Position3>       m_adjacentUpdateQueue;
    int                         m_loadRadius,
                                m_chunkColsLoaded {0};
    Position3                   m_oldPlayerPos;

    std::unique_ptr<Shader>     m_shader {nullptr};
    std::unique_ptr<Texture>    m_blockTexture {nullptr};

    bool                        m_loadingDone {false};
    Frustrum&                   m_frustrum;
    Settings&                   m_config;
};

#endif // SUPERCHUNK_H_INCLUDED
