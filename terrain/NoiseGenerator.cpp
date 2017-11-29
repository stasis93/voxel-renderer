#include "noisegenerator.h"

NoiseGenerator::NoiseGenerator(double flatTerrainFreq,
                                   double flatTerrainScale,
                                   double flatTerrainBias,
                                   double selectorFreq,
                                   double selectorPersistence,
                                   double edgeFalloff)
    : m_flatFreq(flatTerrainFreq)
    , m_flatScale(flatTerrainScale)
    , m_flatBias(flatTerrainBias)
    , m_selFreq(selectorFreq)
    , m_selPersist(selectorPersistence)
    , m_edgeFalloff(edgeFalloff)
{
    apply();
}

void NoiseGenerator::setSeed(int seed)
{
    m_mountainTerrain.SetSeed(seed);
    m_baseFlatTerrain.SetSeed(seed);
}

void NoiseGenerator::apply()
{
    // libnoise mix of flat terrain and mountain terrain.
    // Billow and RidgedMulti are 2 sources,
    // ScaleBias is for downscaling Billow output,
    // Select is a control module for selecting output from one of sources
    //   based on Perlin output

    m_baseFlatTerrain.SetFrequency(2.0);

    m_flatTerrain.SetSourceModule(0, m_baseFlatTerrain);
    m_flatTerrain.SetScale(m_flatScale);
    m_flatTerrain.SetBias(m_flatBias);

    m_terrainType.SetFrequency(m_selFreq);
    m_terrainType.SetPersistence(m_selPersist);

    m_finalTerrain.SetSourceModule(0, m_flatTerrain);
    m_finalTerrain.SetSourceModule(1, m_mountainTerrain);
    m_finalTerrain.SetControlModule(m_terrainType);
    m_finalTerrain.SetBounds(0.0, 1.0);
    m_finalTerrain.SetEdgeFalloff(m_edgeFalloff);
}

double NoiseGenerator::getValue2d(double x, double z) const
{
    return m_finalTerrain.GetValue(x, 0, z);
}
