#include "superchunk.h"
#include <memory>
#include <iostream>
#include <cstring>
#include <thread>
#include <mingw.thread.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "shader.h"
#include "heightmapprovider.h"
#include "utils.h"

int ChunkManager::MAX_CHUNKS_LOADED = CY_MAX * 1000;
int ChunkManager::MAX_CHUNKS_PER_FRAME = CY_MAX * 1;

ChunkManager::ChunkManager(Shader& shader)
    : m_shader(shader)
{
}

void ChunkManager::update(const Position3& playerPosition)
{
    m_renderList.clear();

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

    m_chunksLoaded = 0;

    for (const Position3 &pos : renderPositions)
    {
        auto it = m_chunkColumns.find(pos);
        if (it != m_chunkColumns.end())
            m_renderList.emplace_back(&it->second);
        else
        {
            if (m_chunksLoaded == MAX_CHUNKS_PER_FRAME)
                continue;

            ChunkColumn newColumn;
            newColumn.reserve(CY_MAX);

            for (auto y = 0; y < CY_MAX; y++)
                newColumn.emplace_back(*this, Position3 {pos.x, y, pos.z});


            HeightMapProvider::fillChunkColumn(newColumn);

            auto ins = m_chunkColumns.emplace(std::make_pair(pos, std::move(newColumn)));
            assert(ins.second);
            auto colPtr = &ins.first->second;
            m_renderList.emplace_back(colPtr);
            m_chunksLoaded += CY_MAX;
        }
    }
}

bool ChunkManager::tryUnloadAtPosition(const Position3& pos)
{
    // TODO
}

void ChunkManager::unloadSpareChunkColumns()
{
    // TODO
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

void ChunkManager::render()
{
    using namespace Blocks;
    m_shader.use();
    glm::mat4 ident{1};

    for (auto col : m_renderList)
    for (Chunk &chunk : *col)
    {
        auto p = chunk.getPosition();
        glm::mat4 model = glm::translate(glm::mat4(1),
        glm::vec3(p.x * Blocks::CX, p.y * Blocks::CY, p.z * Blocks::CZ));

        glUniformMatrix4fv(glGetUniformLocation(m_shader.id(), "model"),
                            1, GL_FALSE, glm::value_ptr(model));

        chunk.render();
    }
}

