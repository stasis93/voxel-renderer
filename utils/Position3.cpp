#include "Position3.h"

bool Position3::operator==(const Position3 &other) const
{
    return x == other.x && y == other.y && z == other.z;
}
