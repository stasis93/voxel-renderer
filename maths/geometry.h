#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include <glm/vec3.hpp>
#include <glm/detail/func_geometric.hpp>
#include <array>

namespace Geom
{

struct Plane
{
    Plane() = default;
    Plane(glm::vec3 norm, glm::vec3 point)
        : norm(glm::normalize(norm)), point(point) {}

    bool isFrontFacingToVec(const glm::vec3& vector) const;
    bool vectorIntersects(const glm::vec3& start, const glm::vec3& direction, float& distBeforeIntersect) const;
    float distanceToPoint(const glm::vec3& point) const;
    glm::vec3 projectPoint(const glm::vec3& point) const;

    glm::vec3 norm {0, 1, 0},
              point {0, 0, 0};
};

struct Line
{
    Line(const glm::vec3& point1, const glm::vec3& point2)
        : point1(point1), point2(point2) {}
    glm::vec3 point1, point2;
};

struct Triangle
{
    Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
        : p1(p1), p2(p2), p3(p3) {}

    Plane getPlane() const;
    bool isPointInside(const glm::vec3& point) const;
    std::array<glm::vec3, 3> getPoints() const;
    std::array<Line, 3> getEdges() const;

    glm::vec3 p1, p2, p3;
};

struct AABB
{
    AABB() = default;
    AABB(glm::vec3 min, glm::vec3 max)
        : min(min), max(max) {}

    bool intersects(const AABB& box) const;
    bool isPointInside(const glm::vec3& point) const;
    std::array<Plane, 6> getPlanes() const;
    std::array<Triangle, 12> getTriangles() const;

    glm::vec3 min, max; // min & max xyz in world coordinates
};

float squaredLength(const glm::vec3& vec);

bool solveQuadEquation(float a, float b, float c, std::pair<float, float>& roots);

}

#endif // GEOMETRY_H_INCLUDED
