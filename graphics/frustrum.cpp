#include <iostream>
#include <glm/detail/func_geometric.hpp>

#include "frustrum.h"

int Frustrum::checkPoint(const glm::vec3& p) const
{
    for (int i = 0; i < 6; i++)
        if (distanceToPlane(p, i) < 0.0f)
            return Outside;
    return Inside;
}

int Frustrum::checkSphere(const glm::vec3& center, float rad) const
{
    bool intersect = false;

    for (int i = 0; i < 6; i++)
    {
        float dist = distanceToPlane(center, i);

        if (dist < -rad)
            return Outside;
        if (dist < rad)
            intersect = true;
    }
    if (intersect)
        return Intersect;
    return Inside;
}

float Frustrum::distanceToPlane(const glm::vec3& point, int plane) const
{
    // Ax + By + Cz = -D;
    // (A,B,C) = n; (x,y,z) = p => D = - n dot p; (p is point on plane (Plane.point in our case))
    // distance_to_point = plane_normal dot point + D;

    const Plane& pln = m_planes[plane];
    float D = -glm::dot(pln.norm, pln.point);
    return glm::dot(pln.norm, point) + D;
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

