#include "camera.h"
#include "glad/glad.h"

Camera::Camera(const glm::vec3 & position, float yaw, float pitch)
    : m_pos(position)
    , m_orientation({0.0f, 0.0f, -1.0f})
    , m_worldUp({0.0f, 1.0f, 0.0f})
    , m_yaw(yaw)
    , m_pitch(pitch)
    , m_canFly(true)
{
    update();
}

void Camera::move(CameraMovement dir, float dt_sec)
{
    float vel = dt_sec * 50.0f;
    switch (dir)
    {
    case CameraMovement::FORWARD:
        m_pos += vel * (m_canFly ? m_orientation : glm::normalize(glm::cross(m_worldUp, m_right)));
        break;
    case CameraMovement::BACK:
        m_pos -= vel * (m_canFly ? m_orientation : glm::normalize(glm::cross(m_worldUp, m_right)));
        break;
    case CameraMovement::LEFT:
        m_pos -= vel * m_right;
        break;
    case CameraMovement::RIGHT:
        m_pos += vel * m_right;
        break;
    default:
        break;
    }
}

void Camera::rotate(float offset_x, float offset_y)
{
    m_yaw += offset_x * 0.1f;
    m_yaw = std::fmod(m_yaw, 360.0f);
    m_pitch -= offset_y * 0.1f;

    if (m_pitch > 89.9f) m_pitch = 89.9f;
    if (m_pitch < -89.9f) m_pitch = -89.9f;

    update();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_pos, m_pos + m_orientation, m_up);
}

void Camera::update()
{
    m_orientation.x = std::cos(glm::radians(m_pitch)) * std::cos(glm::radians(m_yaw));
    m_orientation.y = std::sin(glm::radians(m_pitch));
    m_orientation.z = std::cos(glm::radians(m_pitch)) * std::sin(glm::radians(m_yaw));

    m_right = glm::normalize(glm::cross(m_orientation, m_worldUp));
    m_up    = glm::normalize(glm::cross(m_right, m_orientation));
}
