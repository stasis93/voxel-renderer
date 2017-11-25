#ifndef POSITION3_H
#define POSITION3_H

#include <functional>

struct Position3
{
    Position3() = default;
    Position3(int x, int y, int z) : x(x), y(y), z(z) {}
    int x {0}, y {0}, z {0};
    bool operator==(const Position3 &other) const;
};

namespace std
{
template<>
struct hash<Position3>
{
    size_t operator()(const Position3 &pos) const
    {
        // http://stackoverflow.com/a/1646913/126995
//        size_t res = 17;
//        res = res * 31 + std::hash<int>()(pos.x);
//        res = res * 31 + std::hash<int>()(pos.y);
//        res = res * 31 + std::hash<int>()(pos.z);
        int16_t x = static_cast<int16_t>(pos.x);
        int16_t z = static_cast<int16_t>(pos.z);
        return x ^ (z << 16);
    }
};
}



#endif // POSITION3_H
