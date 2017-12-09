#include "camera.h"
#include "glad/glad.h"
#include <cmath>

Camera::Camera(const glm::vec3& position, float yaw, float pitch)
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

void Camera::rotate(float offsYaw, float offsPitch)
{
    m_yaw += offsYaw;
    m_pitch += offsPitch;
    validateYawPitch();
    update();
}

void Camera::setRotation(float yaw, float pitch)
{
    m_yaw = yaw;
    m_pitch = pitch;
    validateYawPitch();
    update();
}

void Camera::validateYawPitch()
{
    m_yaw = std::fmod(m_yaw, 360.0f);
    if (m_pitch > 89.9f) m_pitch = 89.9f;
    if (m_pitch < -89.9f) m_pitch = -89.9f;
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

void Camera::setDirection(const glm::vec3& direction)
{
    m_yaw = glm::degrees(std::atan2(direction.z, direction.x));

    float xz_len = std::sqrt(direction.z * direction.z + direction.x * direction.x);
    m_pitch = glm::degrees(std::atan2(direction.y, xz_len));

    validateYawPitch();
    update();
}
