#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include <glm/vec3.hpp>
#include <glm/detail/func_geometric.hpp>
#include <array>
#include <vector>

namespace Geom
{

struct AABB
{
    AABB() = default;
    AABB(glm::vec3 min, glm::vec3 max)
        : min(min), max(max) {}
    glm::vec3 min, max; // min & max xyz in world coordinates
    bool intersects(const AABB& box);
};

struct Plane
{
    Plane() = default;
    Plane(glm::vec3 norm, glm::vec3 point)
        : norm(glm::normalize(norm)), point(point) {}
    glm::vec3 norm {0, 1, 0};
    glm::vec3 point {0, 0, 0};
};

struct Triangle
{
    Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
        : p1(p1), p2(p2), p3(p3) {}
    glm::vec3 p1, p2, p3;
};

struct Edge
{
    Edge(const glm::vec3& point1, const glm::vec3& point2)
        : point1(point1), point2(point2) {}
    glm::vec3 point1, point2;
};

struct Sphere
{
    Sphere() = default;
    Sphere(glm::vec3 center, float radius)
        : center(center), radius(radius) {}
    glm::vec3 center {0, 0, 0};
    float radius {1};
};

std::array<Plane, 6> planesFromAABB(const AABB& box);

std::vector<Triangle> trianglesFromAABB(const AABB& box);

float distanceToPlane(const glm::vec3& point, const Plane& plane);

glm::vec3 projectPointToPlane(const glm::vec3& point, const Plane& plane);

float squaredLength(const glm::vec3& vec);

bool vecIntersectsPlane(const Plane& plane,
                        const glm::vec3& position,
                        const glm::vec3& direction,
                        float& distBeforeIntersect);

Plane planeFromTriangle(const Triangle& triangle);

bool isPlaneFrontFacingToVec(const Plane& plane, const glm::vec3& vec);

bool onSameSide(const glm::vec3& p1, const glm::vec3& p2, const Edge& line);

bool isPointInTriangle(const glm::vec3& point, const Triangle& triangle);

bool solveQuadEquation(float a, float b, float c, std::pair<float, float>& roots);

}

#endif // GEOMETRY_H_INCLUDED
