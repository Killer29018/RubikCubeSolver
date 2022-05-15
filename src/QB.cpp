#include "QB.hpp"

#include <glm/gtx/quaternion.hpp>

void QB::addFace(FaceFlag face)
{
    if ((face & FaceFlag::Top) == FaceFlag::Top)
    {
        TopFace f(true);
        m_Faces[FaceFlag::Top] = f;
        activeFaces |= FaceFlag::Top;
    }
    if ((face & FaceFlag::Bottom) == FaceFlag::Bottom)
    {
        BottomFace f(true);
        m_Faces[FaceFlag::Bottom] = f;
        activeFaces |= FaceFlag::Bottom;
    }
    if ((face & FaceFlag::Front) == FaceFlag::Front)
    {
        FrontFace f(true);
        m_Faces[FaceFlag::Front] = f;
        activeFaces |= FaceFlag::Front;
    }
    if ((face & FaceFlag::Back) == FaceFlag::Back)
    {
        BackFace f(true);
        m_Faces[FaceFlag::Back] = f;
        activeFaces |= FaceFlag::Back;
    }
    if ((face & FaceFlag::Left) == FaceFlag::Left)
    {
        LeftFace f(true);
        m_Faces[FaceFlag::Left] = f;
        activeFaces |= FaceFlag::Left;
    }
    if ((face & FaceFlag::Right) == FaceFlag::Right)
    {
        RightFace f(true);
        m_Faces[FaceFlag::Right] = f;
        activeFaces |= FaceFlag::Right;
    }
}

void QB::rotate(glm::ivec3 rotation)
{
    glm::vec3 rotationF = glm::vec3(rotation.x, rotation.y, rotation.z);
    m_TotalRotation = glm::angleAxis(glm::radians(90.0f), rotationF) * m_TotalRotation;
}

void QB::draw(KRE::Shader& shader)
{
    glm::mat4 model(1.0f);
    glm::mat4 translation(1.0f);
    glm::mat4 scale(1.0f);

    glm::mat4 rotation(1.0f);

    rotation = glm::toMat4(m_TotalRotation);
    translation = glm::translate(translation, pos);
    scale = glm::scale(scale, glm::vec3(0.99f));
    model = translation * rotation * scale;

    shader.bind();

    shader.setUniformMatrix4("u_Model", model);

    if ((activeFaces & FaceFlag::Top) == FaceFlag::Top)
        m_Faces.at(FaceFlag::Top).draw(shader);
    if ((activeFaces & FaceFlag::Bottom) == FaceFlag::Bottom)
        m_Faces.at(FaceFlag::Bottom).draw(shader);
    if ((activeFaces & FaceFlag::Front) == FaceFlag::Front)
        m_Faces.at(FaceFlag::Front).draw(shader);
    if ((activeFaces & FaceFlag::Back) == FaceFlag::Back)
        m_Faces.at(FaceFlag::Back).draw(shader);
    if ((activeFaces & FaceFlag::Left) == FaceFlag::Left)
        m_Faces.at(FaceFlag::Left).draw(shader);
    if ((activeFaces & FaceFlag::Right) == FaceFlag::Right)
        m_Faces.at(FaceFlag::Right).draw(shader);
}
