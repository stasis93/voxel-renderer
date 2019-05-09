#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <glm/vec3.hpp>
#include "playercontrols.h"
#include "graphics/renderable.h"

using PControl = std::unique_ptr<AbstractPlayerControl>;

class Camera;
class ChunkManager;

class Player : public Renderable
{
public:
    Player() = default;
    Player(PControl control);

    void setControl(PControl control);

    void move(AbstractPlayerControl::Dir dir, float offset);
    void jump();
    void setPosition(glm::vec3 pos);
    glm::vec3 getPosition() const;
    void rotate(float offsYaw, float offsPitch);
    void setRotation(float yaw, float pitch);
    void setDirection(glm::vec3 dir);

    void bindCamera(Camera* camera);

    void setWorldData(ChunkManager* chunkManager);
    ChunkManager* getWorldData() const;

    void update(float dt);
    void render();

private:
    PControl    m_control;
    Camera*     m_camera {nullptr};
    ChunkManager* m_world {nullptr};
};

#endif // PLAYER_H
