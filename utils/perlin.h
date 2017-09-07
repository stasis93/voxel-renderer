#ifndef PERLIN_H_INCLUDED
#define PERLIN_H_INCLUDED

#include <cstring>
#include <vector>

using PerlinData = std::vector<std::vector<float>>;

struct Vector2
{
    Vector2(){};
    Vector2(float x, float y)
        : x(x), y(y){}

    float x {0};
    float y {0};
};

class PerlinNoiseGenerator
{
public:
                        PerlinNoiseGenerator() = default;

    const PerlinData&   generate(unsigned int height, unsigned int width,
                                float min, float max, unsigned int grid_size_x, unsigned int grid_size_y);

    const PerlinData&   getData() const;

private:
    void                initGradientGrid();
    static float        lerp(float a0, float a1, float w);
    float               dotGridGradient(unsigned int ix, unsigned int iy, float x, float y);
    float               perlin(float x, float y);
    void                scaleResult();

private:
    PerlinData          m_data;

    std::vector<std::vector<Vector2>>   m_grid;

    float               m_min {0}, m_max {0};
    float               m_min_gen {0}, m_max_gen {0};
};

#endif // PERLIN_H_INCLUDED
