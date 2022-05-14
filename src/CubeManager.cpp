#include "CubeManager.hpp"

// TODO: Add seperate functions for "rotate" and add seperate rotations for each direction to follow clockwise order

QB**** CubeManager::s_Cubies;
uint8_t CubeManager::s_SizeX;
uint8_t CubeManager::s_SizeY;
uint8_t CubeManager::s_SizeZ;

std::vector<glm::ivec2> CubeManager::s_SwapIndices;

void PrintQBPosition(QB* t)
{
    std::cout << t->pos.x << " : " << t->pos.y << " : " << t->pos.z << "\n";
}

void CubeManager::generate(uint8_t sizeX, uint8_t sizeY, uint8_t sizeZ)
{
    s_SizeX = sizeX;
    s_SizeY = sizeY;
    s_SizeZ = sizeZ;

    s_Cubies = new QB***[sizeZ];

    float lowestX = (float)((sizeX - 1) / 2.0f) - (sizeX - 1);
    float lowestY = (float)((sizeY - 1) / 2.0f) - (sizeY - 1);
    float lowestZ = (float)((sizeZ - 1) / 2.0f) - (sizeZ - 1);

    for (int i = 0; i < sizeZ; i++)
    {
        s_Cubies[i] = new QB**[sizeY];
        for (int j = 0; j < sizeY; j++)
        {
            s_Cubies[i][j] = new QB*[sizeX];

            for (int k = 0; k < sizeX; k++)
            {
                glm::vec3 pos(lowestX + k, lowestY + j, lowestZ + i);
                // glm::vec3 otherPos = coordsToPosition(k, j, i);
                // std::cout << pos.x << " : " << pos.y << " : " << pos.z << "\n";
                // std::cout << otherPos.x << " : " << otherPos.y << " : " << otherPos.z << "\n";
                // std::cout << "\n";
                s_Cubies[i][j][k] = new QB(pos);
            }
        }
    }

    // Top | Bottom
    for (int i = 0; i < sizeZ; i++)
    {
        for (int k = 0; k < sizeX; k++)
        {
            s_Cubies[i][0][k]->addFace(FaceFlag::Bottom);
            s_Cubies[i][s_SizeY - 1][k]->addFace(FaceFlag::Top);
        }
    }

    // Left | Right
    for (int i = 0; i < sizeZ; i++)
    {
        for (int j = 0; j < sizeY; j++)
        {
            s_Cubies[i][j][0]->addFace(FaceFlag::Left);
            s_Cubies[i][j][sizeX - 1]->addFace(FaceFlag::Right);
        }
    }

    // Front | Back
    for (int j = 0; j < sizeY; j++)
    {
        for (int k = 0; k < sizeX; k++)
        {
            s_Cubies[0][j][k]->addFace(FaceFlag::Back);
            s_Cubies[s_SizeZ - 1][j][k]->addFace(FaceFlag::Front);
        }
    }

    if (s_SizeX != s_SizeY || s_SizeX != s_SizeY)
    { 
        assert(false && "Only same sized cubes are currently allowed");
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

void CubeManager::rotate(FaceFlag face, RotationType type)
{
    glm::vec3 rotation;
    uint32_t position;
    switch (face)
    {
    case FaceFlag::Top:
        rotation = glm::vec3(0.0f, 1.0f, 0.0f);
        position = s_SizeY - 1;
        break;
    case FaceFlag::Bottom:
        rotation = glm::vec3(0.0f, 1.0f, 0.0f);
        position = 0;
        break;
    case FaceFlag::Right:
        rotation = glm::vec3(-1.0f, 0.0f, 0.0f);
        position = s_SizeX - 1;
        break;
    case FaceFlag::Left:
        rotation = glm::vec3(-1.0f, 0.0f, 0.0f);
        position = 0;
        break;
    case FaceFlag::Front:
        rotation = glm::vec3(0.0f, 0.0f, -1.0f);
        position = s_SizeZ - 1;
        break;
    case FaceFlag::Back:
        rotation = glm::vec3(0.0f, 0.0f, -1.0f);
        position = 0;
        break;
    }

    switch (face)
    {
    case FaceFlag::Top:
    case FaceFlag::Bottom:
        {
            for(int i = 0; i < static_cast<int>(type); i++)
            {
                std::vector<QB*> stored;
                for (int i = 0; i < s_SizeX - 1; i++)
                {
                    glm::ivec2 lastPosition = s_SwapIndices.at(s_SwapIndices.size() - 1 - (s_SizeX - 2) + i);
                    QB* last = s_Cubies[lastPosition.x][position][lastPosition.y];
                    stored.push_back(last);
                }

                for (int i = s_SwapIndices.size() - 1; i > 1; i--)
                {
                    glm::ivec2 currentPos = s_SwapIndices.at(i);
                    glm::ivec2 nextPos = s_SwapIndices.at(i - 2);

                    s_Cubies[currentPos.x][position][currentPos.y] = s_Cubies[nextPos.x][position][nextPos.y];
                    s_Cubies[currentPos.x][position][currentPos.y]->pos = coordsToPosition(currentPos.y, position, currentPos.x);

                    s_Cubies[currentPos.x][position][currentPos.y]->rotate(rotation);
                }

                for (int i = 0; i < stored.size(); i++)
                {
                    glm::ivec2 firstPosition = s_SwapIndices.at(i);

                    s_Cubies[firstPosition.x][position][firstPosition.y] = stored[i];
                    s_Cubies[firstPosition.x][position][firstPosition.y] = stored[i];
                    s_Cubies[firstPosition.x][position][firstPosition.y]->pos = coordsToPosition(firstPosition.y, position, firstPosition.x);

                    s_Cubies[firstPosition.x][position][firstPosition.y]->rotate(rotation);
                }
            }
            break;
        }
    case FaceFlag::Left:
    case FaceFlag::Right:
        {
            for(int i = 0; i < static_cast<int>(type); i++)
            {
                std::vector<QB*> stored;
                for (int i = 0; i < s_SizeX - 1; i++)
                {
                    glm::ivec2 lastPosition = s_SwapIndices.at(s_SwapIndices.size() - 1 - (s_SizeZ - 2) + i);
                    QB* last = s_Cubies[lastPosition.x][lastPosition.y][position];
                    stored.push_back(last);
                }

                for (int i = s_SwapIndices.size() - 1; i > 1; i--)
                {
                    glm::ivec2 currentPos = s_SwapIndices.at(i);
                    glm::ivec2 nextPos = s_SwapIndices.at(i - 2);

                    s_Cubies[currentPos.x][currentPos.y][position] = s_Cubies[nextPos.x][nextPos.y][position];
                    s_Cubies[currentPos.x][currentPos.y][position]->pos = coordsToPosition(position, currentPos.y, currentPos.x);

                    s_Cubies[currentPos.x][currentPos.y][position]->rotate(rotation);
                }

                for (int i = 0; i < stored.size(); i++)
                {
                    glm::ivec2 firstPosition = s_SwapIndices.at(i);

                    s_Cubies[firstPosition.x][firstPosition.y][position] = stored[i];
                    s_Cubies[firstPosition.x][firstPosition.y][position]->pos = coordsToPosition(position, firstPosition.y, firstPosition.x);
                    s_Cubies[firstPosition.x][firstPosition.y][position]->rotate(rotation);
                }
            }
            break;
        }
    case FaceFlag::Front:
    case FaceFlag::Back:
        {
            for(int i = 0; i < static_cast<int>(type); i++)
            {
                std::vector<QB*> stored;
                for (int i = 0; i < s_SizeX - 1; i++)
                {
                    glm::ivec2 lastPosition = s_SwapIndices.at(s_SwapIndices.size() - 1 - (s_SizeX - 2) + i);
                    QB* last = s_Cubies[position][lastPosition.x][lastPosition.y];
                    stored.push_back(last);
                }

                for (int i = s_SwapIndices.size() - 1; i > 1; i--)
                {
                    glm::ivec2 currentPos = s_SwapIndices.at(i);
                    glm::ivec2 nextPos = s_SwapIndices.at(i - 2);

                    s_Cubies[position][currentPos.x][currentPos.y] = s_Cubies[position][nextPos.x][nextPos.y];
                    s_Cubies[position][currentPos.x][currentPos.y]->pos = coordsToPosition(currentPos.y, currentPos.x, position);

                    s_Cubies[position][currentPos.x][currentPos.y]->rotate(rotation);
                }

                for (int i = 0; i < stored.size(); i++)
                {
                    glm::ivec2 firstPosition = s_SwapIndices.at(i);

                    s_Cubies[position][firstPosition.x][firstPosition.y] = stored[i];
                    s_Cubies[position][firstPosition.x][firstPosition.y]->pos = coordsToPosition(firstPosition.y, firstPosition.x, position);
                    s_Cubies[position][firstPosition.x][firstPosition.y]->rotate(rotation);
                }
            }
            break;
        }
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
