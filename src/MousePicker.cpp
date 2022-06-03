#include "MousePicker.hpp"

#include <glm/gtx/string_cast.hpp>

Camera* MousePicker::s_Camera;
QB**** MousePicker::s_Cubies;

void MousePicker::init(Camera* camera, QB**** cubies)
{
    s_Camera = camera;
    s_Cubies = cubies;
}

void MousePicker::getXYRotations(FaceEnum face, glm::vec3& xRotation, glm::vec3& yRotation)
{
    glm::ivec3 frontVector = getClosestXYAxis(s_Camera->getFront());
    glm::ivec3 rightVector = getClosestXYAxis(glm::cross(glm::vec3(frontVector), glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::ivec3 faceNormal = Face::getNormal(face);

    switch (face)
    {
    case FaceEnum::UP:
        xRotation = -getClosestAxis(glm::cross(glm::vec3(rightVector), glm::vec3(faceNormal)));
        yRotation = -getClosestAxis(glm::cross(glm::vec3(frontVector), glm::vec3(faceNormal)));
        return;

    case FaceEnum::DOWN:
        xRotation = -getClosestAxis(glm::cross(glm::vec3(rightVector), glm::vec3(faceNormal)));
        yRotation = getClosestAxis(glm::cross(glm::vec3(frontVector), glm::vec3(faceNormal)));
        return;

    default:
        break;
    }

    glm::ivec3 chosenVector = frontVector;
    if (faceNormal == (rightVector * -1))
        chosenVector = frontVector * -1;
    else if ((chosenVector * -1) == faceNormal)
        chosenVector = rightVector;
    else if (chosenVector == faceNormal)
        chosenVector = rightVector * -1;

    glm::vec3 cross = glm::cross(glm::vec3(faceNormal), glm::vec3(chosenVector));
    xRotation = getClosestAxis(cross);
    yRotation = getClosestAxis((glm::cross(glm::vec3(faceNormal), xRotation)));
}

glm::ivec3 MousePicker::getClosestAxis(glm::vec3 vector)
{
    vector = glm::normalize(vector);

    float absX = std::abs(vector.x);
    float absY = std::abs(vector.y);
    float absZ = std::abs(vector.z);

    if (absX >= absZ && absX >= absY)
    {
        return glm::ivec3((1.0f / vector.x) * absX, 0, 0);
    }
    else if (absZ > absX && absZ > absY)
    {
        return glm::ivec3(0, 0, (1.0f / vector.z) * absZ);
    }
    else
    {
        return glm::ivec3(0, (1.0f / vector.y) * absY, 0);
    }
}

glm::ivec3 MousePicker::getClosestXYAxis(glm::vec3 vector)
{
    vector = glm::normalize(vector);

    float absX = std::abs(vector.x);
    float absZ = std::abs(vector.z);

    if (absX > absZ)
    {
        return glm::ivec3((1.0f / vector.x) * absX, 0, 0);
    }
    else
    {
        return glm::ivec3(0, 0, (1.0f / vector.z) * absZ);
    }
}
