#ifndef HEIGHTMAPPROVIDER_H_INCLUDED
#define HEIGHTMAPPROVIDER_H_INCLUDED

#include <vector>

class Chunk;

namespace HeightMapProvider
{
void init(int seed = 0);
void fillChunkColumn(std::vector<Chunk>& column);
}

#endif // HEIGHTMAPPROVIDER_H_INCLUDED
