#include "collision.h"
#include <cmath>
#include <iostream>

constexpr float Eps = 1e-3f;

namespace Collision
{

inline bool isInRange(float val, float min, float max)
{
    return val >= min && val <= max;
}

inline bool isZero(float val, float eps = Eps)
{
    return (val > -eps && val < eps);
}

/*
    Used Kasper Fauerby's algorithm "Improved Collision Detection and Response"
*/
void checkTriangle(Packet& packet, const Triangle& triangle)
{
    Plane plane = planeFromTriangle(triangle);

    if (!isPlaneFrontFacingToVec(plane, packet.velocity))
        return;

    float t0, t1;
    bool embedded = false;

    float distance = distanceToPlane(packet.basePoint, plane);
    float n_dot_vel = glm::dot(plane.norm, packet.velocity);

    if (isZero(n_dot_vel))
    {
        if (std::abs(distance) >= 1.0f)
            return;
        embedded = true;
        t0 = 0.0f;
        t1 = 1.0f;
    }
    else {
        t0 = ( 1 - distance) / n_dot_vel;
        t1 = (-1 - distance) / n_dot_vel;
        if (t0 > t1)
            std::swap(t0, t1);
        if (!isInRange(t0, 0.0f, 1.0f) && !isInRange(t1, 0.0f, 1.0f) && (std::abs(distance) >= 1.0f))
            return;
        // Clamp to [0, 1]
        t0 = t0 < 0 ? 0 : t0 > 1 ? 1 : t0;
        t1 = t1 < 0 ? 0 : t1 > 1 ? 1 : t1;
    }

    // Check against the inside of triangle.
    // If intersect, no need for further testing
    glm::vec3 collisionPoint;
    bool foundCollision = false;
    float t = 1.0f;

    if (!embedded)
    {
        glm::vec3 planeIntersectionPoint = packet.basePoint -
        plane.norm + t0 * packet.velocity;

        if (isPointInTriangle(planeIntersectionPoint, triangle))
        {
            foundCollision = true;
            t = t0;
            collisionPoint = planeIntersectionPoint;
        }
    }
    if (!foundCollision)
    {
        // Check against triangle vertices
        auto& vel = packet.velocity;
        auto& base = packet.basePoint;
        float vel_lenSquared = squaredLength(vel);

        std::pair<float, float> roots;
        float a = vel_lenSquared;
        float b, c;

        for (const auto& p : {triangle.p1, triangle.p2, triangle.p3})
        {
            b = glm::dot(vel, base - p) * 2;
            c = squaredLength(p - base) - 1;

            if (solveQuadEquation(a, b, c, roots) && (roots.first > 0 && roots.first < t))
            {
                t = roots.first;
                foundCollision = true;
                collisionPoint = p;
            }
        }
        // Check against triangle edges
        for (const Edge& edge : {Edge {triangle.p1, triangle.p2},
                                 Edge {triangle.p2, triangle.p3},
                                 Edge {triangle.p3, triangle.p1}})
        {
            auto edgeVec = edge.point2 - edge.point1;
            auto baseToVertex = edge.point1 - base;
            float edgeVec_lenSquared = squaredLength(edgeVec);
            float edgeVec_dot_vel = glm::dot(edgeVec, vel);
            float edgeVec_dot_baseToVertex = glm::dot(edgeVec, baseToVertex);

            a = edgeVec_lenSquared * -vel_lenSquared +
                edgeVec_dot_vel * edgeVec_dot_vel;

            b = edgeVec_lenSquared * 2 * glm::dot(vel, baseToVertex) -
                2 * edgeVec_dot_vel * edgeVec_dot_baseToVertex;

            c = edgeVec_lenSquared * (1 - squaredLength(baseToVertex)) +
                edgeVec_dot_baseToVertex * edgeVec_dot_baseToVertex;

            if (solveQuadEquation(a, b, c, roots) && (roots.first > 0 && roots.first < t))
            {
                // Check if intersection is within line segment
                float f = (edgeVec_dot_vel * roots.first -
                           edgeVec_dot_baseToVertex) /
                           edgeVec_lenSquared;
                if (isInRange(f, 0, 1))
                {
                    t = roots.first;
                    foundCollision = true;
                    collisionPoint = edge.point1 + f * edgeVec;
                }
            }
        }
    } // end if (!foundCollision)
    if (foundCollision)
    {
//        std::cout << "  t = " << t << std::endl;
        float distToCollision = t * glm::length(packet.velocity);
        if (!packet.foundCollision || distToCollision < packet.nearestDistance)
        {
            packet.nearestDistance = distToCollision;
            packet.intersectionPoint = collisionPoint;
            packet.foundCollision = true;
        }
    }
} // end checkTriangle

void checkTriangles(Packet& packet, const std::vector<Triangle>& triangles)
{
    packet.foundCollision = false;
    packet.nearestDistance = glm::length(packet.velocity);

    for (const Triangle& t : triangles)
        checkTriangle(packet, t);

    if (!packet.foundCollision)
        return;

    auto velocityNormalized = glm::normalize(packet.velocity);
    auto baseAtIntersect = packet.basePoint + velocityNormalized * packet.nearestDistance;
    auto normal = baseAtIntersect - packet.intersectionPoint;

    Plane slidingPlane(normal, packet.intersectionPoint);

    auto plannedPosition = packet.basePoint + packet.velocity;
    auto plannedPositionProj = projectPointToPlane(plannedPosition, slidingPlane);
    auto slidingVel = plannedPositionProj - packet.intersectionPoint;

    packet.velocity = slidingVel;
    packet.basePoint = baseAtIntersect;
}

void collide(Packet& packet, const std::vector<Triangle>& triangles)
{
    int depth = 0;
    float velLength;
    do {
        velLength = glm::length(packet.velocity);
        if (velLength < Eps)
            break;
        checkTriangles(packet, triangles);
    } while (packet.foundCollision && ++depth < 5);
//    std::cout << depth << std::endl;

    if (velLength < Eps)
        packet.finalPosition = packet.basePoint;
    else
        packet.finalPosition = packet.basePoint + (packet.velocity / velLength) * packet.nearestDistance;
}


} // end namespace Collision
