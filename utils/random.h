#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED

#include <random>
#include <ctime>

namespace Random
{
    void init(int seed = 0);
    int intInRange(int min = 0, int max = 255);
    float floatInRange(float min = 0.0f, float max = 1.0f);
}

#endif // RANDOM_H_INCLUDED
