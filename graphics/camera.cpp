#include "camera.h"
#include "glad/glad.h"

Camera::Camera(const glm::vec3 & position, float yaw, float pitch)
    : m_pos(position)
    , m_orientation({0.0f, 0.0f, -1.0f})
    , m_worldUp({0.0f, 1.0f, 0.0f})
    , m_yaw(yaw)
    , m_pitch(pitch)
{
    validateYawPitch();
    update();
}

void Camera::move(CameraMovement dir, float offset)
{
    float vel = offset;
    switch (dir)
    {
    case Forward:
        m_pos += vel * m_orientation;
        break;
    case Back:
        m_pos -= vel * m_orientation;
        break;
    case Left:
        m_pos -= vel * m_right;
        break;
    case Right:
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
