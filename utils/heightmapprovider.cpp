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

void fillChunkColumn(std::vector<Chunk> &column)
{
    auto colPos = column[0].getPosition();
    int ix = colPos.x,
        iz = colPos.z;
    int y_max = column.size() * Blocks::CY;

    nmBuilder.SetBounds(ix * winSz, (ix + 1) * winSz, iz * winSz, (iz + 1) * winSz);
    nmBuilder.Build();

    for (int z = 0; z < Blocks::CZ; z++)
    {

        const float *row = nm.GetConstSlabPtr(z);
        for (int x = 0; x < Blocks::CX; x++)
        {

            int val = (row[x] + 1.0f) * y_max / 2.0f;
            val = val < 1 ? 1 : val > y_max ? y_max : val;

            for (int y = 0; y < val; y++)
            {
                int chunk_y = y / Blocks::CY;
                Chunk &chunk = column[chunk_y];
                chunk.set({x, y % Blocks::CY, z}, 4);
            }
        }
    }
}
}
