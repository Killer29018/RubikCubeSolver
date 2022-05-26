#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    float mouseSensitivity = 0.3f;
    float distance = 5.0f;

    float minDistance = 3.0f;
    float maxDistance = 20.0f;
private:
    glm::vec2 m_WindowSize;

    glm::vec3 m_Position;
    glm::vec3 m_Center;

    glm::vec3 m_Front;

    float m_vFOV = 90.0f;
    float m_NearClipping = 0.1f;
    float m_FarClipping = 100.0f;

    float m_Pitch = 0.0f;
    float m_Yaw = 90.0f;
public:
    Camera(glm::vec2 windowSize);
    ~Camera() = default;

    void setAngle(float yaw, float pitch);
    void processAngleChange(float xOffset, float yOffset);

    void processScrollWheel(float yOffset);

    glm::mat4 getProjectionMatrix();
    glm::mat4 getViewMatrix();
private:
    void recalculateVectors();
};

#endif
