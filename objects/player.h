#ifndef PLAYER_H
#define PLAYER_H

#include <glm/vec3.hpp>
#include "geometry.h"

class Camera;
class ChunkManager;

class Player
{
public:
    Player() = default;

    enum Dir {Forward, Back, Left, Right};
    void move(Dir dir, float offset);
    void setPosition(glm::vec3 pos);
    void rotate(float offsYaw, float offsPitch);
    void setRotation(float yaw, float pitch);
    void setDirection(glm::vec3 dir);
    void bindCamera(Camera* camera);
    void setWorldData(ChunkManager* chunkManager);
    glm::vec3 getPosition() const;

    void update();

private:
    void processCollisionsWithWorld();

private:
    Camera*     m_camera {nullptr};
    ChunkManager* m_world  {nullptr};
    glm::vec3   m_pos {0, 0, 0},
                m_dir {0, 0, 1},
                m_movementVec {0, 0, 0};
};

#endif // PLAYER_H
