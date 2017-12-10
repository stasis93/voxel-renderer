#ifndef COLLISION_H_INCLUDED
#define COLLISION_H_INCLUDED

#include <vector>
#include "geometry.h"

using namespace Geom;

namespace Collision
{

class Packet
{
public:
    void setBasePosition(const glm::vec3& basePoint);
    void setVelocity(const glm::vec3& velocity);
    glm::vec3 testAgainstTriangles(const std::vector<Triangle>& trianglesToTest);

private:
    void checkTriangle(const Triangle& triangle);
    void checkTriangles(const std::vector<Triangle>& triangles);

private:
    bool m_foundCollision {false};
    float m_nearestDistance;
    glm::vec3 m_intersectionPoint,
              m_velocity,
              m_basePoint,
              m_finalPosition;
};

}
#endif // COLLISION_H_INCLUDED
