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
private:
    std::unordered_map<FaceEnum, Face> m_Faces;
    // std::vector<glm::vec3> m_Rotations;
    glm::quat m_TotalRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::quat m_TempRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
public:
    QB() = default;
    QB(glm::vec3 pos) : pos(pos) {};

    void addFace(FaceEnum face);

    void rotate(glm::ivec3 rotation, float percentage, int8_t angleMult = 1);

    void draw(KRE::Shader& shader);

    size_t getFaceCount() { return m_Faces.size(); }
    bool hasFace(FaceEnum face) { return (activeFaces & face) == face; }

    FaceEnum getSide(FaceEnum face);
    glm::ivec3 getFaceNormal(FaceEnum face);
private:
};

#endif
