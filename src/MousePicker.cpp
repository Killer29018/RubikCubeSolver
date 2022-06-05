#include "MousePicker.hpp"

#include <glm/gtx/string_cast.hpp>
#include "CubeManager.hpp"

MousePickerStruct MousePicker::pickedObject;
float MousePicker::movementThreshold = 30.0f;

Camera* MousePicker::s_Camera;
QB**** MousePicker::s_Cubies;
uint16_t MousePicker::s_Size = 0;

bool MousePicker::s_MousePickEnabled = false;

void MousePicker::init(Camera* camera, QB**** cubies, uint16_t size)
{
    s_Camera = camera;
    s_Cubies = cubies;
    s_Size = size;
}

void MousePicker::startPicking(uint32_t fbo, glm::vec2 mousePosition)
{
    if (s_MousePickEnabled)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT1);

    glm::vec4 pixelData(0.0f);
    glReadPixels((int)mousePosition.x, (int)mousePosition.y, 1, 1, GL_RGBA, GL_FLOAT, glm::value_ptr(pixelData));

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (pixelData.x < 0.0f || pixelData.y < 0.0f)
        return;

    s_MousePickEnabled = true;

    pickedObject.pickedFace = static_cast<FaceEnum>(pixelData.x);

    glm::ivec3 index;
    index.x = pixelData.y;
    index.y = pixelData.z;
    index.z = pixelData.w;

    pickedObject.index = index;
    pickedObject.qb = s_Cubies[index.z][index.y][index.x];

    pickedObject.mouseOffset = glm::vec2(0.0f);
}

void MousePicker::mouseMoved(glm::vec2 mouseOffset)
{
    if (!s_MousePickEnabled)
        return;

    pickedObject.mouseOffset += mouseOffset;

    if (std::abs(pickedObject.mouseOffset.x) >= movementThreshold && !pickedObject.moving)
    {
        // Moved enough on the X to start moving
        pickedObject.moving = true;
        pickedObject.movingX = true;
    }
    else if (std::abs(pickedObject.mouseOffset.x) < movementThreshold && pickedObject.moving && pickedObject.movingX)
    {
        // Moving on X but no longer enough
        pickedObject.moving = false;
        pickedObject.movingX = false;
    }

    if (std::abs(pickedObject.mouseOffset.y) >= movementThreshold && !pickedObject.moving)
    {
        // Moved enough on Y to start Moving
        pickedObject.moving = true;
        pickedObject.movingX = false;
    }
    else if (std::abs(pickedObject.mouseOffset.y) < movementThreshold && pickedObject.moving && !pickedObject.movingX)
    {
        // Moving on Y but no longer enough
        pickedObject.moving = false;
        pickedObject.movingX = false;
    }

    if (!pickedObject.moving)
    {
        pickedObject.angleAxis = glm::vec3(0.0f);
        return;
    }

    static glm::vec3 xRotation;
    static glm::vec3 yRotation;

    getXYRotations(pickedObject.pickedFace, xRotation, yRotation);
    float offset = 0.0f;

    if (pickedObject.movingX)
    {
        pickedObject.angleAxis = xRotation;
        offset = pickedObject.mouseOffset.x;
    }
    else
    {
        pickedObject.angleAxis = yRotation;
        offset = pickedObject.mouseOffset.y;
    }

    pickedObject.angle = offset / 2.0f;
}

void MousePicker::endPicking()
{
    if (!s_MousePickEnabled)
        return;

    FaceEnum slice = getPickedSliceFace();
    int sliceIndex = getPickedSliceIndex();

    int angleMult = (int)std::round(std::fmod(pickedObject.angle, 360.0f) / 90.0f);

    RotationEnum rotation = RotationEnum::NONE;

    if (angleMult == -1 || angleMult == 3)
        rotation = RotationEnum::PRIME;
    else if (angleMult == 1)
        rotation = RotationEnum::NORMAL;
    else if (angleMult == 2)
        rotation = RotationEnum::TWICE;

    glm::ivec3 facing = getClosestXYAxis(s_Camera->getFront());

    switch (slice)
    {
    case FaceEnum::UP:
        rotation = reverseRotation(rotation);
        break;
    case FaceEnum::RIGHT:
        break;
    case FaceEnum::FRONT:
        rotation = reverseRotation(rotation);
        break;
    }

    if (facing == glm::ivec3(0, 0, 1) || facing == glm::ivec3(1, 0, 0))
    {
        rotation = reverseRotation(rotation);
    }

    Move move = Move(slice, rotation, sliceIndex);
    move.time = 1.0f;
    CubeManager::doMove(move);

    s_MousePickEnabled = false;
    pickedObject.moving = false;
    pickedObject.movingX = false;
    pickedObject.angle = 0.0f;
}

void MousePicker::getXYRotations(FaceEnum face, glm::vec3& xRotation, glm::vec3& yRotation)
{
    glm::ivec3 frontVector = getClosestXYAxis(s_Camera->getFront());
    glm::ivec3 rightVector = getClosestXYAxis(glm::cross(glm::vec3(frontVector), glm::vec3(0.0f, 1.0f, 0.0f)));

    FaceEnum currentFace = pickedObject.qb->getFacingSide(face);
    glm::ivec3 faceNormal = pickedObject.qb->getFaceNormal(face);

    switch (currentFace)
    {
    case FaceEnum::UP:
        xRotation = -getClosestAxis(glm::cross(glm::vec3(rightVector), glm::vec3(faceNormal)));
        yRotation = -getClosestAxis(glm::cross(glm::vec3(frontVector), glm::vec3(faceNormal)));
        return;

    case FaceEnum::DOWN:
        xRotation = -getClosestAxis(glm::cross(glm::vec3(rightVector), glm::vec3(faceNormal)));
        yRotation = getClosestAxis(glm::cross(glm::vec3(frontVector), glm::vec3(faceNormal)));
        return;

    default:
        break;
    }

    glm::ivec3 chosenVector = frontVector;
    if (faceNormal == (rightVector * -1))
        chosenVector = frontVector * -1;
    else if ((chosenVector * -1) == faceNormal)
        chosenVector = rightVector;
    else if (chosenVector == faceNormal)
        chosenVector = rightVector * -1;

    glm::vec3 cross = glm::cross(glm::vec3(faceNormal), glm::vec3(chosenVector));
    xRotation = getClosestAxis(cross);
    yRotation = getClosestAxis((glm::cross(glm::vec3(faceNormal), xRotation)));
}

bool MousePicker::qbPartOfSlice(glm::ivec3 index)
{
    if (!pickedObject.moving || !s_MousePickEnabled)
        return false;

    FaceEnum slice = getPickedSliceFace();

    switch (slice)
    {
    case FaceEnum::UP:
        return (index.y == pickedObject.index.y);
        break;
    case FaceEnum::RIGHT:
        return (index.x == pickedObject.index.x);
        break;
    case FaceEnum::FRONT:
        return (index.z == pickedObject.index.z);
        break;

    default:
        assert(false && "Not possible");
        return false;
    }
}

glm::ivec3 MousePicker::getClosestAxis(glm::vec3 vector)
{
    vector = glm::normalize(vector);

    float absX = std::abs(vector.x);
    float absY = std::abs(vector.y);
    float absZ = std::abs(vector.z);

    if (absX >= absZ && absX >= absY)
    {
        return glm::ivec3((1.0f / vector.x) * absX, 0, 0);
    }
    else if (absZ > absX && absZ > absY)
    {
        return glm::ivec3(0, 0, (1.0f / vector.z) * absZ);
    }
    else
    {
        return glm::ivec3(0, (1.0f / vector.y) * absY, 0);
    }
}

glm::ivec3 MousePicker::getClosestXYAxis(glm::vec3 vector)
{
    vector = glm::normalize(vector);

    float absX = std::abs(vector.x);
    float absZ = std::abs(vector.z);

    if (absX > absZ)
    {
        return glm::ivec3((1.0f / vector.x) * absX, 0, 0);
    }
    else
    {
        return glm::ivec3(0, 0, (1.0f / vector.z) * absZ);
    }
}

FaceEnum MousePicker::getPickedSliceFace()
{
    FaceEnum sliceFace = pickedObject.qb->getFacingSide(pickedObject.pickedFace);

    switch (sliceFace)
    {
    case FaceEnum::UP:
    case FaceEnum::DOWN:
        {
            glm::ivec3 front = getClosestXYAxis(s_Camera->getFront());
            if (front == glm::ivec3(1, 0, 0) || front == glm::ivec3(-1, 0, 0)) // Facing left or right face
            {
                return (!pickedObject.movingX) ? FaceEnum::FRONT : FaceEnum::RIGHT;
            }
            return (pickedObject.movingX) ? FaceEnum::FRONT : FaceEnum::RIGHT;
        }

    case FaceEnum::LEFT:
    case FaceEnum::RIGHT:
        return (pickedObject.movingX) ? FaceEnum::UP : FaceEnum::FRONT;

    case FaceEnum::FRONT:
    case FaceEnum::BACK:
        return (pickedObject.movingX) ? FaceEnum::UP : FaceEnum::RIGHT;
    }
}

int MousePicker::getPickedSliceIndex()
{
    FaceEnum slice = getPickedSliceFace();
    switch (slice)
    {
    case FaceEnum::UP:
        return s_Size - pickedObject.index.y - 1;
    case FaceEnum::RIGHT:
        return s_Size - pickedObject.index.x - 1;
    case FaceEnum::FRONT:
        return s_Size - pickedObject.index.z - 1;

    default:
        assert(false && "Not possible");
    }
}
