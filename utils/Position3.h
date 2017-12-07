#ifndef POSITION3_H
#define POSITION3_H

#include <functional>

template <typename T>
struct Position3_
{
    Position3_() = default;
    Position3_(T x, T y, T z) : x(x), y(y), z(z) {}
    T x {(T)0}, y {(T)0}, z {(T)0};
    bool operator==(const Position3_ &other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

using Position3  = Position3_<int>;
using Position3f = Position3_<float>;

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
