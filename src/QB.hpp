#ifndef QB_HPP
#define QB_HPP

#include <unordered_map>
#include <glm/glm.hpp>
#include "KRE/KRE.hpp"

#include "Face.hpp"

class QB
{
public:
    FaceFlag activeFaces = static_cast<FaceFlag>(0);
    glm::vec3 pos;
private:
    std::unordered_map<FaceFlag, Face> m_Faces;
    // std::vector<glm::vec3> m_Rotations;
    glm::quat m_TotalRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
public:
    QB() = default;
    QB(glm::vec3 pos) : pos(pos) {};

    void addFace(FaceFlag face);

    void rotate(glm::ivec3 rotation);

    void draw(KRE::Shader& shader);
private:
};

#endif
