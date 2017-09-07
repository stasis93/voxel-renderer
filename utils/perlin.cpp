#include "perlin.h"
#include "random.h"
#include <iostream>

#define PRINT_FUNC() std::cout << __FUNCTION__ << std::endl;

const PerlinData& PerlinNoiseGenerator::generate(unsigned int height, unsigned int width,
                                    float min, float max,
                                    unsigned int grid_size_x, unsigned int grid_size_y)
{
    PRINT_FUNC();

    m_grid.reserve(grid_size_y + 1);
    m_grid.resize(grid_size_y + 1);

    for (auto &row : m_grid)
    {
        row.reserve(grid_size_x + 1);
        row.resize(grid_size_x + 1);
    }

    //m_data.allocate(height, width);
    m_data.reserve(height);
    m_data.resize(height);
    for (auto &row : m_data)
    {
        row.reserve(width);
        row.resize(width);
    }

    m_min = min;
    m_max = max;

    initGradientGrid();

    //auto data_ptr = m_data.getDataPtr();

    float step_x = (float)grid_size_x / width;
    float step_y = (float)grid_size_y / height;

    std::cout << "stepy=" << step_y << " stepx=" << step_x << std::endl;

    for (auto y = 0u; y < height; y++)
    for (auto x = 0u; x < width; x++)
    {
        //std::cout << "Y=" << y << " X=" << x << std::endl;

        float n_y = y * step_y;
        float n_x = x * step_x;
        float val = perlin(n_x, n_y);
        if (val < m_min_gen) m_min_gen = val;
        if (val > m_max_gen) m_max_gen = val;
        m_data[y][x] = val;
    }
    scaleResult();
    return m_data;
}

const PerlinData& PerlinNoiseGenerator::getData() const
{
    return m_data;
}

void PerlinNoiseGenerator::initGradientGrid()
{
    PRINT_FUNC();

    for (auto y = 0u; y < m_grid.size(); y++)
    for (auto x = 0u; x < m_grid[0].size(); x++)
    {
        Vector2 &v = m_grid[y][x];

        v.x = Random::floatInRange(0.0f, 1.0f);
        v.y = Random::floatInRange(0.0f, 1.0f);

        auto mod = std::sqrt(v.x * v.x + v.y * v.y);
        v.x /= mod;
        v.y /= mod;
    }
}

float PerlinNoiseGenerator::lerp(float a0, float a1, float w)
{
    //w = (3 * w * w) - (2 * w * w * w);
    return w * (a1 - a0) + a0;
}

float PerlinNoiseGenerator::perlin(float x, float y)
{
    int x0 = std::floor(x);
    int x1 = x0 + 1;
    int y0 = std::floor(y);
    int y1 = y0 + 1;

    float wx = x - (float)x0;
    float wy = y - (float)y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;
    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = lerp(n0, n1, wx);
    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = lerp(n0, n1, wx);
    value = lerp(ix0, ix1, wy);

    return value;
}

float PerlinNoiseGenerator::dotGridGradient(unsigned int ix, unsigned int iy, float x, float y)
{
    //assert(ix < m_grid[0].size() && iy < m_grid.size());
    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return dx * m_grid[iy][ix].x + dy * m_grid[iy][ix].y;
}

void PerlinNoiseGenerator::scaleResult()
{
    PRINT_FUNC();

    float d = m_max_gen - m_min_gen;
    float range = m_max - m_min;

    std::cout << "Generated range: [" << m_min_gen << ", " << m_max_gen << "]\nRequested range: [" << m_min << ", " << m_max << "]" << std::endl;

    float k = (float)range / d;
    float avg_gen = (m_max_gen + m_min_gen) / 2.0f;
    float avg_rqs = (m_min + m_max) / 2.0f;
    float bias = avg_rqs - avg_gen * k;

    std::cout << "Scaling coeff: " << k << "\nBias: " << bias << std::endl;

    for (auto y = 0u; y < m_data.size(); y++)
    for (auto x = 0u; x < m_data[0].size(); x++)
    {
        float new_val = m_data[y][x] * k + bias;
        if (new_val < m_min) new_val = m_min;
        if (new_val > m_max) new_val = m_max;
        m_data[y][x] = new_val;
    }
}

