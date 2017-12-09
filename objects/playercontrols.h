#ifndef PLAYERCONTROLS_H_INCLUDED
#define PLAYERCONTROLS_H_INCLUDED

#include <glm/vec3.hpp>

class Player;

class AbstractPlayerControl
{
public:
    enum Dir {Forward, Back, Left, Right};

    virtual ~AbstractPlayerControl() = default;
    virtual void move(Dir dir, float offset) = 0;
    virtual void update(float dt) = 0;
    virtual void jump() {};
    void setPlayer(Player* player) {m_player = player;}
    void setPosition(glm::vec3 pos) {m_pos = pos;}
    glm::vec3 getPosition() {return m_pos;}

    void setDirection(glm::vec3 dir) {m_dir = dir;}
    glm::vec3 getDirection() {return m_dir;}

protected:
    Player*     m_player {nullptr};
    glm::vec3   m_pos {0, 0, 0},
                m_dir {0, 0, 1},
                m_movementVec {0, 0, 0};
};

// Simple flying (noclip)

class FlyingControl : public AbstractPlayerControl
{
public:
    void move(Dir dir, float offset);
    void update(float dt);
};

// Walking with gravity and collisions

class WalkingControl : public AbstractPlayerControl
{
public:
    void move(Dir dir, float offset);
    void update(float dt);
    void jump();

protected:
    void processCollisionsWithWorld();

protected:
    float       m_verticalVelocity {0};
    bool        m_inAir {true},
                m_leftRight {false},
                m_forwBack {false};
};

#endif // PLAYERCONTROLS_H_INCLUDED
