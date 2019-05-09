#include "player.h"
#include "graphics/camera.h"
#include "chunkmanager.h"
#include "playercontrols.h"
#include "graphics/outline.h"
#include "maths/geometry.h"

#include <glm/geometric.hpp>
#include <vector>
#include <cmath>

constexpr glm::vec3 Head {0, 1.4, 0};

Player::Player(PControl control)
{
    setControl(std::move(control));
}

void Player::setControl(PControl control)
{
    glm::vec3 pos {0, 0, 0};
    glm::vec3 dir {0, 0, 1};
    if (m_control) {
        pos = m_control->getPosition();
        dir = m_control->getDirection();
    }
    m_control.reset();
    m_control = std::move(control);
    m_control->setPlayer(this);
    m_control->setPosition(pos);
    m_control->setDirection(dir);
}

void Player::move(AbstractPlayerControl::Dir dir, float offset)
{
    m_control->move(dir, offset);
}

void Player::update(float dt)
{
    m_control->update(dt);
    if (m_camera)
        m_camera->setPosition(m_control->getPosition() + Head);
}

void Player::setPosition(glm::vec3 pos)
{
    m_control->setPosition(pos);
    if (m_camera)
        m_camera->setPosition(pos + Head);
}

void Player::rotate(float offsYaw, float offsPitch)
{
    if (m_camera) {
        m_camera->rotate(offsYaw, offsPitch);
        m_control->setDirection(m_camera->getDirection());
    }
}

void Player::setRotation(float yaw, float pitch)
{
    if (m_camera) {
        m_camera->setRotation(yaw, pitch);
        m_control->setDirection(m_camera->getDirection());
    }
}

void Player::setDirection(glm::vec3 dir)
{
    if (m_camera) {
        m_camera->setDirection(dir);
        m_control->setDirection(dir);
    }
}

void Player::jump()
{
    m_control->jump();
}

void Player::bindCamera(Camera* camera)
{
    m_camera = camera;
    m_camera->setPosition(m_control->getPosition() + Head);
}

void Player::setWorldData(ChunkManager* chunkManager)
{
    m_world = chunkManager;
}

ChunkManager* Player::getWorldData() const
{
    return m_world;
}

glm::vec3 Player::getPosition() const
{
    return m_control->getPosition();
}

void Player::render()
{
    int radius = 5;

    glm::vec3 head = m_control->getPosition() + Head;
    glm::vec3 dir = m_control->getDirection();

    int headx = std::floor(head.x);
    int heady = std::floor(head.y);
    int headz = std::floor(head.z);

    int xmin = headx - radius;
    int ymin = heady - radius;
    int zmin = headz - radius;

    int xmax = xmin + 2 * radius + 1;
    int ymax = ymin + 2 * radius + 1;
    int zmax = zmin + 2 * radius + 1;

    glm::vec3 target;
    bool found = false;
    float distance = radius * 2;

    for (int x = xmin; x <= xmax; x++)
    for (int y = ymin; y <= ymax; y++)
    for (int z = zmin; z <= zmax; z++)
    {
        if ((headx - x) * (headx - x) + (heady - y) * (heady - y) + (headz - z) * (headz - z) > radius * radius)
            continue;

        if (m_world->get({x, y, z}) == static_cast<uint8_t>(Blocks::Type::None) ||
            m_world->get({x, y, z}) == static_cast<uint8_t>(Blocks::Type::Water))
            continue;

        Geom::AABB box({x, y, z}, {x + 1, y + 1, z + 1});
        auto triangles = box.getTriangles();
        for (Geom::Triangle& triangle : triangles)
        {
            Geom::Plane plane = triangle.getPlane();
            float distanceCurrent;
            bool intersects = plane.vectorIntersects(head, dir, distanceCurrent);
            if (intersects)
            {
                auto point = head + dir * distanceCurrent;
                if (box.isPointInside(point) && distanceCurrent < distance)
                {
                    distance = distanceCurrent;
                    target = glm::vec3(x, y, z);
                    found = true;
                }
            }
        }
    }

    if (found)
        Outline::render(Geom::AABB(target - glm::vec3(0.01, 0.01, 0.01),
                                   target + glm::vec3(1.01, 1.01, 1.01)));
}




