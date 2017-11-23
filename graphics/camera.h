#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera
{
public:
    enum CameraMovement {Forward, Back, Left, Right};

    Camera(const glm::vec3 & position = {0.0f, 0.0f, 0.0f}, float yaw = -90.0f, float pitch = 0.0f);
    void move(CameraMovement dir, float offset);
    void rotate(float offsYaw, float offsPitch);
    void setRotation(float yaw, float pitch);
    glm::mat4 getViewMatrix() const;

    const glm::vec3& getPosition()  const {return m_pos;}
    const glm::vec3& getDirection() const {return m_orientation;}
    const glm::vec3& getUp()        const {return m_up;}
    const glm::vec3& getRight()     const {return m_right;}

    void setPosition(float x, float y, float z) {m_pos = {x, y, z};}

private:
    void update();
    void validateYawPitch();

private:
    glm::vec3   m_pos,
                m_orientation,
                m_worldUp,      // used for m_right calculation
                m_up,           // used for view matrix calculation
                m_right;        // used for left-right movement calculation

    float       m_yaw,
                m_pitch;
};

#endif // CAMERA_H_INCLUDED
