#include "playercontrols.h"
#include "collision.h"
#include "chunkmanager.h"
#include "player.h"


constexpr glm::vec3 Up {0, 1, 0};
constexpr glm::vec3 BodyRadius {0.5, 1.5, 0.5};

constexpr float g = 0.5f;
constexpr float invSqrt2 = 1 / std::sqrt(2.0f);


void FlyingControl::move(Dir dir, float offset)
{
    glm::vec3 movement {0, 0, 0};
    switch (dir)
    {
    case Forward:
        movement = m_dir;
        break;
    case Back:
        movement = -m_dir;
        break;
    case Left:
        movement = glm::normalize(glm::cross(Up, m_dir));
        break;
    case Right:
        movement = -glm::normalize(glm::cross(Up, m_dir));
        break;
    default:
        break;
    }
    m_movementVec += movement * offset;
}

void FlyingControl::update(float /*dt*/)
{
    m_pos += m_movementVec;
    m_movementVec = {0, 0, 0};
}

// =========================================================================

void WalkingControl::move(Dir dir, float offset)
{
    glm::vec3 movement {0, 0, 0};
    switch (dir)
    {
    case Forward:
        movement = glm::normalize(glm::vec3{m_dir.x, 0, m_dir.z});
        m_forwBack = true;
        break;
    case Back:
        movement = -glm::normalize(glm::vec3{m_dir.x, 0, m_dir.z});
        m_forwBack = true;
        break;
    case Left:
        movement = glm::normalize(glm::cross(Up, m_dir));
        m_leftRight = true;
        break;
    case Right:
        movement = -glm::normalize(glm::cross(Up, m_dir));
        m_leftRight = true;
        break;
    default:
        break;
    }
    m_movementVec += movement * offset;
}

void WalkingControl::update(float dt)
{
    // Normalize if going diagonally
    if (m_leftRight && m_forwBack) {
        m_movementVec.x *= invSqrt2;
        m_movementVec.z *= invSqrt2;
    }
    m_leftRight = false;
    m_forwBack = false;

    processCollisionsWithWorld();
    m_movementVec = {0, 0, 0};

    m_verticalVelocity -= g * dt;
}

void WalkingControl::jump()
{
    if (!m_inAir)
        m_verticalVelocity = 0.2f;
}

void WalkingControl::processCollisionsWithWorld()
{
    // Get nearest blocks for collision testing
    int xmin = std::floor(m_pos.x) - std::ceil(BodyRadius.x);
    int xmax = std::floor(m_pos.x) + std::ceil(BodyRadius.x);
    int ymin = std::floor(m_pos.y) - std::ceil(BodyRadius.y);
    int ymax = std::floor(m_pos.y) + std::ceil(BodyRadius.y);
    int zmin = std::floor(m_pos.z) - std::ceil(BodyRadius.z);
    int zmax = std::floor(m_pos.z) + std::ceil(BodyRadius.z);

    // Extract triangles for testing from these blocks
    std::vector<Geom::Triangle> triangles;
    triangles.reserve((xmax - xmin + 1) * (ymax - ymin + 1) * (zmax - zmin + 1));

    for (int x = xmin; x <= xmax; x++)
    for (int y = ymin; y <= ymax; y++)
    for (int z = zmin; z <= zmax; z++)
    {
        auto block = static_cast<Blocks::Type>(m_player->getWorldData()->get({x, y, z}));
        if (block != Blocks::Type::None && block != Blocks::Type::Water)
        {
            Geom::AABB blockBox({x, y, z}, {x + 1, y + 1, z + 1});
            auto boxTriangles = Geom::trianglesFromAABB(blockBox);
            triangles.insert(std::end(triangles), std::begin(boxTriangles), std::end(boxTriangles));
        }
    }
    for (Triangle& t : triangles) {
        t.p1 /= BodyRadius;
        t.p2 /= BodyRadius;
        t.p3 /= BodyRadius;
    }
    // Process possible collisions
    Collision::Packet packet;

    packet.basePoint = m_pos / BodyRadius;
    packet.velocity = m_movementVec / BodyRadius;

    Collision::collide(packet, triangles);

    // Second pass for gravity
    packet.basePoint = packet.finalPosition;
    packet.velocity = glm::vec3 {0, m_verticalVelocity, 0};

    Collision::collide(packet, triangles);
    m_pos = packet.finalPosition * BodyRadius;

    // If not flying, reset vertical velocity
    float delta_y = packet.finalPosition.y - packet.basePoint.y;
    if (delta_y == 0) {
        m_verticalVelocity = 0.0f;
        m_inAir = false;
    } else {
        m_inAir = true;
    }
}
