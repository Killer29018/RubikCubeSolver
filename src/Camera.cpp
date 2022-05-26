#include "Camera.hpp"

#include <algorithm>
#include <iostream>

Camera::Camera(glm::vec2 windowSize)
    : m_WindowSize(windowSize)
{
    recalculateVectors();
}

void Camera::setAngle(float yaw, float pitch)
{
    m_Yaw = yaw;
    m_Pitch = pitch;

    recalculateVectors();
}

void Camera::processAngleChange(float xOffset, float yOffset)
{
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    m_Yaw += xOffset;
    m_Pitch -= yOffset;

    m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);

    recalculateVectors();
}

void Camera::processScrollWheel(float yOffset)
{
    distance += yOffset;

    distance = std::clamp(distance, minDistance, maxDistance);

    recalculateVectors();
}

glm::mat4 Camera::getProjectionMatrix()
{
    return glm::perspective(glm::radians(m_vFOV), m_WindowSize.x / m_WindowSize.y, m_NearClipping, m_FarClipping);
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(m_Position, m_Center, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::recalculateVectors()
{
    float pitch = m_Pitch;
    float yaw = m_Yaw;
    m_Position.x = distance * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    m_Position.y = distance * sin(glm::radians(pitch));
    m_Position.z = distance * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    m_Front = glm::normalize(m_Center - m_Position);
}
