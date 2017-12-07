#include "geometry.h"

#include <glm/detail/func_geometric.hpp>
#include <cmath>

namespace Geom
{

float distanceToPlane(const glm::vec3& point, const Plane& plane)
{
    // Ax + By + Cz = -D;
    // (A,B,C) = n; (x,y,z) = p => D = - n dot p; (p is point on plane (Plane.point in our case))
    // distance_to_point = plane_normal dot point + D;
    float D = -glm::dot(plane.norm, plane.point);
    return glm::dot(plane.norm, point) + D;
}

glm::vec3 projectPointToPlane(const glm::vec3& point, const Plane& plane)
{
    float distance = distanceToPlane(point, plane);
    return point - plane.norm * distance;
}

bool AABB::intersects(const AABB& box)
{
    if ((max.x < box.min.x || box.max.x < min.x) ||
        (max.y < box.min.y || box.max.y < min.y) ||
        (max.z < box.min.z || box.max.z < min.z))
        return false;
    return true;
}

std::array<Plane, 6> planesFromAABB(const AABB& box)
{  /* Extract planes from axis-aligned bounding box */
    float bx = box.max.x - box.min.x,
          by = box.max.y - box.min.y,
          bz = box.max.z - box.min.z;

    return {
        /*  normal vector  |       point on plane                            */
        Plane({ 0,  0, -1}, {box.min.x      , box.min.y      , box.min.z}),
        Plane({ 0,  0,  1}, {box.min.x      , box.min.y      , box.min.z + bz}),
        Plane({ 0, -1,  0}, {box.min.x      , box.min.y      , box.min.z}),
        Plane({ 0,  1,  0}, {box.min.x      , box.min.y + by , box.min.z}),
        Plane({-1,  0,  0}, {box.min.x      , box.min.y      , box.min.z}),
        Plane({ 1,  0,  0}, {box.min.x + bx , box.min.y      , box.min.z})
    };
}

std::vector<Triangle> trianglesFromAABB(const AABB& box)
{
    float x0 = box.min.x, x1 = box.max.x,
          y0 = box.min.y, y1 = box.max.y,
          z0 = box.min.z, z1 = box.max.z;

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

bool vecIntersectsPlane(const Plane& plane,
                        const glm::vec3& position,
                        const glm::vec3& direction,
                        float& distBeforeIntersect)
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

    float dot = glm::dot(plane.norm, direction);
    if (dot > -Eps && dot < Eps) // if direction is parallel with plane
        return false;

    float len = glm::dot(plane.norm, plane.point - position) / dot;
    if (len < -Eps) // if collision is behind
        return false;

    distBeforeIntersect = len;
    return true;
}

float squaredLength(const glm::vec3& vec)
{
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

Plane planeFromTriangle(const Triangle& triangle)
{
    glm::vec3 u = triangle.p2 - triangle.p1,
              v = triangle.p3 - triangle.p1;
    Plane plane;
    plane.norm = glm::normalize(glm::cross(u, v));
    plane.point = triangle.p1;
    return plane;
}

bool isPlaneFrontFacingToVec(const Plane& plane, const glm::vec3& vec)
{
    return glm::dot(plane.norm, vec) < 0;
}

bool onSameSide(const glm::vec3& p1, const glm::vec3& p2, const Edge& line)
{
    auto cross1 = glm::cross(line.point2 - line.point1, p1 - line.point1);
    auto cross2 = glm::cross(line.point2 - line.point1, p2 - line.point1);
    return glm::dot(cross1, cross2) >= 0;
}

bool isPointInTriangle(const glm::vec3& point, const Triangle& triangle)
{
    /* http://blackpawn.com/texts/pointinpoly/ */

//    if (onSameSide(point, triangle.p3, {triangle.p1, triangle.p2}) &&
//        onSameSide(point, triangle.p1, {triangle.p2, triangle.p3}) &&
//        onSameSide(point, triangle.p2, {triangle.p3, triangle.p1}))
//            return true;
//    return false;

    // compute vectors
    auto v0 = triangle.p3 - triangle.p1;
    auto v1 = triangle.p2 - triangle.p1;
    auto v2 = point - triangle.p1;

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

}
