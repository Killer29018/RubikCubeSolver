#ifndef MOUSE_PICKER_HPP
#define MOUSE_PICKER_HPP

#include "Enums.hpp"
#include "QB.hpp"
#include "Camera.hpp"
#include "Util.hpp"

#include <glad/gl.h>
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

struct CubeManager;

class MousePicker
{
public:
    static MousePickerStruct pickedObject;

    static float movementThreshold;
private:
    static Camera* s_Camera;
    static QB** s_Cubies;
    static uint16_t s_Size;

    static CubeManager* s_CubeManager;

    static bool s_MousePickEnabled;
public:
    static void init(Camera* camera, CubeManager* cubeManager);

    static void startPicking(uint32_t fbo, glm::vec2 mousePosition);
    static void mouseMoved(glm::vec2 mouseOffset);
    static void endPicking();

    static void getXYRotations(FaceEnum face, glm::vec3& xRotation, glm::vec3& yRotation);

    static bool qbPartOfSlice(glm::ivec3 index);
private:
    MousePicker() = default;

    static glm::ivec3 getClosestAxis(glm::vec3 vector);
    static glm::ivec3 getClosestXYAxis(glm::vec3 vector);

    static FaceEnum getPickedSliceFace();
    static int getPickedSliceIndex();
};

#endif
