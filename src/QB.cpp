#include "QB.hpp"

#include <glm/gtx/quaternion.hpp>

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

void QB::rotate(glm::ivec3 rotation, float percentage)
{
    glm::vec3 rotationF = glm::vec3(rotation.x, rotation.y, rotation.z);
    if (percentage < 1.0f)
    {
        m_TempRotation = glm::angleAxis(glm::radians(90.0f * percentage), rotationF);
    }
    else
    {
        m_TotalRotation = glm::angleAxis(glm::radians(90.0f), rotationF) * m_TotalRotation;
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
}
