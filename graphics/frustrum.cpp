#include <iostream>
#include <glm/detail/func_geometric.hpp>

#include "frustrum.h"

int Frustrum::checkPoint(const glm::vec3& p) const
{
    for (const auto& plane : m_planes)
        if (plane.distanceToPoint(p) < 0.0f)
            return Outside;
    return Inside;
}

int Frustrum::checkSphere(const glm::vec3& center, float rad) const
{
    bool intersect = false;

    for (const auto& plane : m_planes)
    {
        float dist = plane.distanceToPoint(center);

        if (dist < -rad)
            return Outside;
        if (dist < rad)
            intersect = true;
    }
    if (intersect)
        return Intersect;
    return Inside;
}

int Frustrum::checkBox(const Geom::AABB& box) const
{
    bool intersect = false;

    for (const auto& plane : m_planes)
    {
        /* The positive vertex is the vertex from the box
        that is further along the normal’s direction.
        The negative vertex is the opposite vertex.*/

        glm::vec3 negVert = box.max;
        glm::vec3 posVert = box.min;

        if (plane.norm.x > 0) {
            negVert.x = box.min.x;
            posVert.x = box.max.x;
        }
        if (plane.norm.y > 0) {
            negVert.y = box.min.y;
            posVert.y = box.max.y;
        }
        if (plane.norm.z > 0) {
            negVert.z = box.min.z;
            posVert.z = box.max.z;
        }
        if (plane.distanceToPoint(posVert) < 0)
            return Outside;
        if (plane.distanceToPoint(negVert) < 0)
            intersect = true;
    }
    if (intersect)
        return Intersect;
    return Inside;
}

void Frustrum::updatePlanes(const glm::vec3& cam_pos,
                            const glm::vec3& cam_dir,
                            const glm::vec3& cam_up,
                            const glm::vec3& cam_right,
                            float fovy, float aspect,
                            float dNear, float dFar)
{
    const glm::vec3& d = cam_dir;
    const glm::vec3& p = cam_pos;
    const glm::vec3& r = cam_right;
    const glm::vec3& up = cam_up;

    float tan = std::tan(fovy / 2.0f);
    float hNear = 2 * tan * dNear;
    float wNear = hNear * aspect;

    glm::vec3 fc = p + d * dFar;
    glm::vec3 nc = p + d * dNear;

    // compute planes, all normal vectors must point inside of frustrum
    // Near
    m_planes[static_cast<int>(Planes::Near)].norm = d;
    m_planes[static_cast<int>(Planes::Near)].point = nc;

    // Far
    m_planes[static_cast<int>(Planes::Far)].norm = -d;
    m_planes[static_cast<int>(Planes::Far)].point = fc;

    // Right
    glm::vec3 aux;
    aux = glm::normalize(nc + r * wNear / 2.0f - p);
    m_planes[static_cast<int>(Planes::Right)].point = p; // cam_pos is common point for L,R,U,D planes
    m_planes[static_cast<int>(Planes::Right)].norm = glm::cross(up, aux);

    // Left
    aux = glm::normalize(nc - r * wNear / 2.0f - p);
    m_planes[static_cast<int>(Planes::Left)].point = p;
    m_planes[static_cast<int>(Planes::Left)].norm = glm::cross(aux, up);

    // Top
    aux = glm::normalize(nc + up * hNear / 2.0f - p);
    m_planes[static_cast<int>(Planes::Top)].point = p;
    m_planes[static_cast<int>(Planes::Top)].norm = glm::cross(aux, r);

    // Bottom
    aux = glm::normalize(nc - up * hNear / 2.0f - p);
    m_planes[static_cast<int>(Planes::Bottom)].point = p;
    m_planes[static_cast<int>(Planes::Bottom)].norm = glm::cross(r, aux);
}

