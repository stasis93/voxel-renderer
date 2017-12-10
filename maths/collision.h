#ifndef COLLISION_H_INCLUDED
#define COLLISION_H_INCLUDED

#include <vector>
#include "geometry.h"

using namespace Geom;

namespace Collision
{

struct Packet
{
    glm::vec3 velocity,                 // in
              basePoint,                // in
              finalPosition;            // out

private:
    bool foundCollision {false};
    float nearestDistance;
    glm::vec3 intersectionPoint;

    friend void collide(Packet& packet, const std::vector<Triangle>& triangles);
    friend void checkTriangle(Packet& packet, const Triangle& triangle);
    friend void checkTriangles(Packet& packet, const std::vector<Triangle>& triangles);
};


void collide(Packet& packet, const std::vector<Triangle>& triangles);

}

#endif // COLLISION_H_INCLUDED
