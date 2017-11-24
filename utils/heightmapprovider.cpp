#include "heightmapprovider.h"
#include "noiseutils.h"
#include <noise/noise.h>
#include "../chunk.h"

namespace HeightMapProvider
{
noise::module::Perlin perlin;
noise::utils::NoiseMap nm;
noise::utils::NoiseMapBuilderPlane nmBuilder;

double winSz = 0.075;

void init(int seed)
{
    perlin.SetSeed(seed);
    perlin.SetOctaveCount(3);
    nmBuilder.SetSourceModule(perlin);
    nmBuilder.SetDestNoiseMap(nm);
    nmBuilder.SetDestSize(Blocks::CX, Blocks::CZ);
}

static uint8_t chooseBlock(float c);

void fillChunkColumn(std::vector<Chunk> &column)
{
    auto colPos = column[0].getPosition();
    int ix = colPos.x * Blocks::CX / 16.0f,
        iz = colPos.z * Blocks::CZ / 16.0f;
    int y_max = column.size() * Blocks::CY;

    nmBuilder.SetBounds(ix * winSz, (ix + 1) * winSz, iz * winSz, (iz + 1) * winSz);
    nmBuilder.Build();

    for (int z = 0; z < Blocks::CZ; z++)
    {
        const float *row = nm.GetConstSlabPtr(z);
        for (int x = 0; x < Blocks::CX; x++)
        {
            int val = (row[x] + 1.0f) * y_max / 2.0f; // map from [-1, 1] to [0, y_max]
            val = val < 1 ? 1 : val > y_max ? y_max : val;

            for (int y = 0; y < val; y++)
            {
                int chunk_y = y / Blocks::CY;
                Chunk &chunk = column[chunk_y];
                chunk.set({x, y % Blocks::CY, z}, 4);
            }
            // set top-block's type
            Chunk &chunk = column[(val - 1) / Blocks::CY];
            chunk.set({x, (val - 1) % Blocks::CY, z}, chooseBlock((float)val / (float)y_max));
        }
    }
}

static uint8_t chooseBlock(float c)
{
    float grass1 = 0.3f,
          grass2 = 0.5f,
          stone  = 0.75f,
          snow   = 0.9f;

    uint8_t block = 5;
    if (c < grass1) block = 1;
    else if (c < grass2) block = 2;
    else if (c < stone) block = 4;
    else if (c < snow) block = 5;
    return block;
}
}
