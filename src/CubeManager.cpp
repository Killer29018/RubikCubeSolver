#include "CubeManager.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

QB**** CubeManager::s_Cubies;
QB**** CubeManager::s_CurrentCubies;

std::vector<glm::ivec2> CubeManager::s_SwapIndices;

uint8_t CubeManager::s_Size;

std::queue<Move> CubeManager::s_Moves;

bool CubeManager::s_MousePick = false;

void CubeManager::generate(uint8_t size)
{
    s_Size = size;

    assert(s_Size == 3 && "Only a 3x3 Cube is currently supported");

    s_Cubies = new QB***[s_Size];
    s_CurrentCubies = new QB***[s_Size];

    float lowestX = (float)((s_Size - 1) / 2.0f) - (s_Size - 1);
    float lowestY = (float)((s_Size - 1) / 2.0f) - (s_Size - 1);
    float lowestZ = (float)((s_Size - 1) / 2.0f) - (s_Size - 1);

    for (uint8_t i = 0; i < s_Size; i++)
    {
        s_Cubies[i] = new QB**[s_Size];
        s_CurrentCubies[i] = new QB**[s_Size];

        for (uint8_t j = 0; j < s_Size; j++)
        {
            s_Cubies[i][j] = new QB*[s_Size];
            s_CurrentCubies[i][j] = new QB*[s_Size];

            for (uint8_t k = 0; k < s_Size; k++)
            {
                glm::vec3 pos(lowestX + k, lowestY + j, lowestZ + i);
                glm::ivec3 index(k, j, i);

                QB* qb = new QB(pos, index);

                s_Cubies[i][j][k] = qb;
                s_CurrentCubies[i][j][k] = qb;
            }
        }
    }

    // Top | Bottom
    for (uint8_t i = 0; i < s_Size; i++)
    {
        for (uint8_t k = 0; k < s_Size; k++)
        {
            s_Cubies[i][0][k]->addFace(FaceEnum::DOWN);
            s_Cubies[i][s_Size - 1][k]->addFace(FaceEnum::UP);
        }
    }

    // Left | Right
    for (uint8_t i = 0; i < s_Size; i++)
    {
        for (uint8_t j = 0; j < s_Size; j++)
        {
            s_Cubies[i][j][0]->addFace(FaceEnum::LEFT);
            s_Cubies[i][j][s_Size - 1]->addFace(FaceEnum::RIGHT);
        }
    }

    // Front | Back
    for (uint8_t j = 0; j < s_Size; j++)
    {
        for (uint8_t k = 0; k < s_Size; k++)
        {
            s_Cubies[0][j][k]->addFace(FaceEnum::BACK);
            s_Cubies[s_Size - 1][j][k]->addFace(FaceEnum::FRONT);
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
    for (int i = 0; i < s_Size; i++)
    {
        for (int j = 0; j < s_Size; j++)
        {
            for (int k = 0; k < s_Size; k++)
            {
                delete s_Cubies[i][j][k];
            }
            delete[] s_Cubies[i][j];
        }
        delete[] s_Cubies[i];
    }
    delete[] s_Cubies;
}

void CubeManager::draw(KRE::Shader& shader)
{
    for (int i = 0; i < s_Size; i++) // z
    {
        for (int j = 0; j < s_Size; j++) // y
        {
            for (int k = 0; k < s_Size; k++) // x
            {
                glm::mat4 customRotation(1.0f);

                glm::ivec3 index(k, j, i);

                if (MousePicker::qbPartOfSlice(index))
                    customRotation = glm::toMat4(glm::angleAxis(glm::radians(MousePicker::pickedObject.angle), MousePicker::pickedObject.angleAxis));

                s_Cubies[i][j][k]->draw(shader, customRotation);
            }
        }
    }
}

void CubeManager::update(float dt)
{
    if (s_Moves.empty())
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
    s_Moves.push(move);
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
        assert(false && "Impossible");
    }
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
        index.x = swapPosition.x;
        index.y = swapPosition.y;
        index.z = constant;
    }
    else if (axis.y != 0)
    {
        index.x = swapPosition.x;
        index.y = constant;
        index.z = swapPosition.y;
    }
    else if (axis.z != 0)
    {
        index.x = constant;
        index.y = swapPosition.x;
        index.z = swapPosition.y;
    }

    return index;
}

void CubeManager::rotateAnimate(float dt)
{
    Move& move = s_Moves.front();

    if (move.rotation == RotationEnum::NONE)
    {
        s_Moves.pop();
        return;
    }

    if (move.time >= 1.0f)
    {
        s_Moves.pop();
    }
    rotate(&s_Cubies, move, dt, true);

}

void CubeManager::rotateCurrent(Move& move)
{
    rotate(&s_CurrentCubies, move, 0.0f, false);
}

void CubeManager::rotate(QB***** cubies, Move& move, float dt, bool animate)
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
        }
    }

    if (animate)
    {
        move.time += (dt / Move::seconds);
    }
}

void CubeManager::swapCW(QB***** cubies, uint16_t constant, glm::ivec3 rotationAxis, float percentage, int8_t angleMult, bool animate)
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
            nextPos = s_SwapIndices.at(i + 2);

            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);
            nextIndex = getCurrentIndex(constant, rotationAxis, nextPos);

            if (percentage >= 1.0f && animate)
            {
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z] = (*cubies)[nextIndex.x][nextIndex.y][nextIndex.z];
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->pos = coordsToPosition(currentIndex.z, currentIndex.y, currentIndex.x);
            }
            else if (!animate)
            {
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z] = (*cubies)[nextIndex.x][nextIndex.y][nextIndex.z];
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->index = glm::ivec3(currentIndex.z, currentIndex.y, currentIndex.x);
            }

            if (!completed)
            {
                if (animate)
                    (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage, angleMult);
                else
                    (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->rotateCurrent(rotationAxis, angleMult);
            }
        }

        for (size_t i = 0; i < stored.size(); i++)
        {
            currentPos = s_SwapIndices.at(s_SwapIndices.size() - s_Size + 1 + i);
            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);

            if (percentage >= 1.0f && animate)
            {
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z] = stored[i];
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->pos = coordsToPosition(currentIndex.z, currentIndex.y, currentIndex.x);
            }
            else if (!animate)
            {
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z] = stored[i];
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->index = glm::ivec3(currentIndex.z, currentIndex.y, currentIndex.x);
            }

            if (!completed)
            {
                if (animate)
                    (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage, angleMult);
                else
                    (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->rotateCurrent(rotationAxis, angleMult);
            }
        }

        if (!completed && animate)
        {
            currentIndex = getCurrentIndex(constant, rotationAxis, { 1, 1 });
            (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage, angleMult);
        }
        completed = true;
    }
}

void CubeManager::swapCCW(QB***** cubies, uint16_t constant, glm::ivec3 rotationAxis, float percentage, int8_t angleMult, bool animate)
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
            nextPos = s_SwapIndices.at(i - 2);

            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);
            nextIndex = getCurrentIndex(constant, rotationAxis, nextPos);

            if (percentage >= 1.0f && animate)
            {
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z] = (*cubies)[nextIndex.x][nextIndex.y][nextIndex.z];
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->pos = coordsToPosition(currentIndex.z, currentIndex.y, currentIndex.x);
            }
            else if (!animate)
            {
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z] = (*cubies)[nextIndex.x][nextIndex.y][nextIndex.z];
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->index = glm::ivec3(currentIndex.z, currentIndex.y, currentIndex.x);
            }

            if (!completed)
            {
                if (animate)
                    (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage, angleMult);
                else
                    (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->rotateCurrent(rotationAxis, angleMult);
            }
        }

        for (size_t i = 0; i < stored.size(); i++)
        {
            currentPos = s_SwapIndices.at(i);
            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);

            if (percentage >= 1.0f && animate)
            {
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z] = stored[i];
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->pos = coordsToPosition(currentIndex.z, currentIndex.y, currentIndex.x);
            }
            else if (!animate)
            {
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z] = stored[i];
                (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->index = glm::ivec3(currentIndex.z, currentIndex.y, currentIndex.x);
            }

            if (!completed)
            {
                if (animate)
                    (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage, angleMult);
                else
                    (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->rotateCurrent(rotationAxis, angleMult);
            }
        }

        if (!completed && animate)
        {
            currentIndex = getCurrentIndex(constant, rotationAxis, { 1, 1 });
            (*cubies)[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage, angleMult);
        }
        completed = true;
    }
}

void CubeManager::getSavedPositionsCW(QB***** cubies, std::vector<QB*>& stored, uint16_t constant, glm::ivec3 axis)
{
    for (int i = 0; i < s_Size - 1; i++)
    {
        glm::ivec2 swapPosition = s_SwapIndices.at(i);

        glm::ivec3 index = getCurrentIndex(constant, axis, swapPosition);

        QB* last = (*cubies)[index.x][index.y][index.z];
        stored.push_back(last);
    }
}

void CubeManager::getSavedPositionsCCW(QB***** cubies, std::vector<QB*>& stored, uint16_t constant, glm::ivec3 axis)
{
    for (int i = 0; i < s_Size - 1; i++)
    {
        glm::ivec2 swapPosition = s_SwapIndices.at(s_SwapIndices.size() - s_Size + 1 + i);

        glm::ivec3 index = getCurrentIndex(constant, axis, swapPosition);

        QB* last = (*cubies)[index.x][index.y][index.z];
        stored.push_back(last);
    }
}
