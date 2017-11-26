#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include <noise/noise.h>

class NoiseGenerator
{
public:
    NoiseGenerator(double flatTerrainFreq = 2.0,
                     double flatTerrainScale = 0.125,
                     double flatTerrainBias = -0.75,
                     double selectorFreq = 0.5,
                     double selectorPersistence = 0.25,
                     double edgeFalloff = 0.07);

    double getValue2d(double x, double z) const;
    void setSeed(int seed);

private:
    void apply();

private:
    noise::module::Perlin       m_terrainType;
    noise::module::Billow       m_baseFlatTerrain;
    noise::module::ScaleBias    m_flatTerrain;
    noise::module::RidgedMulti  m_mountainTerrain;
    noise::module::Select       m_finalTerrain;

    double m_flatFreq,
           m_flatScale,
           m_flatBias,
           m_selFreq,
           m_selPersist,
           m_edgeFalloff;
};

#endif // TERRAINGENERATOR_H
