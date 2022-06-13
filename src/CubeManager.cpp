#include "CubeManager.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

QB** CubeManager::s_Cubies;
QB** CubeManager::s_CurrentCubies;

std::vector<glm::ivec2> CubeManager::s_SwapIndices;

uint8_t CubeManager::s_Size;

bool CubeManager::s_MousePick = false;

void CubeManager::generate(uint8_t size)
{
    s_Size = size;

    assert(s_Size >= 2  && s_Size <= 3 && "Only a 3x3 or 2x2 Cube is currently supported");

    // s_Cubies = new QB***[s_Size];
    // s_CurrentCubies = new QB***[s_Size];

    float lowestX = (float)((s_Size - 1) / 2.0f) - (s_Size - 1);
    float lowestY = (float)((s_Size - 1) / 2.0f) - (s_Size - 1);
    float lowestZ = (float)((s_Size - 1) / 2.0f) - (s_Size - 1);

    s_Cubies = new QB*[s_Size * s_Size * s_Size];
    s_CurrentCubies = new QB*[s_Size * s_Size * s_Size];

    for (uint8_t z = 0; z < s_Size; z++)
    {
        for (uint8_t y = 0; y < s_Size; y++)
        {
            for (uint8_t x = 0; x < s_Size; x++)
            {
                glm::vec3 pos(lowestX + x, lowestY + y, lowestZ + z);
                glm::ivec3 index(x, y, z);

                QB* qb = new QB(pos, index);

                s_Cubies[getIndex(index)] = qb;
                s_CurrentCubies[getIndex(index)] = qb;
            }
        }
    }

    // Top | Bottom
    for (uint8_t z = 0; z < s_Size; z++)
    {
        for (uint8_t x = 0; x < s_Size; x++)
        {
            s_Cubies[getIndex(x, 0, z)]->addFace(FaceEnum::DOWN);
            s_Cubies[getIndex(x, s_Size - 1, z)]->addFace(FaceEnum::UP);
        }
    }

    // Left | Right
    for (uint8_t z = 0; z < s_Size; z++)
    {
        for (uint8_t y = 0; y < s_Size; y++)
        {
            s_Cubies[getIndex(0, y, z)]->addFace(FaceEnum::LEFT);
            s_Cubies[getIndex(s_Size - 1, y, z)]->addFace(FaceEnum::RIGHT);
        }
    }

    // Front | Back
    for (uint8_t y = 0; y < s_Size; y++)
    {
        for (uint8_t x = 0; x < s_Size; x++)
        {
            s_Cubies[getIndex(x, y, 0)]->addFace(FaceEnum::BACK);
            s_Cubies[getIndex(x, y, s_Size - 1)]->addFace(FaceEnum::FRONT);
        }
    }


    // Swap Indices
    for (uint8_t i = 0; i < s_Size; i++)
    {
        s_SwapIndices.emplace_back(i, 0);
    }
    for (uint8_t i = 1; i < s_Size; i++)
    {
        s_SwapIndices.emplace_back(s_Size - 1, i);
    }
    for (uint8_t i = 1; i < s_Size; i++)
    {
        s_SwapIndices.emplace_back(s_Size - 1 - i, s_Size - 1);
    }
    for (uint8_t i = 1; i < s_Size - 1; i++)
    {
        s_SwapIndices.emplace_back(0, s_Size - 1 - i);
    }

    Solver::loadCube(s_CurrentCubies, s_Size);
}

void CubeManager::destroy()
{
    for (int z = 0; z < s_Size - 1; z++)
    {
        for (int y = 0; y < s_Size - 1; y++)
        {
            for (int x = 0; x < s_Size - 1; x++)
            {
                delete s_Cubies[getIndex(x, y, z)];
            }
        }
    }
    delete[] s_Cubies;
    delete[] s_CurrentCubies;

    s_SwapIndices.clear();
}

void CubeManager::draw(KRE::Shader& shader)
{
    for (int z = 0; z < s_Size; z++) // z
    {
        for (int y = 0; y < s_Size; y++) // y
        {
            for (int x = 0; x < s_Size; x++) // x
            {
                glm::quat customRotation(1.0f, 0.0f, 0.0f, 0.0f);

                glm::ivec3 index(x, y, z);

                if (MousePicker::qbPartOfSlice(index))
                    customRotation = glm::angleAxis(glm::radians(MousePicker::pickedObject.angle), MousePicker::pickedObject.angleAxis);

                s_Cubies[getIndex(index)]->draw(shader, customRotation);
            }
        }
    }
}

void CubeManager::update(float dt)
{
    if (MoveManager::isEmpty())
        return;

    rotateAnimate(dt);
}

void CubeManager::applyMoves(std::string input)
{
    for (size_t i = 0; i <= input.size() - 1;)
    {
        char c = input[i];
        char nextC;
        if (i == input.size() - 1)
            nextC = ' ';
        else
            nextC = input[i + 1];

        RotationEnum rotation = RotationEnum::NONE;
        switch (nextC)
        {
        case '\'':
            rotation = RotationEnum::PRIME;
            i += 3;
            break;
        case ' ':
            rotation = RotationEnum::NORMAL;
            i += 2;
            break;
        case '2':
            rotation = RotationEnum::TWICE;
            i += 3;
            break;
        }

        switch (c)
        {
            case 'U': doMove({ FaceEnum::UP,       rotation }); break;
            case 'D': doMove({ FaceEnum::DOWN,     rotation }); break;
            case 'L': doMove({ FaceEnum::LEFT,     rotation }); break;
            case 'R': doMove({ FaceEnum::RIGHT,    rotation }); break;
            case 'F': doMove({ FaceEnum::FRONT,    rotation }); break;
            case 'B': doMove({ FaceEnum::BACK,     rotation }); break;

            case 'u': doMove({ FaceEnum::UP,       rotation, 1, SliceMode::ALL }); break;
            case 'd': doMove({ FaceEnum::DOWN,     rotation, 1, SliceMode::ALL }); break;
            case 'l': doMove({ FaceEnum::LEFT,     rotation, 1, SliceMode::ALL }); break;
            case 'r': doMove({ FaceEnum::RIGHT,    rotation, 1, SliceMode::ALL }); break;
            case 'f': doMove({ FaceEnum::FRONT,    rotation, 1, SliceMode::ALL }); break;
            case 'b': doMove({ FaceEnum::BACK,     rotation, 1, SliceMode::ALL }); break;

            case 'M': doMove({ FaceEnum::LEFT,     rotation, 1 }); break;
            case 'E': doMove({ FaceEnum::DOWN,     rotation, 1 }); break;
            case 'S': doMove({ FaceEnum::FRONT,    rotation, 1 }); break;
        }
    }
}

void CubeManager::doMove(Move move)
{
    rotateCurrent(move);
    MoveManager::addMove(move);
}

glm::ivec2 CubeManager::getLocalPos(glm::ivec3 pos, FaceEnum face)
{
    glm::ivec2 localPos(0);
    switch (face)
    {
    case FaceEnum::UP: // Y is locked
        localPos.x = pos.x;
        localPos.y = s_Size - 1 - pos.z;
        break;
    case FaceEnum::DOWN:
        localPos.x = pos.x;
        localPos.y = pos.z;
        break;

    case FaceEnum::LEFT: // X is locked
        localPos.x = pos.z;
        localPos.y = pos.y;
        break;
    case FaceEnum::RIGHT:
        localPos.x = s_Size - 1 - pos.z;
        localPos.y = pos.y;
        break;

    case FaceEnum::FRONT: // Z is locked
        localPos.x = pos.x;
        localPos.y = pos.y;
        break;
    case FaceEnum::BACK:
        localPos.x = s_Size - 1 - pos.x;
        localPos.y = pos.y;
        break;

    default:
        assert(false && "Not possible");
    }

    return localPos;
}

LocalEdgeEnum CubeManager::getLocalEdge(glm::ivec3 pos, FaceEnum face)
{
    glm::ivec2 localPos = getLocalPos(pos, face);

    // Fixed Size
    if (localPos.x == 0)
        return LocalEdgeEnum::LEFT;
    else if (localPos.y == 0)
        return LocalEdgeEnum::BOTTOM;

    // Variable Size
    switch (face)
    {
    case FaceEnum::UP:
    case FaceEnum::DOWN:
        if (localPos.x == s_Size - 1)
            return LocalEdgeEnum::RIGHT;
        else if (localPos.y == s_Size - 1)
            return LocalEdgeEnum::TOP;

    case FaceEnum::LEFT:
    case FaceEnum::RIGHT:
        if (localPos.x == s_Size - 1)
            return LocalEdgeEnum::RIGHT;
        else if (localPos.y == s_Size - 1)
            return LocalEdgeEnum::TOP;

    case FaceEnum::FRONT:
    case FaceEnum::BACK:
        if (localPos.x == s_Size - 1)
            return LocalEdgeEnum::RIGHT;
        else if (localPos.y == s_Size - 1)
            return LocalEdgeEnum::TOP;

    default:
        assert(false && "Impossible");
    }
}

LocalCornerEnum CubeManager::getLocalCorner(glm::ivec3 pos, FaceEnum face)
{
    glm::ivec2 localPos = getLocalPos(pos, face);

    if (localPos.x == 0 && localPos.y == 0)
        return LocalCornerEnum::BOTTOM_LEFT;

    switch (face)
    {
    case FaceEnum::UP:
    case FaceEnum::DOWN:
        if (localPos.x == 0 && localPos.y == s_Size - 1)
            return LocalCornerEnum::TOP_LEFT;
        else if (localPos.x == s_Size - 1 && localPos.y == 0)
            return LocalCornerEnum::BOTTOM_RIGHT;
        else
            return LocalCornerEnum::TOP_RIGHT;

    case FaceEnum::LEFT:
    case FaceEnum::RIGHT:
        if (localPos.x == 0 && localPos.y == s_Size - 1)
            return LocalCornerEnum::TOP_LEFT;
        else if (localPos.x == s_Size - 1 && localPos.y == 0)
            return LocalCornerEnum::BOTTOM_RIGHT;
        else
            return LocalCornerEnum::TOP_RIGHT;

    case FaceEnum::FRONT:
    case FaceEnum::BACK:
        if (localPos.x == 0 && localPos.y == s_Size - 1)
            return LocalCornerEnum::TOP_LEFT;
        else if (localPos.x == s_Size - 1 && localPos.y == 0)
            return LocalCornerEnum::BOTTOM_RIGHT;
        else
            return LocalCornerEnum::TOP_RIGHT;

    default:
        assert(false && "Not possible");
    }
}

size_t CubeManager::getIndex(uint16_t x, uint16_t y, uint16_t z)
{
    return x + (s_Size * y) + (s_Size * s_Size * z);
}

size_t CubeManager::getIndex(glm::ivec3 index)
{
    return getIndex(index.x, index.y, index.z);
}

glm::vec3 CubeManager::coordsToPosition(uint16_t x, uint16_t y, uint16_t z)
{
    glm::vec3 returnVec(0.0f);
    returnVec.x = (float)(((s_Size - 1) / 2.0f) - (s_Size - 1)) + x;
    returnVec.y = (float)(((s_Size - 1) / 2.0f) - (s_Size - 1)) + y;
    returnVec.z = (float)(((s_Size - 1) / 2.0f) - (s_Size - 1)) + z;

    return returnVec;
}

glm::ivec3 CubeManager::getCurrentIndex(uint16_t constant, glm::ivec3 axis, glm::ivec2 swapPosition)
{
    glm::ivec3 index(0);
    if (axis.x != 0)
    {
        index.z = swapPosition.x;
        index.y = swapPosition.y;
        index.x = constant;
    }
    else if (axis.y != 0)
    {
        index.z = swapPosition.x;
        index.y = constant;
        index.x = swapPosition.y;
    }
    else if (axis.z != 0)
    {
        index.z = constant;
        index.y = swapPosition.x;
        index.x = swapPosition.y;
    }

    return index;
}

void CubeManager::rotateAnimate(float dt)
{
    Move* move = MoveManager::getMove();

    if (MoveManager::moveFinished())
    {
        Move m = MoveManager::removeMove();
        move = &m;
    }

    rotate(&s_Cubies, *move, dt, true);
}

void CubeManager::rotateCurrent(Move& move)
{
    rotate(&s_CurrentCubies, move, 0.0f, false);
}

void CubeManager::rotate(QB*** cubies, Move& move, float dt, bool animate)
{
    if (move.rotation == RotationEnum::NONE)
    {
        return;
    }

    float percentage = move.time;
     
    glm::ivec3 rotationAxis;
    uint32_t position;

    int rotationInt = static_cast<int>(move.rotation);

    int count = 0;
    switch (move.sliceMode)
    {
    case SliceMode::SINGLE:
        count = 1;
        break;
    case SliceMode::ALL:
        count = move.slice + 1;
        break;
    }

    for (int i = 0; i < count; i++)
    {
        int slice = move.slice - i;
        switch (move.face)
        {
            case FaceEnum::UP:
                rotationAxis = glm::vec3(0, -1, 0);
                position = s_Size - 1 - slice;
                if (move.rotation == RotationEnum::PRIME)
                    swapCCW(cubies, position, rotationAxis, percentage, rotationInt, animate);
                else
                    swapCW(cubies, position, rotationAxis, percentage, rotationInt, animate);

                break;

            case FaceEnum::DOWN:
                rotationAxis = glm::vec3(0, 1, 0);
                position = slice;

                if (move.rotation == RotationEnum::PRIME)
                    swapCW(cubies, position, rotationAxis, percentage, rotationInt, animate);
                else
                    swapCCW(cubies, position, rotationAxis, percentage, rotationInt, animate);
                break;

            case FaceEnum::RIGHT:
                rotationAxis = glm::vec3(-1, 0, 0);
                position = s_Size - 1 - slice;
                if (move.rotation == RotationEnum::PRIME)
                    swapCW(cubies, position, rotationAxis, percentage, rotationInt, animate);
                else
                    swapCCW(cubies, position, rotationAxis, percentage, rotationInt, animate);
                break;

            case FaceEnum::LEFT:
                rotationAxis = glm::vec3(1, 0, 0);
                position = slice;
                if (move.rotation == RotationEnum::PRIME)
                    swapCCW(cubies, position, rotationAxis, percentage, rotationInt, animate);
                else
                    swapCW(cubies, position, rotationAxis, percentage, rotationInt, animate);
                break;

            case FaceEnum::FRONT:
                rotationAxis = glm::vec3(0, 0, -1);
                position = s_Size - 1 - slice;
                if (move.rotation == RotationEnum::PRIME)
                    swapCW(cubies, position, rotationAxis, percentage, rotationInt, animate);
                else
                    swapCCW(cubies, position, rotationAxis, percentage, rotationInt, animate);
                break;

            case FaceEnum::BACK:
                rotationAxis = glm::vec3(0, 0, 1);
                position = slice;
                if (move.rotation == RotationEnum::PRIME)
                    swapCCW(cubies, position, rotationAxis, percentage, rotationInt, animate);
                else
                    swapCW(cubies, position, rotationAxis, percentage, rotationInt, animate);
                break;

            default:
                assert(false && "Not possible");
        }
    }

    if (animate)
    {
        move.time += (dt / Move::seconds);
    }
}

void CubeManager::swapCW(QB*** cubies, uint16_t constant, glm::ivec3 rotationAxis, float percentage, int8_t angleMult, bool animate)
{
    bool completed = false;
    for (int i = 0; i < std::abs(angleMult); i++)
    {
        std::vector<QB*> stored;
        getSavedPositionsCW(cubies, stored, constant, rotationAxis);

        glm::ivec2 currentPos, nextPos;
        glm::ivec3 currentIndex, nextIndex;

        for (size_t i = 0; i < s_SwapIndices.size() - s_Size + 1; i++)
        {
            currentPos = s_SwapIndices.at(i);
            nextPos = s_SwapIndices.at(i + s_Size - 1);

            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);
            nextIndex = getCurrentIndex(constant, rotationAxis, nextPos);

            if (percentage >= 1.0f && animate)
            {
                (*cubies)[getIndex(currentIndex)] = (*cubies)[getIndex(nextIndex)];
                (*cubies)[getIndex(currentIndex)]->pos = coordsToPosition(currentIndex.x, currentIndex.y, currentIndex.z);
            }
            else if (!animate)
            {
                (*cubies)[getIndex(currentIndex)] = (*cubies)[getIndex(nextIndex)];
                (*cubies)[getIndex(currentIndex)]->index = glm::ivec3(currentIndex.x, currentIndex.y, currentIndex.z);
            }

            if (!completed)
            {
                if (animate)
                    (*cubies)[getIndex(currentIndex)]->rotate(rotationAxis, percentage, angleMult);
                else
                    (*cubies)[getIndex(currentIndex)]->rotateCurrent(rotationAxis, angleMult);
            }
        }

        for (size_t i = 0; i < stored.size(); i++)
        {
            currentPos = s_SwapIndices.at(s_SwapIndices.size() - s_Size + 1 + i);
            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);

            if (percentage >= 1.0f && animate)
            {
                (*cubies)[getIndex(currentIndex)] = stored[i];
                (*cubies)[getIndex(currentIndex)]->pos = coordsToPosition(currentIndex.x, currentIndex.y, currentIndex.z);
            }
            else if (!animate)
            {
                (*cubies)[getIndex(currentIndex)] = stored[i];
                (*cubies)[getIndex(currentIndex)]->index = glm::ivec3(currentIndex.x, currentIndex.y, currentIndex.z);
            }

            if (!completed)
            {
                if (animate)
                    (*cubies)[getIndex(currentIndex)]->rotate(rotationAxis, percentage, angleMult);
                else
                    (*cubies)[getIndex(currentIndex)]->rotateCurrent(rotationAxis, angleMult);
            }
        }

        if (!completed && animate)
        {
            if (s_Size > 2)
            {
                currentIndex = getCurrentIndex(constant, rotationAxis, { 1, 1 });
                (*cubies)[getIndex(currentIndex)]->rotate(rotationAxis, percentage, angleMult);
            }
        }
        completed = true;
    }
}

void CubeManager::swapCCW(QB*** cubies, uint16_t constant, glm::ivec3 rotationAxis, float percentage, int8_t angleMult, bool animate)
{
    bool completed = false;
    for (int i = 0; i < std::abs(angleMult); i++)
    {
        std::vector<QB*> stored;
        getSavedPositionsCCW(cubies, stored, constant, rotationAxis);

        glm::ivec2 currentPos, nextPos;
        glm::ivec3 currentIndex, nextIndex;

        for (size_t i = s_SwapIndices.size() - 1; i >= (size_t)(s_Size - 1); i--)
        {
            currentPos = s_SwapIndices.at(i);
            nextPos = s_SwapIndices.at(i - s_Size + 1);

            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);
            nextIndex = getCurrentIndex(constant, rotationAxis, nextPos);

            if (percentage >= 1.0f && animate)
            {
                (*cubies)[getIndex(currentIndex)] = (*cubies)[getIndex(nextIndex)];
                (*cubies)[getIndex(currentIndex)]->pos = coordsToPosition(currentIndex.x, currentIndex.y, currentIndex.z);
            }
            else if (!animate)
            {
                (*cubies)[getIndex(currentIndex)] = (*cubies)[getIndex(nextIndex)];
                (*cubies)[getIndex(currentIndex)]->index = glm::ivec3(currentIndex.x, currentIndex.y, currentIndex.z);
            }

            if (!completed)
            {
                if (animate)
                    (*cubies)[getIndex(currentIndex)]->rotate(rotationAxis, percentage, angleMult);
                else
                    (*cubies)[getIndex(currentIndex)]->rotateCurrent(rotationAxis, angleMult);
            }
        }

        for (size_t i = 0; i < stored.size(); i++)
        {
            currentPos = s_SwapIndices.at(i);
            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);

            if (percentage >= 1.0f && animate)
            {
                (*cubies)[getIndex(currentIndex)] = stored[i];
                (*cubies)[getIndex(currentIndex)]->pos = coordsToPosition(currentIndex.x, currentIndex.y, currentIndex.z);
            }
            else if (!animate)
            {
                (*cubies)[getIndex(currentIndex)] = stored[i];
                (*cubies)[getIndex(currentIndex)]->index = glm::ivec3(currentIndex.x, currentIndex.y, currentIndex.z);
            }

            if (!completed)
            {
                if (animate)
                    (*cubies)[getIndex(currentIndex)]->rotate(rotationAxis, percentage, angleMult);
                else
                    (*cubies)[getIndex(currentIndex)]->rotateCurrent(rotationAxis, angleMult);
            }
        }

        if (!completed && animate)
        {
            if (s_Size > 2)
            {
                currentIndex = getCurrentIndex(constant, rotationAxis, { 1, 1 });
                (*cubies)[getIndex(currentIndex)]->rotate(rotationAxis, percentage, angleMult);
            }
        }
        completed = true;
    }
}

void CubeManager::getSavedPositionsCW(QB*** cubies, std::vector<QB*>& stored, uint16_t constant, glm::ivec3 axis)
{
    for (int i = 0; i < s_Size - 1; i++)
    {
        glm::ivec2 swapPosition = s_SwapIndices.at(i);

        glm::ivec3 index = getCurrentIndex(constant, axis, swapPosition);

        QB* last = (*cubies)[getIndex(index)];
        stored.push_back(last);
    }
}

void CubeManager::getSavedPositionsCCW(QB*** cubies, std::vector<QB*>& stored, uint16_t constant, glm::ivec3 axis)
{
    for (int i = 0; i < s_Size - 1; i++)
    {
        glm::ivec2 swapPosition = s_SwapIndices.at(s_SwapIndices.size() - s_Size + 1 + i);

        glm::ivec3 index = getCurrentIndex(constant, axis, swapPosition);

        QB* last = (*cubies)[getIndex(index)];
        stored.push_back(last);
    }
}
