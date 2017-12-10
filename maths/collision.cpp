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
void Packet::checkTriangle(const Triangle& triangle)
{
    Plane plane = triangle.getPlane();

    if (!plane.isFrontFacingToVec(m_velocity))
        return;

    float t0, t1;
    bool embedded = false;

    float distance = plane.distanceToPoint(m_basePoint);
    float n_dot_vel = glm::dot(plane.norm, m_velocity);

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
        glm::vec3 planeIntersectionPoint = m_basePoint -
        plane.norm + t0 * m_velocity;

        if (triangle.isPointInside(planeIntersectionPoint))
        {
            foundCollision = true;
            t = t0;
            collisionPoint = planeIntersectionPoint;
        }
    }
    if (!foundCollision)
    {
        // Check against triangle vertices
        auto& vel = m_velocity;
        auto& base = m_basePoint;
        float vel_lenSquared = squaredLength(vel);

        std::pair<float, float> roots;
        float a = vel_lenSquared;
        float b, c;

        for (const auto& p : triangle.getPoints())
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
        for (const auto& edge : triangle.getEdges())
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
        float distToCollision = t * glm::length(m_velocity);
        if (!m_foundCollision || distToCollision < m_nearestDistance)
        {
            m_nearestDistance = distToCollision;
            m_intersectionPoint = collisionPoint;
            m_foundCollision = true;
        }
    }
} // end checkTriangle

void Packet::checkTriangles(const std::vector<Triangle>& triangles)
{
    m_foundCollision = false;
    m_nearestDistance = glm::length(m_velocity);

    for (const Triangle& t : triangles)
        checkTriangle(t);

    if (!m_foundCollision)
        return;

    auto velocityNormalized = glm::normalize(m_velocity);
    auto baseAtIntersect = m_basePoint + velocityNormalized * m_nearestDistance;
    auto normal = baseAtIntersect - m_intersectionPoint;

    Plane slidingPlane(normal, m_intersectionPoint);

    auto plannedPosition = m_basePoint + m_velocity;
    auto plannedPositionProj = slidingPlane.projectPoint(plannedPosition);
    auto slidingVel = plannedPositionProj - m_intersectionPoint;

    m_velocity = slidingVel;
    m_basePoint = baseAtIntersect;
}

void Packet::setBasePosition(const glm::vec3& basePoint)
{
    m_basePoint = basePoint;
}

void Packet::setVelocity(const glm::vec3& velocity)
{
    m_velocity = velocity;
}

glm::vec3 Packet::testAgainstTriangles(const std::vector<Triangle>& trianglesToTest)
{
    int depth = 0;
    float velLength;
    do {
        velLength = glm::length(m_velocity);
        if (velLength < Eps)
            break;
        checkTriangles(trianglesToTest);
    } while (m_foundCollision && ++depth < 5);
//    std::cout << depth << std::endl;

    if (velLength < Eps)
        m_finalPosition = m_basePoint;
    else
        m_finalPosition = m_basePoint + (m_velocity / velLength) * m_nearestDistance;

    return m_finalPosition;
}


} // end namespace Collision
