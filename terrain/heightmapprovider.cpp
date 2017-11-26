#include "heightmapprovider.h"
#include "NoiseGenerator.h"
#include "random.h"
#include "../chunk.h"

#include <iostream>
namespace HeightMapProvider
{
NoiseGenerator noiseGenerator;

double winSz = 0.075;

void init(int seed)
{
    noiseGenerator.setSeed(seed);
}

static Blocks::Type chooseBlock(float c);

void fillChunkColumn(std::vector<Chunk> &column)
{
    const Position3& colPos = column[0].getPosition();

    int ix = colPos.x * Blocks::CX / 16.0f,
        iz = colPos.z * Blocks::CZ / 16.0f,
        y_max = column.size() * Blocks::CY;

    for (int z = 0; z < Blocks::CZ; z++)
    for (int x = 0; x < Blocks::CX; x++)
    {
        // calculate noise 'window' location depending on column position
        double noiseX = (x / (double)Blocks::CX + ix) * winSz;
        double noiseZ = (z / (double)Blocks::CZ + iz) * winSz;
        double noiseVal = noiseGenerator.getValue2d(noiseX, noiseZ);

        int val = (noiseVal + 1.0f) * y_max / 2.25f; // map from [-1, 1] to [0, y_max]
        val = val < 1 ? 1 : val > y_max ? y_max : val;

        for (int y = 0; y < val; y++)
        {
            int chunk_y = y / Blocks::CY;
            Chunk &chunk = column[chunk_y];
            chunk.set({x, y % Blocks::CY, z}, Blocks::Type::Stone);
        }

        // set top-block's type
        Chunk &chunk = column[(val - 1) / Blocks::CY];

        auto turbulence = noiseGenerator.getValue2d(noiseX * 5, noiseZ * 5) / 5;
        auto blockType = chooseBlock(val / (float)y_max + turbulence);
        chunk.set({x, (val - 1) % Blocks::CY, z}, blockType);
    }
}

static Blocks::Type chooseBlock(float c)
{
    float sand   = 0.05f,
          grass1 = 0.3f,
          grass2 = 0.5f,
          stone  = 0.75f,
          snow   = 0.95f;

    auto block = Blocks::Type::Stone;

    if (c < sand) block = Blocks::Type::Sand;
    else if (c < grass1) block = Blocks::Type::Grass1;
    else if (c < grass2) block = Blocks::Type::Grass2;
    else if (c < stone) block = Blocks::Type::Stone;
    else if (c < snow) block = Blocks::Type::Snow;
    return block;
}
}
