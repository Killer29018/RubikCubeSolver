#ifndef MOUSE_PICKER_HPP
#define MOUSE_PICKER_HPP

#include "Enums.hpp"

#include <glm/glm.hpp>

#include "QB.hpp"

struct MousePickerStruct
{
    glm::ivec3 index;
    FaceEnum pickedFace;
    QB* qb;

    float angle;
    glm::vec3 angleAxis;

    glm::vec2 mouseOffset;

    bool moving = false;
    bool movingX = false;
};

class MousePicker
{
public:
    static void getXYRotations(FaceEnum face, glm::vec3& xRotation, glm::vec3& yRotation);
private:
    MousePicker() = default;
};

#endif
