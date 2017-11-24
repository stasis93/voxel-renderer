#include "superchunk.h"

#include <memory>
#include <iostream>
#include <cstring>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "heightmapprovider.h"
#include "utils.h"
#include "frustrum.h"
#include "Settings.h"

ChunkManager::ChunkManager(Frustrum& frustrum)
    : m_frustrum(frustrum)
    , m_config(Settings::get())
{
    m_loadRadius = m_config.rendering().loadRadius;
    int minChunkColsLoaded = m_loadRadius * m_loadRadius * 4;
    if (m_config.rendering().maxChunkColsLoaded < minChunkColsLoaded)
        m_config.rendering().maxChunkColsLoaded = minChunkColsLoaded;
    m_renderList.reserve(minChunkColsLoaded);
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
    //std::cout << m_renderList.size() << std::endl;
    // fill render list (find in map or load if not present)
    m_chunkColsLoaded = 0;

    for (const Position3 &pos : renderPositions)
    {
        auto it = m_chunkColumns.find(pos);
        if (it != m_chunkColumns.end())
            m_renderList.emplace_back(&it->second);
        else
        {
            if (m_chunkColsLoaded == m_config.rendering().maxLoadsPerFrame)
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
        std::cout << "Chunk columns loaded: " << m_chunkColumns.size() << std::endl;
        std::cout << "Adjacent column updates queued: " << m_adjacentUpdateQueue.size() << std::endl;
    }
    unloadSpareChunkColumns();
    updateAdjacent();
}

void ChunkManager::updateAdjacent()
{
    int updated = 0;
    while (updated < m_config.rendering().maxExtraUpdatesPerFrame && !m_adjacentUpdateQueue.empty())
    {
        Position3 &pos = m_adjacentUpdateQueue.front();
        ChunkColumn *col = getColumn(pos);

        if (col)
        {
            for (Chunk &c : *col)
            c.updateVBO();
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
    while (m_chunkColumns.size() > (unsigned)m_config.rendering().maxChunkColsLoaded)
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

void ChunkManager::render(const glm::mat4& proj_view)
{
    m_shader.use();
    m_shader.setMat4("proj_view", glm::value_ptr(proj_view));

    int chunksUpdated = 0;

    for (auto col : m_renderList)
    for (Chunk &chunk : *col)
    {
        if (chunk.empty())
            continue;

        auto p = chunk.getPosition();
        if (m_frustrum)
        {
            constexpr static float chunkRad = std::sqrt(Blocks::CX * Blocks::CX + Blocks::CY * Blocks::CY + Blocks::CZ * Blocks::CZ) / 2.0f;
            glm::vec3 chunkCenter{(p.x + 0.5f) * Blocks::CX, (p.y + 0.5f) * Blocks::CY, (p.z + 0.5f) * Blocks::CZ};
            if (Frustrum::Outside == m_frustrum->checkSphere(chunkCenter, chunkRad))
                continue;
        }

        glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(p.x * Blocks::CX, p.y * Blocks::CY, p.z * Blocks::CZ));
        m_shader.setMat4("model", glm::value_ptr(model));

        if (chunk.changed() && chunksUpdated < MAX_UPDATES_PER_FRAME)
        {
            chunk.updateVBO();
            chunksUpdated++;
        }
        chunk.render();
    }
}

void ChunkManager::setFrustrum(const Frustrum& frustrum)
{
    m_frustrum = &frustrum;
}
