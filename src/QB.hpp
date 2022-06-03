#ifndef QB_HPP
#define QB_HPP

#include <unordered_map>
#include <glm/glm.hpp>
#include "KRE/KRE.hpp"

#include "Face.hpp"

class QB
{
public:
    FaceEnum activeFaces = static_cast<FaceEnum>(0);
    glm::vec3 pos;
    glm::ivec3 index;
private:
    size_t m_FaceCount = 0;

    // AnimationRotation
    glm::quat m_CurrentRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::quat m_TempRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    // Future
    glm::quat m_FutureRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
public:
    QB() = default;
    QB(glm::vec3 pos, glm::ivec3 index) : pos(pos), index(index) {};

    void addFace(FaceEnum face);

    void rotate(glm::ivec3 rotation, float percentage, int8_t angleMult = 1);
    void rotateCurrent(glm::ivec3 rotation, int8_t angleMult);

    void draw(KRE::Shader& shader, glm::quat customRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    size_t getFaceCount() { return m_FaceCount; }
    bool hasFace(FaceEnum face) { return (activeFaces & face) == face; }

    FaceEnum getFacingSide(FaceEnum face);
    glm::ivec3 getFaceNormal(FaceEnum face);
private:
};

#endif
