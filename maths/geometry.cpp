#include "geometry.h"

#include <glm/geometric.hpp>
#include <cmath>

namespace Geom
{

float Plane::distanceToPoint(const glm::vec3& point) const
{
    // Ax + By + Cz = -D;
    // (A,B,C) = n; (x,y,z) = p => D = - n dot p; (p is point on plane (Plane.point in our case))
    // distance_to_point = plane_normal dot point + D;
    return glm::dot(this->norm, point - this->point);
}

glm::vec3 Plane::projectPoint(const glm::vec3& point) const
{
    float distance = this->distanceToPoint(point);
    return point - this->norm * distance;
}

bool Plane::isFrontFacingToVec(const glm::vec3& vector) const
{
    return glm::dot(norm, vector) < 0;
}

bool Plane::vectorIntersects(const glm::vec3& start, const glm::vec3& direction, float& distBeforeIntersect) const
{
    /*
    [From http://nehe.gamedev.net/tutorial/collision_detection/17005/]

    The two equations we have so far are:

    PointOnRay = Raystart + t * Raydirection
    Xn dot X = d

    If a ray intersects the plane at some point then there must be some point
    on the ray which satisfies the plane equation as follows:

    Xn dot PointOnRay = d or (Xn dot Raystart) + t * (Xn dot Raydirection) = d

    solving for t:

    t = (d - Xn dot Raystart) / (Xn dot Raydirection)

    replacing d:

    t= (Xn dot PointOnPLANE - Xn dot Raystart) / (Xn dot Raydirection)

    summing it up:

    t= (Xn dot (PointOnPLANE - Raystart)) / (Xn dot Raydirection)
    */
    static constexpr float Eps = 1e-3f;

    float dot = glm::dot(norm, direction);
    if (dot > -Eps && dot < Eps) // if direction is parallel with plane
        return false;

    float len = glm::dot(norm, point - start) / dot;
    if (len < -Eps) // if collision is behind
        return false;

    distBeforeIntersect = len;
    return true;
}

// ===========================================================

bool AABB::intersects(const AABB& box) const
{
    if ((max.x < box.min.x || box.max.x < min.x) ||
        (max.y < box.min.y || box.max.y < min.y) ||
        (max.z < box.min.z || box.max.z < min.z))
        return false;
    return true;
}

bool AABB::isPointInside(const glm::vec3& point) const
{
    return point.x >= min.x && point.x <= max.x &&
           point.y >= min.y && point.y <= max.y &&
           point.z >= min.z && point.z <= max.z;
}

std::array<Plane, 6> AABB::getPlanes() const
{
    float bx = max.x - min.x,
          by = max.y - min.y,
          bz = max.z - min.z;

    return {
        /*  normal vector  |       point on plane               */
        Plane({ 0,  0, -1}, {min.x      , min.y      , min.z}),
        Plane({ 0,  0,  1}, {min.x      , min.y      , min.z + bz}),
        Plane({ 0, -1,  0}, {min.x      , min.y      , min.z}),
        Plane({ 0,  1,  0}, {min.x      , min.y + by , min.z}),
        Plane({-1,  0,  0}, {min.x      , min.y      , min.z}),
        Plane({ 1,  0,  0}, {min.x + bx , min.y      , min.z})
    };
}

std::array<Triangle, 12> AABB::getTriangles() const
{
    float x0 = min.x, x1 = max.x,
          y0 = min.y, y1 = max.y,
          z0 = min.z, z1 = max.z;

    /* Extract triangles from each face... */
    return {
        // view from -x
        Triangle({x0, y0, z0}, {x0, y0, z1}, {x0, y1, z1}),
        Triangle({x0, y0, z0}, {x0, y1, z1}, {x0, y1, z0}),
        // view from +x
        Triangle({x1, y0, z1}, {x1, y0, z0}, {x1, y1, z0}),
        Triangle({x1, y0, z1}, {x1, y1, z0}, {x1, y1, z1}),
        // view from -y
        Triangle({x0, y0, z0}, {x1, y0, z0}, {x1, y0, z1}),
        Triangle({x0, y0, z0}, {x1, y0, z1}, {x0, y0, z1}),
        // view from +y
        Triangle({x0, y1, z1}, {x1, y1, z1}, {x1, y1, z0}),
        Triangle({x0, y1, z1}, {x1, y1, z0}, {x0, y1, z0}),
        // view from -z
        Triangle({x1, y0, z0}, {x0, y0, z0}, {x0, y1, z0}),
        Triangle({x1, y0, z0}, {x0, y1, z0}, {x1, y1, z0}),
        // view from +z
        Triangle({x0, y0, z1}, {x1, y0, z1}, {x1, y1, z1}),
        Triangle({x0, y0, z1}, {x1, y1, z1}, {x0, y1, z1})
    };
}

// ===========================================================

Plane Triangle::getPlane() const
{
    glm::vec3 u = p2 - p1,
              v = p3 - p1;
    Plane plane;
    plane.norm = glm::normalize(glm::cross(u, v));
    plane.point = p1;
    return plane;
}

bool Triangle::isPointInside(const glm::vec3& point) const
{
    /* http://blackpawn.com/texts/pointinpoly/ */
    // compute vectors
    auto v0 = p3 - p1;
    auto v1 = p2 - p1;
    auto v2 = point - p1;

    // compute dot products
    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);

    // compute barycentric coordinates
    float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // check if point is in triangle
    return u >= 0 && v >= 0 && u + v <= 1;
}

std::array<glm::vec3, 3> Triangle::getPoints() const
{
    return {p1, p2, p3};
}

std::array<Line, 3> Triangle::getEdges() const
{
    return {Line(p1, p2), Line(p2, p3), Line(p3, p1)};
}

// ===========================================================

bool solveQuadEquation(float a, float b, float c, std::pair<float, float>& roots)
{
    float d = b * b - 4 * a * c;

    if (d < 0)
        return false;

    float d_root = std::sqrt(d);

    float r1 = (-b - d_root) / (2 * a);
    float r2 = (-b + d_root) / (2 * a);

    if (r1 > r2)
        std::swap(r1, r2);

    roots.first = r1;
    roots.second = r2;

    return true;
}

float squaredLength(const glm::vec3& vec)
{
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

}
