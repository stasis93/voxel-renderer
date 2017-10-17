#include "superchunk.h"
#include <memory>
#include <iostream>
#include <cstring>
#include <thread>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "shader.h"
#include "heightmapprovider.h"
#include "utils.h"

int ChunkManager::MAX_CHUNK_COLUMNS_LOADED;
int ChunkManager::MAX_CHUNK_COLS_PER_FRAME = 2;
int ChunkManager::MAX_UPDATES_PER_FRAME = 10;
int ChunkManager::MAX_EXTRA_UPDATES_PER_FRAME = 1;

ChunkManager::ChunkManager(Shader& shader)
    : m_loadRadius(15)
    , m_shader(shader)
{
    m_renderList.reserve(m_loadRadius * m_loadRadius);
    MAX_CHUNK_COLUMNS_LOADED = 5 * m_loadRadius * m_loadRadius;
}

void ChunkManager::update(const Position3 &playerPosition)
{
    // if position is same and nothing to load, let's re-update chunks to remove extra vertices between chunks
    if (playerPosition.x == m_oldPlayerPos.x &&
        playerPosition.z == m_oldPlayerPos.z &&
        m_loadingDone)
    {
        updateAdjacent();
        return;
    }

    m_loadingDone = false;
    m_oldPlayerPos = playerPosition;

    m_renderList.clear();

    // get positions for chunk column _potential_ rendering
    std::vector<Position3> renderPositions;

    int x0 = playerPosition.x / Blocks::CX - m_loadRadius;
    int z0 = playerPosition.z / Blocks::CZ - m_loadRadius;

    for (int x = x0; x <= x0 + 2 * m_loadRadius; x++)
    {
        int dx = x - playerPosition.x / Blocks::CX;
        for (int z = z0; z <= z0 + 2 * m_loadRadius; z++)
        {
            int dz = z - playerPosition.z / Blocks::CZ;
            if (dz * dz + dx * dx <= m_loadRadius * m_loadRadius)
                renderPositions.emplace_back(x, 0, z);
        }
    }

    // fill render list (find in map or load if not present)
    m_chunkColsLoaded = 0;

    for (const Position3 &pos : renderPositions)
    {
        auto it = m_chunkColumns.find(pos);
        if (it != m_chunkColumns.end())
            m_renderList.emplace_back(&it->second);
        else
        {
            if (m_chunkColsLoaded == MAX_CHUNK_COLS_PER_FRAME)
                continue;

            ChunkColumn newColumn;
            newColumn.reserve(CY_MAX);

            // create CY_MAX chunks in new column
            for (auto y = 0; y < CY_MAX; y++)
                newColumn.emplace_back(*this, Position3 {pos.x, y, pos.z});

            // Queue an Update of 4 adjacent chunks in XZ plane if they exist already for all chunks in created column
            if (getChunk(Position3 {pos.x - 1, 0, pos.z}))
                m_adjacentUpdateQueue.emplace(Position3 {pos.x - 1, 0, pos.z});
            if (getChunk(Position3 {pos.x + 1, 0, pos.z}))
                m_adjacentUpdateQueue.emplace(Position3 {pos.x + 1, 0, pos.z});
            if (getChunk(Position3 {pos.x, 0, pos.z - 1}))
                m_adjacentUpdateQueue.emplace(Position3 {pos.x, 0, pos.z - 1});
            if (getChunk(Position3 {pos.x, 0, pos.z + 1}))
                m_adjacentUpdateQueue.emplace(Position3 {pos.x, 0, pos.z + 1});

            HeightMapProvider::fillChunkColumn(newColumn);

            auto ins = m_chunkColumns.emplace(std::make_pair(pos, std::move(newColumn)));
            assert(ins.second);

            auto colPtr = &ins.first->second;
            m_renderList.emplace_back(colPtr);
            m_loadedQueue.emplace(colPtr);

            m_chunkColsLoaded++;
        }
    }

    if (m_chunkColsLoaded == 0)
    {
        m_loadingDone = true;
        std::cout << "Chunk columns held in std::map: " << m_chunkColumns.size() << std::endl;
        std::cout << "Adjacent column updates queued: " << m_adjacentUpdateQueue.size() << std::endl;
    }
    unloadSpareChunkColumns();
    updateAdjacent();
}

void ChunkManager::updateAdjacent()
{
    int updated = 0;
    while (updated < MAX_EXTRA_UPDATES_PER_FRAME && !m_adjacentUpdateQueue.empty())
    {
        Position3 &pos = m_adjacentUpdateQueue.front();
        ChunkColumn *col = getColumn(pos);

        if (col)
        {
            for (Chunk &c : *col)
            c.update();
            updated++;
        }
        m_adjacentUpdateQueue.pop();
    }
}

bool ChunkManager::tryUnloadAtPosition(const Position3& pos)
{
    for (const ChunkColumn *columnToRender : m_renderList)
    {
        if ((*columnToRender)[0].getPosition().x == pos.x &&
            (*columnToRender)[0].getPosition().z == pos.z)
            return false;
    }
    auto it = m_chunkColumns.find(pos);
    assert (it != m_chunkColumns.end());
    m_chunkColumns.erase(it);
    return true;
}

void ChunkManager::unloadSpareChunkColumns()
{
    while (m_chunkColumns.size() > (unsigned int)MAX_CHUNK_COLUMNS_LOADED)
    {
        ChunkColumn *colToUnload = m_loadedQueue.front();
        const Position3& posToUnload = (*colToUnload)[0].getPosition();

        if (tryUnloadAtPosition(posToUnload))
        {
            m_loadedQueue.pop();
        }
        else
        {
            m_loadedQueue.pop();
            m_loadedQueue.emplace(colToUnload);
        }
    }
}

Chunk* ChunkManager::getChunk(const Position3& index)
{
    if(!(index.y >= 0 && index.y < CY_MAX))
        return nullptr;

    ChunkColumn *column = getColumn({index.x, 0, index.z});
    if (!column)
        return nullptr;
    return &(*column)[index.y];
}

ChunkColumn* ChunkManager::getColumn(const Position3 &index)
{
    auto it = m_chunkColumns.find(index);

    if (it == m_chunkColumns.end())
        return nullptr;

    return &it->second;
}

uint8_t ChunkManager::get(const Position3& pos)
{
    int cx = pos.x / Blocks::CX,
        cy = pos.y / Blocks::CY,
        cz = pos.z / Blocks::CZ;

    int x = pos.x % Blocks::CX,
        y = pos.y % Blocks::CY,
        z = pos.z % Blocks::CZ;

    assert(cy >= 0 && cy < CY_MAX);

    Chunk *ch = getChunk({cx, cy, cz});

    if (!ch)
        return (uint8_t)0;

    return ch->get({x, y, z});
}

void ChunkManager::set(const Position3& pos, uint8_t type)
{
    int cx = pos.x / Blocks::CX,
        cy = pos.y / Blocks::CY,
        cz = pos.z / Blocks::CZ;

    int x = pos.x % Blocks::CX,
        y = pos.y % Blocks::CY,
        z = pos.z % Blocks::CZ;

    assert(cy >= 0 && cy < CY_MAX);

    Chunk *ch = getChunk({cx, cy, cz});

    if (!ch)
        return;

    ch->set({x, y, z}, type);
}

void ChunkManager::render(const glm::mat4 &proj_view)
{
    int chunksRendered = 0;
    m_shader.use();
//    glm::vec4 chunkCenter;
    int chunksUpdated = 0;

    for (auto col : m_renderList)
    for (Chunk &chunk : *col)
    {
        if (chunk.empty())
            continue;

        auto p = chunk.getPosition();
        glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(p.x * Blocks::CX, p.y * Blocks::CY, p.z * Blocks::CZ));

        ///TODO: Frustrum culling

        // culling (https://en.wikibooks.org/wiki/OpenGL_Programming/Glescraft_5)
        // fails if move far from origin. Need to find better way

//        constexpr static float diam = std::sqrt(Blocks::CX * Blocks::CX + Blocks::CY * Blocks::CY + Blocks::CZ * Blocks::CZ);
//        //constexpr static float diam = std::sqrt(1.0f + 1.0f + 1.0f);
//
//        chunkCenter = glm::vec4{p.x + 0.5f, p.y + 0.5f, p.z + 0.5f, 1.0f};
//        chunkCenter = proj_view * model * chunkCenter;
//        chunkCenter.x /= chunkCenter.w;
//        chunkCenter.y /= chunkCenter.w;
//
//        float d = diam / std::fabs(chunkCenter.w);
//
//        if (chunkCenter.z < -diam)
//            continue;
//
//        if (std::fabs(chunkCenter.x) > d + 1.0f || std::fabs(chunkCenter.y) > d + 1.0f)
//            continue;

        glUniformMatrix4fv(glGetUniformLocation(m_shader.id(), "model"),
                            1, GL_FALSE, glm::value_ptr(model));

        if (chunk.changed() && chunksUpdated < MAX_UPDATES_PER_FRAME)
        {
            chunk.render();
            chunksRendered++;
            chunksUpdated++;
        } else
        if (!chunk.changed())
        {
            chunk.render();
            chunksRendered++;
        }
    }
//    std::cout << "Chunks rendered: " << chunksRendered << std::endl;
}

