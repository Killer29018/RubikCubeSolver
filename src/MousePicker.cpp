#include "MousePicker.hpp"

void MousePicker::getXYRotations(FaceEnum face, glm::vec3& xRotation, glm::vec3& yRotation)
{
    switch (face)
    {
    case FaceEnum::UP:
        xRotation = glm::vec3(0.0f, 0.0f, -1.0f);
        yRotation = glm::vec3(-1.0f, 0.0f, 0.0f);
        break;
    case FaceEnum::DOWN:
        xRotation = glm::vec3(0.0f, 0.0f, 1.0f);
        yRotation = glm::vec3(-1.0f, 0.0f, 0.0f);
        break;

    case FaceEnum::RIGHT:
        xRotation = glm::vec3(0.0f, 1.0f, 0.0f);
        yRotation = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    case FaceEnum::LEFT:
        xRotation = glm::vec3(0.0f, 1.0f, 0.0f);
        yRotation = glm::vec3(0.0f, 0.0f, -1.0f);
        break;

    case FaceEnum::FRONT:
        xRotation = glm::vec3(0.0f, 1.0f, 0.0f);
        yRotation = glm::vec3(-1.0f, 0.0f, 0.0f);
        break;
    case FaceEnum::BACK:
        xRotation = glm::vec3(0.0f, 1.0f, 0.0f);
        yRotation = glm::vec3(1.0f, 0.0f, 0.0f);
        break;
    }
}
