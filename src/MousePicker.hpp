#ifndef MOUSE_PICKER_HPP
#define MOUSE_PICKER_HPP

#include "Enums.hpp"
#include "QB.hpp"
#include "Camera.hpp"

#include <glm/glm.hpp>

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
private:
    static Camera* s_Camera;
    static QB**** s_Cubies;
public:
    static void init(Camera* camera, QB**** cubies);
    static void getXYRotations(FaceEnum face, glm::vec3& xRotation, glm::vec3& yRotation);
private:
    MousePicker() = default;

    static glm::ivec3 getClosestAxis(glm::vec3 vector);
    static glm::ivec3 getClosestXYAxis(glm::vec3 vector);
};

#endif
