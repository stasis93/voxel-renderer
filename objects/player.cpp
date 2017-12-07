#include "player.h"
#include "camera.h"
#include "chunkmanager.h"
#include "collision.h"

#include <glm/detail/func_geometric.hpp>
#include <vector>
#include <cmath>
#include <iostream>

constexpr glm::vec3 Up {0, 1, 0};
//constexpr glm::vec3 Size {0.5, 2.9, 0.5};
//constexpr glm::vec3 Cam {Size.x * 0.5, Size.y, Size.z * 0.5};

void Player::move(Dir dir, float offset)
{
    switch (dir)
    {
    case Forward:
        m_movementVec = m_dir;
        break;
    case Back:
        m_movementVec = -m_dir;
        break;
    case Left:
        m_movementVec = glm::normalize(glm::cross(Up, m_dir));
        break;
    case Right:
        m_movementVec = -glm::normalize(glm::cross(Up, m_dir));
        break;
    default:
        break;
    }
    m_movementVec *= offset;
    //m_posPlanned = m_pos + offset * m_vel;
//    setPosition(m_pos + offset * m_vel);
}

void Player::update()
{
    processCollisionsWithWorld();
    if (m_camera)
        m_camera->setPosition(m_pos);
    m_movementVec = {0, 0, 0};
}

void Player::setPosition(glm::vec3 pos)
{
//    m_movementVec = pos - m_pos;
    m_pos = pos;
}

void Player::rotate(float offsYaw, float offsPitch)
{
    if (m_camera) {
        m_camera->rotate(offsYaw, offsPitch);
        m_dir = m_camera->getDirection();
    }
}

void Player::setRotation(float yaw, float pitch)
{
    if (m_camera) {
        m_camera->setRotation(yaw, pitch);
        m_dir = m_camera->getDirection();
    }
}

void Player::setDirection(glm::vec3 dir)
{
    if (m_camera) {
        m_camera->setDirection(dir);
        m_dir = dir;
    }
}

void Player::bindCamera(Camera* camera)
{
    m_camera = camera;
    m_camera->setPosition(m_pos);
}

void Player::setWorldData(ChunkManager* chunkManager)
{
    m_world = chunkManager;
}

glm::vec3 Player::getPosition() const
{
    return m_pos;
}

void Player::processCollisionsWithWorld()
{
    int xmin = std::floor(m_pos.x) - 2;
    int xmax = std::floor(m_pos.x) + 2;
    int ymin = std::floor(m_pos.y) - 2;
    int ymax = std::floor(m_pos.y) + 2;
    int zmin = std::floor(m_pos.z) - 2;
    int zmax = std::floor(m_pos.z) + 2;

    Collision::Packet packet;
    packet.basePoint = m_pos;
    packet.velocity = m_movementVec;

    std::vector<Geom::Triangle> triangles;

    for (int x = xmin; x <= xmax; x++)
    for (int y = ymin; y <= ymax; y++)
    for (int z = zmin; z <= zmax; z++)
    {
        auto block = static_cast<Blocks::Type>(m_world->get({x, y, z}));
        if (block != Blocks::Type::None && block != Blocks::Type::Water)
        {
            Geom::AABB blockBox({x, y, z}, {x + 1, y + 1, z + 1});
            auto boxTriangles = Geom::trianglesFromAABB(blockBox);
            triangles.insert(std::end(triangles), std::begin(boxTriangles), std::end(boxTriangles));
        }
    }
    Collision::collide(packet, triangles);
    m_pos = packet.finalPosition;
}
