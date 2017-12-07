#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera
{
public:
    enum CameraMovement {Forward, Back, Left, Right};

    Camera() = default;
    Camera(const glm::vec3& position, float yaw, float pitch);

    void move(CameraMovement dir, float offset);
    void rotate(float offsYaw, float offsPitch);
    void setRotation(float yaw, float pitch);
    void setDirection(const glm::vec3& direction);

    void setPosition(const glm::vec3& position) {m_pos = position;}
    const glm::vec3& getPosition()  const {return m_pos;}
    const glm::vec3& getDirection() const {return m_orientation;}
    const glm::vec3& getUp()        const {return m_up;}
    const glm::vec3& getRight()     const {return m_right;}

    glm::mat4 getViewMatrix() const;

private:
    void update();
    void validateYawPitch();

private:
    glm::vec3   m_pos           {0, 100, 0},
                m_orientation,
                m_worldUp       {0, 1, 0},// used for m_right calculation
                m_up,            // used for view matrix calculation
                m_right;        // used for left-right movement calculation

    float       m_yaw           {90},
                m_pitch         {0};
};

#endif // CAMERA_H_INCLUDED
