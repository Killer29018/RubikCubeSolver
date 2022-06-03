#include "QB.hpp"

#include <glm/gtx/quaternion.hpp>

#include <glm/gtx/string_cast.hpp>

void QB::addFace(FaceEnum face)
{
    activeFaces |= face;
    m_FaceCount++;
}

void QB::rotate(glm::ivec3 rotation, float percentage, int8_t angleMult)
{
    glm::vec3 rotationF = glm::vec3(rotation.x, rotation.y, rotation.z);
    float angle = 90.0f * angleMult;
    if (percentage < 1.0f)
    {
        angle *= percentage;
        m_TempRotation = glm::angleAxis(glm::radians(angle), rotationF);
    }
    else
    {
        m_CurrentRotation = glm::angleAxis(glm::radians(angle), rotationF) * m_CurrentRotation;
        m_TempRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }
}

void QB::rotateCurrent(glm::ivec3 rotation, int8_t angleMult)
{
    glm::vec3 rotationF = glm::vec3(rotation.x, rotation.y, rotation.z);
    float angle = 90.0f * angleMult;
    m_FutureRotation = glm::angleAxis(glm::radians(angle), rotationF) * m_FutureRotation;
}

void QB::draw(KRE::Shader& shader, glm::quat customRotation)
{
    glm::mat4 tempRotation = glm::toMat4(m_TempRotation);

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 rotation = glm::toMat4(m_CurrentRotation);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.99f));

    glm::mat4 model = glm::toMat4(customRotation) * tempRotation * translation * rotation * scale;

    shader.setUniformVector3("u_QBPosition", index);

    Face::draw(shader, activeFaces, model);
}

FaceEnum QB::getFacingSide(FaceEnum face)
{
    glm::ivec3 normal = getFaceNormal(face);

    if (normal.x == 1)
        return FaceEnum::RIGHT;
    else if (normal.x == -1)
        return FaceEnum::LEFT;
    else if (normal.y == 1)
        return FaceEnum::UP;
    else if (normal.y == -1)
        return FaceEnum::DOWN;
    else if (normal.z == 1)
        return FaceEnum::FRONT;
    else if (normal.z == -1)
        return FaceEnum::BACK;

    else
        return static_cast<FaceEnum>(0);
}

glm::ivec3 QB::getFaceNormal(FaceEnum face)
{
    if ((activeFaces & face) != face)
        return glm::vec3(0, 0, 0);

    // Face* f = &m_Faces.at(face);

    glm::vec4 newFacing = glm::vec4(Face::getNormal(face), 0.0f);
    newFacing = glm::rotate(m_FutureRotation, newFacing);

    glm::ivec3 returnVector;
    returnVector.x = std::round(newFacing.x);
    returnVector.y = std::round(newFacing.y);
    returnVector.z = std::round(newFacing.z);
    return returnVector;
}
