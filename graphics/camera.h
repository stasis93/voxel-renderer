#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement
{
    FORWARD,
    BACK,
    LEFT,
    RIGHT
};

class Camera
{
public:
    Camera(const glm::vec3 & position = {0.0f, 0.0f, 0.0f}, float yaw = -90.0f, float pitch = 0.0f);
    void move(CameraMovement dir, float offset);
    void rotate(float offset_x, float offset_y);
    glm::mat4 getViewMatrix() const;
    const glm::vec3& getPosition() const;

private:
    void update();

private:
    glm::vec3   m_pos,
                m_orientation,
                m_worldUp,      // used for m_right calculation
                m_up,           // used for view matrix calculation
                m_right;        // used for left-right movement calculation

    float       m_yaw,
                m_pitch;

    bool        m_canFly;
};

#endif // CAMERA_H_INCLUDED
