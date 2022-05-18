#include "QB.hpp"

#include <glm/gtx/quaternion.hpp>

#include "InnerCube.hpp"

#include <glm/gtx/string_cast.hpp>

void QB::addFace(FaceEnum face)
{
    if ((face & FaceEnum::UP) == FaceEnum::UP)
    {
        TopFace f(true);
        m_Faces[FaceEnum::UP] = f;
        activeFaces |= FaceEnum::UP;
    }
    if ((face & FaceEnum::DOWN) == FaceEnum::DOWN)
    {
        BottomFace f(true);
        m_Faces[FaceEnum::DOWN] = f;
        activeFaces |= FaceEnum::DOWN;
    }
    if ((face & FaceEnum::FRONT) == FaceEnum::FRONT)
    {
        FrontFace f(true);
        m_Faces[FaceEnum::FRONT] = f;
        activeFaces |= FaceEnum::FRONT;
    }
    if ((face & FaceEnum::BACK) == FaceEnum::BACK)
    {
        BackFace f(true);
        m_Faces[FaceEnum::BACK] = f;
        activeFaces |= FaceEnum::BACK;
    }
    if ((face & FaceEnum::LEFT) == FaceEnum::LEFT)
    {
        LeftFace f(true);
        m_Faces[FaceEnum::LEFT] = f;
        activeFaces |= FaceEnum::LEFT;
    }
    if ((face & FaceEnum::RIGHT) == FaceEnum::RIGHT)
    {
        RightFace f(true);
        m_Faces[FaceEnum::RIGHT] = f;
        activeFaces |= FaceEnum::RIGHT;
    }
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
        m_TotalRotation = glm::angleAxis(glm::radians(angle), rotationF) * m_TotalRotation;
        m_TempRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }
}

void QB::draw(KRE::Shader& shader)
{
    glm::mat4 model(1.0f);
    glm::mat4 translation(1.0f);
    glm::mat4 scale(1.0f);

    glm::mat4 rotation(1.0f);

    rotation = glm::toMat4(m_TotalRotation);
    glm::mat4 tempRotation = glm::toMat4(m_TempRotation);
    translation = glm::translate(translation, pos);
    scale = glm::scale(scale, glm::vec3(0.99f));
    model = tempRotation * translation * rotation * scale;

    shader.bind();

    shader.setUniformMatrix4("u_Model", model);

    if ((activeFaces & FaceEnum::UP) == FaceEnum::UP)
        m_Faces.at(FaceEnum::UP).draw(shader);
    if ((activeFaces & FaceEnum::DOWN) == FaceEnum::DOWN)
        m_Faces.at(FaceEnum::DOWN).draw(shader);
    if ((activeFaces & FaceEnum::FRONT) == FaceEnum::FRONT)
        m_Faces.at(FaceEnum::FRONT).draw(shader);
    if ((activeFaces & FaceEnum::BACK) == FaceEnum::BACK)
        m_Faces.at(FaceEnum::BACK).draw(shader);
    if ((activeFaces & FaceEnum::LEFT) == FaceEnum::LEFT)
        m_Faces.at(FaceEnum::LEFT).draw(shader);
    if ((activeFaces & FaceEnum::RIGHT) == FaceEnum::RIGHT)
        m_Faces.at(FaceEnum::RIGHT).draw(shader);

    InnerCube::draw(shader, model);
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

    Face* f = &m_Faces.at(face);

    glm::vec3 newFacing = glm::vec3(glm::vec4(f->facing, 1.0f) * glm::toMat4(m_TotalRotation));

    glm::ivec3 returnVector;
    returnVector.x = std::round(newFacing.x);
    returnVector.y = std::round(newFacing.y);
    returnVector.z = std::round(newFacing.z);
    return returnVector;
}
