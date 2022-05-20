#include "CubeManager.hpp"

#include <glm/gtx/string_cast.hpp>

QB**** CubeManager::s_Cubies;
QB**** CubeManager::s_CurrentCubies;

std::vector<glm::ivec2> CubeManager::s_SwapIndices;

uint8_t CubeManager::s_SizeX;
uint8_t CubeManager::s_SizeY;
uint8_t CubeManager::s_SizeZ;

std::queue<Move> CubeManager::s_Moves;

void CubeManager::generate(uint8_t sizeX, uint8_t sizeY, uint8_t sizeZ)
{
    s_SizeX = sizeX;
    s_SizeY = sizeY;
    s_SizeZ = sizeZ;

    InnerCube::generate();
    Face::generateData();

    if (s_SizeX != s_SizeY || s_SizeX != s_SizeY)
    { 
        assert(false && "Only same sized cubes are currently allowed");
    }
    assert(s_SizeZ == 3 && "Only a 3x3 Cube is currently supported");

    s_Cubies = new QB***[sizeZ];
    s_CurrentCubies = new QB***[sizeZ];

    float lowestX = (float)((sizeX - 1) / 2.0f) - (sizeX - 1);
    float lowestY = (float)((sizeY - 1) / 2.0f) - (sizeY - 1);
    float lowestZ = (float)((sizeZ - 1) / 2.0f) - (sizeZ - 1);

    for (int i = 0; i < sizeZ; i++)
    {
        s_Cubies[i] = new QB**[sizeY];
        s_CurrentCubies[i] = new QB**[sizeY];
        for (int j = 0; j < sizeY; j++)
        {
            s_Cubies[i][j] = new QB*[sizeX];
            s_CurrentCubies[i][j] = new QB*[sizeX];

            for (int k = 0; k < sizeX; k++)
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
    for (int i = 0; i < sizeZ; i++)
    {
        for (int k = 0; k < sizeX; k++)
        {
            s_Cubies[i][0][k]->addFace(FaceEnum::DOWN);
            s_Cubies[i][s_SizeY - 1][k]->addFace(FaceEnum::UP);
        }
    }

    // Left | Right
    for (int i = 0; i < sizeZ; i++)
    {
        for (int j = 0; j < sizeY; j++)
        {
            s_Cubies[i][j][0]->addFace(FaceEnum::LEFT);
            s_Cubies[i][j][sizeX - 1]->addFace(FaceEnum::RIGHT);
        }
    }

    // Front | Back
    for (int j = 0; j < sizeY; j++)
    {
        for (int k = 0; k < sizeX; k++)
        {
            s_Cubies[0][j][k]->addFace(FaceEnum::BACK);
            s_Cubies[s_SizeZ - 1][j][k]->addFace(FaceEnum::FRONT);
        }
    }


    // Swap Indices
    uint16_t size = s_SizeX;
    for (int i = 0; i < size; i++)
    {
        s_SwapIndices.emplace_back(i, 0);
    }
    for (int i = 1; i < size; i++)
    {
        s_SwapIndices.emplace_back(size - 1, i);
    }
    for (int i = 1; i < size; i++)
    {
        s_SwapIndices.emplace_back(size - 1 - i, size - 1);
    }
    for (int i = 1; i < size - 1; i++)
    {
        s_SwapIndices.emplace_back(0, size - 1 - i);
    }

    Solver::loadCube(s_CurrentCubies, s_SizeX, s_SizeY, s_SizeZ);
}

void CubeManager::destroy()
{
    for (int i = 0; i < s_SizeZ; i++)
    {
        for (int j = 0; j < s_SizeY; j++)
        {
            for (int k = 0; k < s_SizeX; k++)
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
    for (int i = 0; i < s_SizeZ; i++)
    {
        for (int j = 0; j < s_SizeY; j++)
        {
            for (int k = 0; k < s_SizeX; k++)
            {
                s_Cubies[i][j][k]->draw(shader);
            }
        }
    }
}

void CubeManager::update(float dt)
{
    if (s_Moves.empty())
        return;

    rotate(dt);
}

void CubeManager::scramble(std::string input)
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
        localPos.y = s_SizeZ - 1 - pos.z;
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
        localPos.x = s_SizeZ - 1 - pos.z;
        localPos.y = pos.y;
        break;

    case FaceEnum::FRONT: // Z is locked
        localPos.x = pos.x;
        localPos.y = pos.y;
        break;
    case FaceEnum::BACK:
        localPos.x = s_SizeX - 1 - pos.x;
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
        if (localPos.x == s_SizeX - 1)
            return LocalEdgeEnum::RIGHT;
        else if (localPos.y == s_SizeZ - 1)
            return LocalEdgeEnum::TOP;

    case FaceEnum::LEFT:
    case FaceEnum::RIGHT:
        if (localPos.x == s_SizeZ - 1)
            return LocalEdgeEnum::RIGHT;
        else if (localPos.y == s_SizeY - 1)
            return LocalEdgeEnum::TOP;

    case FaceEnum::FRONT:
    case FaceEnum::BACK:
        if (localPos.x == s_SizeX - 1)
            return LocalEdgeEnum::RIGHT;
        else if (localPos.y == s_SizeY - 1)
            return LocalEdgeEnum::TOP;
    }

    assert(false && "Not reachable");
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
        if (localPos.x == 0 && localPos.y == s_SizeZ - 1)
            return LocalCornerEnum::TOP_LEFT;
        else if (localPos.x == s_SizeX - 1 && localPos.y == 0)
            return LocalCornerEnum::BOTTOM_RIGHT;
        else
            return LocalCornerEnum::TOP_RIGHT;

    case FaceEnum::LEFT:
    case FaceEnum::RIGHT:
        if (localPos.x == 0 && localPos.y == s_SizeY - 1)
            return LocalCornerEnum::TOP_LEFT;
        else if (localPos.x == s_SizeZ - 1 && localPos.y == 0)
            return LocalCornerEnum::BOTTOM_RIGHT;
        else
            return LocalCornerEnum::TOP_RIGHT;

    case FaceEnum::FRONT:
    case FaceEnum::BACK:
        if (localPos.x == 0 && localPos.y == s_SizeY - 1)
            return LocalCornerEnum::TOP_LEFT;
        else if (localPos.x == s_SizeX - 1 && localPos.y == 0)
            return LocalCornerEnum::BOTTOM_RIGHT;
        else
            return LocalCornerEnum::TOP_RIGHT;
    }
}

glm::vec3 CubeManager::coordsToPosition(uint16_t x, uint16_t y, uint16_t z)
{
    glm::vec3 returnVec(0.0f);
    returnVec.x = (float)(((s_SizeX - 1) / 2.0f) - (s_SizeX - 1)) + x;
    returnVec.y = (float)(((s_SizeY - 1) / 2.0f) - (s_SizeY - 1)) + y;
    returnVec.z = (float)(((s_SizeZ - 1) / 2.0f) - (s_SizeZ - 1)) + z;

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

void CubeManager::rotate(float dt)
{
    if (s_Moves.empty())
        return;

    Move& move = s_Moves.front();

    if (move.rotation == RotationEnum::NONE)
    {
        s_Moves.pop();
        return;
    }

    float percentage = move.time;
     
    glm::ivec3 rotationAxis;
    uint32_t position;

    int rotationInt = static_cast<int>(move.rotation);

    switch (move.face)
    {
        case FaceEnum::UP:
            rotationAxis = glm::vec3(0, -1, 0);
            position = s_SizeY - 1;
            if (move.rotation == RotationEnum::PRIME)
                swapCCW(position, s_SizeX, rotationAxis, percentage, rotationInt);
            else
                swapCW(position, s_SizeX, rotationAxis, percentage, rotationInt);

            break;

        case FaceEnum::DOWN:
            rotationAxis = glm::vec3(0, 1, 0);
            position = 0;

            if (move.rotation == RotationEnum::PRIME)
                swapCW(position, s_SizeX, rotationAxis, percentage, rotationInt);
            else
                swapCCW(position, s_SizeX, rotationAxis, percentage, rotationInt);
            break;

        case FaceEnum::RIGHT:
            rotationAxis = glm::vec3(-1, 0, 0);
            position = s_SizeX - 1;
            if (move.rotation == RotationEnum::PRIME)
                swapCW(position, s_SizeZ, rotationAxis, percentage, rotationInt);
            else
                swapCCW(position, s_SizeZ, rotationAxis, percentage, rotationInt);
            break;

        case FaceEnum::LEFT:
            rotationAxis = glm::vec3(1, 0, 0);
            position = 0;
            if (move.rotation == RotationEnum::PRIME)
                swapCCW(position, s_SizeZ, rotationAxis, percentage, rotationInt);
            else
                swapCW(position, s_SizeZ, rotationAxis, percentage, rotationInt);
            break;

        case FaceEnum::FRONT:
            rotationAxis = glm::vec3(0, 0, -1);
            position = s_SizeZ - 1;
            if (move.rotation == RotationEnum::PRIME)
                swapCW(position, s_SizeX, rotationAxis, percentage, rotationInt);
            else
                swapCCW(position, s_SizeX, rotationAxis, percentage, rotationInt);
            break;

        case FaceEnum::BACK:
            rotationAxis = glm::vec3(0, 0, 1);
            position = 0;
            if (move.rotation == RotationEnum::PRIME)
                swapCCW(position, s_SizeX, rotationAxis, percentage, rotationInt);
            else
                swapCW(position, s_SizeX, rotationAxis, percentage, rotationInt);
            break;
    }

    if (move.time >= 1)
    {
        s_Moves.pop();
    }
    move.time += dt / Move::seconds;
}

void CubeManager::swapCW(uint16_t constant, uint16_t width, glm::ivec3 rotationAxis, float percentage, int8_t angleMult)
{
    bool completed = false;
    for (int i = 0; i < std::abs(angleMult); i++)
    {
        std::vector<QB*> stored;
        getSavedPositionsCW(stored, width, constant, rotationAxis);

        glm::ivec2 currentPos, nextPos;
        glm::ivec3 currentIndex, nextIndex;

        for (size_t i = 0; i < s_SwapIndices.size() - (width - 1); i++)
        {
            currentPos = s_SwapIndices.at(i);
            nextPos = s_SwapIndices.at(i + 2);

            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);
            nextIndex = getCurrentIndex(constant, rotationAxis, nextPos);


            if (percentage >= 1.0f)
            {
                s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z] = s_Cubies[nextIndex.x][nextIndex.y][nextIndex.z];
                s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->pos = coordsToPosition(currentIndex.z, currentIndex.y, currentIndex.x);
            }

            if (!completed)
                s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage, angleMult);
        }

        for (size_t i = 0; i < stored.size(); i++)
        {
            currentPos = s_SwapIndices.at(s_SwapIndices.size() - 1 - (width - 2) + i);
            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);

            if (percentage >= 1.0f)
            {
                s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z] = stored[i];
                s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->pos = coordsToPosition(currentIndex.z, currentIndex.y, currentIndex.x);
            }

            if (!completed)
                s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage, angleMult);
        }

        if (!completed)
        {
            currentIndex = getCurrentIndex(constant, rotationAxis, { 1, 1 });
            s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage, angleMult);

            completed = true;
        }
    }
}

void CubeManager::swapCCW(uint16_t constant, uint16_t width, glm::ivec3 rotationAxis, float percentage, int8_t angleMult)
{
    bool completed = false;
    for (int i = 0; i < std::abs(angleMult); i++)
    {
        std::vector<QB*> stored;
        getSavedPositionsCCW(stored, width, constant, rotationAxis);

        glm::ivec2 currentPos, nextPos;
        glm::ivec3 currentIndex, nextIndex;

        for (size_t i = s_SwapIndices.size() - 1; i >= width - 1; i--)
        {
            currentPos = s_SwapIndices.at(i);
            nextPos = s_SwapIndices.at(i - 2);

            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);
            nextIndex = getCurrentIndex(constant, rotationAxis, nextPos);

            if (percentage >= 1.0f)
            {
                s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z] = s_Cubies[nextIndex.x][nextIndex.y][nextIndex.z];
                s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->pos = coordsToPosition(currentIndex.z, currentIndex.y, currentIndex.x);
            }

            if (!completed)
                s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage, angleMult);
        }

        for (size_t i = 0; i < stored.size(); i++)
        {
            currentPos = s_SwapIndices.at(i);
            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);

            if (percentage >= 1.0f)
            {
                s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z] = stored[i];
                s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->pos = coordsToPosition(currentIndex.z, currentIndex.y, currentIndex.x);
            }

            if (!completed)
                s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage, angleMult);
        }

        if (!completed)
        {
            currentIndex = getCurrentIndex(constant, rotationAxis, { 1, 1 });
            s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage, angleMult);

            completed = true;
        }
    }
}

void CubeManager::getSavedPositionsCW(std::vector<QB*>& stored, int width, uint16_t constant, glm::ivec3 axis)
{
    for (int i = 0; i < width - 1; i++)
    {
        glm::ivec2 swapPosition = s_SwapIndices.at(i);

        glm::ivec3 index = getCurrentIndex(constant, axis, swapPosition);

        QB* last = s_Cubies[index.x][index.y][index.z];
        stored.push_back(last);
    }
}

void CubeManager::getSavedPositionsCCW(std::vector<QB*>& stored, int width, uint16_t constant, glm::ivec3 axis)
{
    for (int i = 0; i < width - 1; i++)
    {
        glm::ivec2 swapPosition = s_SwapIndices.at(s_SwapIndices.size() - 1 - (width - 2) + i);

        glm::ivec3 index = getCurrentIndex(constant, axis, swapPosition);

        QB* last = s_Cubies[index.x][index.y][index.z];
        stored.push_back(last);
    }
}

void CubeManager::rotateCurrent(Move& move)
{
    if (move.rotation == RotationEnum::NONE)
        return;

    glm::ivec3 rotationAxis;
    uint32_t position;

    int rotationInt = static_cast<int>(move.rotation);

    switch (move.face)
    {
        case FaceEnum::UP:
            rotationAxis = glm::vec3(0, -1, 0);
            position = s_SizeY - 1;
            if (move.rotation == RotationEnum::PRIME)
                swapCCWCurrent(position, s_SizeX, rotationAxis, rotationInt);
            else
                swapCWCurrent(position, s_SizeX, rotationAxis, rotationInt);

            break;

        case FaceEnum::DOWN:
            rotationAxis = glm::vec3(0, 1, 0);
            position = 0;

            if (move.rotation == RotationEnum::PRIME)
                swapCWCurrent(position, s_SizeX, rotationAxis, rotationInt);
            else
                swapCCWCurrent(position, s_SizeX, rotationAxis, rotationInt);
            break;

        case FaceEnum::RIGHT:
            rotationAxis = glm::vec3(-1, 0, 0);
            position = s_SizeX - 1;
            if (move.rotation == RotationEnum::PRIME)
                swapCWCurrent(position, s_SizeZ, rotationAxis, rotationInt);
            else
                swapCCWCurrent(position, s_SizeZ, rotationAxis, rotationInt);
            break;

        case FaceEnum::LEFT:
            rotationAxis = glm::vec3(1, 0, 0);
            position = 0;
            if (move.rotation == RotationEnum::PRIME)
                swapCCWCurrent(position, s_SizeZ, rotationAxis, rotationInt);
            else
                swapCWCurrent(position, s_SizeZ, rotationAxis, rotationInt);
            break;

        case FaceEnum::FRONT:
            rotationAxis = glm::vec3(0, 0, -1);
            position = s_SizeZ - 1;
            if (move.rotation == RotationEnum::PRIME)
                swapCWCurrent(position, s_SizeX, rotationAxis, rotationInt);
            else
                swapCCWCurrent(position, s_SizeX, rotationAxis, rotationInt);
            break;

        case FaceEnum::BACK:
            rotationAxis = glm::vec3(0, 0, 1);
            position = 0;
            if (move.rotation == RotationEnum::PRIME)
                swapCCWCurrent(position, s_SizeX, rotationAxis, rotationInt);
            else
                swapCWCurrent(position, s_SizeX, rotationAxis, rotationInt);
            break;
    }
}

void CubeManager::swapCWCurrent(uint16_t constant, uint16_t width, glm::ivec3 rotationAxis, int8_t angleMult)
{
    bool completed = false;
    for (int i = 0; i < std::abs(angleMult); i++)
    {
        std::vector<QB*> stored;
        getSavedPositionsCWCurrent(stored, width, constant, rotationAxis);

        glm::ivec2 currentPos, nextPos;
        glm::ivec3 currentIndex, nextIndex;

        for (size_t i = 0; i < s_SwapIndices.size() - (width - 1); i++)
        {
            currentPos = s_SwapIndices.at(i);
            nextPos = s_SwapIndices.at(i + 2);

            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);
            nextIndex = getCurrentIndex(constant, rotationAxis, nextPos);

            s_CurrentCubies[currentIndex.x][currentIndex.y][currentIndex.z] = s_CurrentCubies[nextIndex.x][nextIndex.y][nextIndex.z];
            s_CurrentCubies[currentIndex.x][currentIndex.y][currentIndex.z]->index = glm::ivec3(currentIndex.z, currentIndex.y, currentIndex.x);

            if (!completed)
                s_CurrentCubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotateCurrent(rotationAxis, angleMult);
        }

        for (size_t i = 0; i < stored.size(); i++)
        {
            currentPos = s_SwapIndices.at(s_SwapIndices.size() - 1 - (width - 2) + i);
            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);

            s_CurrentCubies[currentIndex.x][currentIndex.y][currentIndex.z] = stored[i];
            s_CurrentCubies[currentIndex.x][currentIndex.y][currentIndex.z]->index = glm::ivec3(currentIndex.z, currentIndex.y, currentIndex.x);

            if (!completed)
                s_CurrentCubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotateCurrent(rotationAxis, angleMult);
        }
        completed = true;
    }
}

void CubeManager::swapCCWCurrent(uint16_t constant, uint16_t width, glm::ivec3 rotationAxis, int8_t angleMult)
{
    bool completed = false;
    for (int i = 0; i < std::abs(angleMult); i++)
    {
        std::vector<QB*> stored;
        getSavedPositionsCCWCurrent(stored, width, constant, rotationAxis);

        glm::ivec2 currentPos, nextPos;
        glm::ivec3 currentIndex, nextIndex;

        for (size_t i = s_SwapIndices.size() - 1; i >= width - 1; i--)
        {
            currentPos = s_SwapIndices.at(i);
            nextPos = s_SwapIndices.at(i - 2);

            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);
            nextIndex = getCurrentIndex(constant, rotationAxis, nextPos);

            s_CurrentCubies[currentIndex.x][currentIndex.y][currentIndex.z] = s_CurrentCubies[nextIndex.x][nextIndex.y][nextIndex.z];
            s_CurrentCubies[currentIndex.x][currentIndex.y][currentIndex.z]->index = glm::ivec3(currentIndex.z, currentIndex.y, currentIndex.x);

            if (!completed)
                s_CurrentCubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotateCurrent(rotationAxis, angleMult);
        }

        for (size_t i = 0; i < stored.size(); i++)
        {
            currentPos = s_SwapIndices.at(i);
            currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);

            s_CurrentCubies[currentIndex.x][currentIndex.y][currentIndex.z] = stored[i];
            s_CurrentCubies[currentIndex.x][currentIndex.y][currentIndex.z]->index = glm::ivec3(currentIndex.z, currentIndex.y, currentIndex.x);

            if (!completed)
                s_CurrentCubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotateCurrent(rotationAxis, angleMult);
        }
        completed = true;
    }
}

void CubeManager::getSavedPositionsCWCurrent(std::vector<QB*>& stored, int width, uint16_t constant, glm::ivec3 axis)
{
    for (int i = 0; i < width - 1; i++)
    {
        glm::ivec2 swapPosition = s_SwapIndices.at(i);

        glm::ivec3 index = getCurrentIndex(constant, axis, swapPosition);

        QB* last = s_CurrentCubies[index.x][index.y][index.z];
        stored.push_back(last);
    }
}

void CubeManager::getSavedPositionsCCWCurrent(std::vector<QB*>& stored, int width, uint16_t constant, glm::ivec3 axis)
{
    for (int i = 0; i < width - 1; i++)
    {
        glm::ivec2 swapPosition = s_SwapIndices.at(s_SwapIndices.size() - 1 - (width - 2) + i);

        glm::ivec3 index = getCurrentIndex(constant, axis, swapPosition);

        QB* last = s_CurrentCubies[index.x][index.y][index.z];
        stored.push_back(last);
    }
}
