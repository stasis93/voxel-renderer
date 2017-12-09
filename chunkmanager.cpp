#include "chunkmanager.h"

#include <memory>
//#include <iostream>
#include <algorithm>
#include <cstring>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "heightmapprovider.h"
#include "utils.h"
#include "frustrum.h"
#include "settings.h"


ChunkManager::ChunkManager(Frustrum& frustrum)
    : m_frustrum(frustrum)
    , m_config(Settings::get())
{
    m_loadRadius = m_config.rendering().loadRadius;
    fillLookupIndexBuffer();
    int minChunkColsLoaded = m_lookupIndexBuffer.size();
    if (m_config.world().maxChunkColsLoaded < minChunkColsLoaded)
        m_config.world().maxChunkColsLoaded = minChunkColsLoaded;
    m_renderList.reserve(minChunkColsLoaded);
    m_chunkColumns.reserve(m_config.world().maxChunkColsLoaded);
}

void ChunkManager::setTransform(const glm::mat4& transform)
{
    m_shader->use();
    m_shader->setMat4("proj_view", &transform[0][0]);
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
    for (const auto& i : m_lookupIndexBuffer)
        renderPositions.emplace_back(playerPosition.x / Blocks::CX + i.first, 0,
                                     playerPosition.z / Blocks::CZ + i.second);
    // fill render list (find in map or load if not present)
    m_chunkColsLoaded = 0;

    for (const Position3 &pos : renderPositions)
    {
        auto it = m_chunkColumns.find(pos);
        if (it != m_chunkColumns.end())
            m_renderList.emplace_back(&it->second);
        else
        {
            if (m_chunkColsLoaded == m_config.world().maxLoadsPerFrame)
                continue;

            ChunkColumn newColumn;
            newColumn.reserve(m_config.world().chunksInCol);

            // create CY_MAX chunks in new column
            for (auto y = 0; y < m_config.world().chunksInCol; y++)
                newColumn.emplace_back(this, Position3 {pos.x, y, pos.z});

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

            auto ins = m_chunkColumns.emplace(pos, std::move(newColumn));
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
//        std::cout << "Chunk columns loaded: " << m_chunkColumns.size() << std::endl;
//        std::cout << "Adjacent column updates queued: " << m_adjacentUpdateQueue.size() << std::endl;
    }
    unloadSpareChunkColumns();
    updateAdjacent();
}

void ChunkManager::updateAdjacent()
{
    int updated = 0;
    while (updated < m_config.world().maxExtraUpdatesPerFrame && !m_adjacentUpdateQueue.empty())
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
        if ((*columnToRender)[0].getIndex().x == pos.x &&
            (*columnToRender)[0].getIndex().z == pos.z)
            return false;
    }
    auto it = m_chunkColumns.find(pos);
    assert (it != m_chunkColumns.end());
    m_chunkColumns.erase(it);
    return true;
}

void ChunkManager::unloadSpareChunkColumns()
{
    while (m_chunkColumns.size() > (unsigned)m_config.world().maxChunkColsLoaded)
    {
        ChunkColumn *colToUnload = m_loadedQueue.front();
        const Position3& posToUnload = (*colToUnload)[0].getIndex();

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
    if(!(index.y >= 0 && index.y < m_config.world().chunksInCol))
        return nullptr;

    ChunkColumn *column = getColumn({index.x, 0, index.z});
    if (!column)
        return nullptr;
    return &(*column)[index.y];
}

ChunkColumn* ChunkManager::getColumn(const Position3& index)
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

    if (x < 0) {
        cx -= 1;
        x = Blocks::CX + x;
    }
    if (y < 0) {
        cy -= 1;
        y = Blocks::CY + y;
    }
    if (z < 0) {
        cz -= 1;
        z = Blocks::CZ + z;
    }
//    assert(cy >= 0 && cy < m_config.world().chunksInCol);
    assert(x >= 0 && x < Blocks::CX && y >= 0 && y < Blocks::CY && z >= 0 && z < Blocks::CZ);

    Chunk *ch = getChunk({cx, cy, cz});

    if (!ch)
        return (uint8_t)0;

    return ch->getRaw({x, y, z});
}

void ChunkManager::set(const Position3& pos, uint8_t type)
{
    // it's wrong for negative index... need fix
    int cx = pos.x / Blocks::CX,
        cy = pos.y / Blocks::CY,
        cz = pos.z / Blocks::CZ;

    int x = pos.x % Blocks::CX,
        y = pos.y % Blocks::CY,
        z = pos.z % Blocks::CZ;

    assert(cy >= 0 && cy < m_config.world().chunksInCol);

    Chunk *ch = getChunk({cx, cy, cz});

    if (!ch)
        return;

    ch->setRaw({x, y, z}, type);
}

void ChunkManager::render()
{
    m_texture->bind();
    m_shader->use();
    m_shader->setFloat("time", m_timer.getElapsedSecs());

    int chunksUpdated = 0;

    for (auto col : m_renderList)
    for (Chunk &chunk : *col)
    {
        if (chunk.empty())
            continue;

        auto p = chunk.getIndex();
        glm::vec3 min = glm::vec3{p.x * Blocks::CX, p.y * Blocks::CY, p.z * Blocks::CZ};
        glm::vec3 max = min + glm::vec3{Blocks::CX, Blocks::CY, Blocks::CZ};

        if (Frustrum::Outside == m_frustrum.checkBox({min, max}))
            continue;

        glm::mat4 model = glm::translate(glm::mat4(1), min);
        m_shader->setMat4("model", &model[0][0]);

        if (chunk.changed() && chunksUpdated < m_config.world().maxUpdatesPerFrame)
        {
            chunk.updateVBO();
            chunksUpdated++;
        }
        chunk.render();
    }
}

void ChunkManager::fillLookupIndexBuffer()
{ /* square spiral lookup */
    m_lookupIndexBuffer.clear();
    int radius = m_config.rendering().loadRadius;
    bool go = true;
    int x = 0, y = 0;
    int dir = 0, lineLen = 1, passesWithCurrentLen = 0;

    while (go)
    {
        int dx = 0, dy = 0;
        switch (dir)
        {
        case 0:
            dy = 1;
            break;
        case 1:
            dx = 1;
            break;
        case 2:
            dy = -1;
            break;
        case 3:
            dx = -1;
            break;
        default:
            break;
        }
        for (int l = 0; l < lineLen; l++)
        {
            m_lookupIndexBuffer.emplace_back(std::make_pair(x, y));
            x += dx; y += dy;
        }
        dir++;
        dir %= 4;
        passesWithCurrentLen++;
        if (passesWithCurrentLen == 2)
        {
            if (lineLen < 2 * radius)
                passesWithCurrentLen = 0;
            lineLen++;
        }
        if (passesWithCurrentLen == 3)
            go = false;
    }
}
