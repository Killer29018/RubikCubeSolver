#include "CubeManager.hpp"

#include <glm/gtx/string_cast.hpp>

// TODO: Add seperate functions for "rotate" and add seperate rotations for each direction to follow clockwise order

QB**** CubeManager::s_Cubies;
std::vector<glm::ivec2> CubeManager::s_SwapIndices;

uint32_t CubeManager::s_VAO;
uint32_t CubeManager::s_EBO;

uint8_t CubeManager::s_SizeX;
uint8_t CubeManager::s_SizeY;
uint8_t CubeManager::s_SizeZ;

void CubeManager::generate(uint8_t sizeX, uint8_t sizeY, uint8_t sizeZ)
{
    s_SizeX = sizeX;
    s_SizeY = sizeY;
    s_SizeZ = sizeZ;

    if (s_SizeX != s_SizeY || s_SizeX != s_SizeY)
    { 
        assert(false && "Only same sized cubes are currently allowed");
    }
    assert(s_SizeZ == 3 && "Only a 3x3 Cube is currently supported");


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

    generateVAO();
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
    shader.bind();
    shader.setUniformVector3("u_Colour", glm::vec3(0.0f));
    glm::mat4 model(1.0f);

    model = glm::scale(model, glm::vec3(s_SizeX, s_SizeY, s_SizeZ));
    model = glm::scale(model, glm::vec3(0.995f));
    shader.setUniformMatrix4("u_Model", model);

    glBindVertexArray(s_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);

    glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, NULL);
            
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

void CubeManager::scramble(std::string input)
{
    for (int i = 0; i < input.size() - 1;)
    {
        char c = input[i];
        char nextC = input[i + 1];

        RotationType rotation = RotationType::NONE;
        switch (nextC)
        {
        case '\'':
            rotation = RotationType::PRIME;
            i += 3;
            break;
        case ' ':
            rotation = RotationType::NORMAL;
            i += 2;
            break;
        case '2':
            rotation = RotationType::TWICE;
            i += 3;
            break;
        }

        switch (c)
        {
        case 'U': rotate(FaceFlag::Top, rotation); break;
        case 'D': rotate(FaceFlag::Bottom, rotation); break;
        case 'L': rotate(FaceFlag::Left, rotation); break;
        case 'R': rotate(FaceFlag::Right, rotation); break;
        case 'F': rotate(FaceFlag::Front, rotation); break;
        case 'B': rotate(FaceFlag::Back, rotation); break;
        }
    }
}

void CubeManager::rotate(FaceFlag face, RotationType type)
{
    glm::ivec3 rotationAxis;
    uint32_t position;

    for (int i = 0; i < static_cast<int>(type); i++)
    {
        switch (face)
        {
            case FaceFlag::Top:
                rotationAxis = glm::vec3(0, -1, 0);
                position = s_SizeY - 1;
                swapCW(position, s_SizeX, rotationAxis);
                break;

            case FaceFlag::Bottom:
                rotationAxis = glm::vec3(0, 1, 0);
                position = 0;
                swapCCW(position, s_SizeX, rotationAxis);
                break;

            case FaceFlag::Right:
                rotationAxis = glm::vec3(-1, 0, 0);
                position = s_SizeX - 1;
                swapCCW(position, s_SizeZ, rotationAxis);
                break;

            case FaceFlag::Left:
                rotationAxis = glm::vec3(1, 0, 0);
                position = 0;
                swapCW(position, s_SizeZ, rotationAxis);
                break;

            case FaceFlag::Front:
                rotationAxis = glm::vec3(0, 0, -1);
                position = s_SizeZ - 1;
                swapCCW(position, s_SizeX, rotationAxis);
                break;

            case FaceFlag::Back:
                rotationAxis = glm::vec3(0, 0, 1);
                position = 0;
                swapCW(position, s_SizeX, rotationAxis);
                break;
        }
    }
}

void CubeManager::generateVAO()
{
    const float vertices[] = 
    {
        -0.5f, -0.5f, -0.5f, // 0
        -0.5f, -0.5f,  0.5f, // 1
        -0.5f,  0.5f, -0.5f, // 2
        -0.5f,  0.5f,  0.5f, // 3
         0.5f, -0.5f, -0.5f, // 4
         0.5f, -0.5f,  0.5f, // 5
         0.5f,  0.5f, -0.5f, // 6
         0.5f,  0.5f,  0.5f, // 7
    };

    const uint32_t indices[] = 
    {
        // Top
        0, 1, 4, 1, 4, 5,

        // Bottom
        2, 3, 6, 3, 6, 7,

        // Left
        0, 1, 2, 1, 2, 3,

        // Right
        4, 5, 6, 5, 6, 7,

        // Front
        1, 3, 5, 3, 5, 7,

        // Back
        0, 2, 4, 2, 4, 6,
    };

    uint32_t vbo;
    glGenVertexArrays(1, &s_VAO);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &s_EBO);

    glBindVertexArray(s_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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

void CubeManager::swapCW(uint16_t constant, uint16_t width, glm::ivec3 rotationAxis)
{
    std::vector<QB*> stored;
    getSavedPositionsCW(stored, width, constant, rotationAxis);

    for (int i = 0; i < s_SwapIndices.size() - (width - 1); i++)
    {
        glm::ivec2 currentPos = s_SwapIndices.at(i);
        glm::ivec2 nextPos = s_SwapIndices.at(i + 2);

        glm::ivec3 currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);
        glm::ivec3 nextIndex = getCurrentIndex(constant, rotationAxis, nextPos);

        s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z] = s_Cubies[nextIndex.x][nextIndex.y][nextIndex.z];
        s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->pos = coordsToPosition(currentIndex.z, currentIndex.y, currentIndex.x);

        s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis);
    }

    for (int i = 0; i < stored.size(); i++)
    {
        glm::ivec2 firstPosition = s_SwapIndices.at(s_SwapIndices.size() - 1 - (width - 2) + i);
        glm::ivec3 index = getCurrentIndex(constant, rotationAxis, firstPosition);

        s_Cubies[index.x][index.y][index.z] = stored[i];
        s_Cubies[index.x][index.y][index.z]->pos = coordsToPosition(index.z, index.y, index.x);

        s_Cubies[index.x][index.y][index.z]->rotate(rotationAxis);
    }
}

void CubeManager::swapCCW(uint16_t constant, uint16_t width, glm::ivec3 rotationAxis)
{
    std::vector<QB*> stored;
    getSavedPositionsCCW(stored, width, constant, rotationAxis);

    for (int i = s_SwapIndices.size() - 1; i >= width - 1; i--)
    {
        glm::ivec2 currentPos = s_SwapIndices.at(i);
        glm::ivec2 nextPos = s_SwapIndices.at(i - 2);

        glm::ivec3 currentIndex = getCurrentIndex(constant, rotationAxis, currentPos);
        glm::ivec3 nextIndex = getCurrentIndex(constant, rotationAxis, nextPos);

        s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z] = s_Cubies[nextIndex.x][nextIndex.y][nextIndex.z];
        s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->pos = coordsToPosition(currentIndex.z, currentIndex.y, currentIndex.x);

        s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis);
    }

    for (int i = 0; i < stored.size(); i++)
    {
        glm::ivec2 firstPosition = s_SwapIndices.at(i);
        glm::ivec3 index = getCurrentIndex(constant, rotationAxis, firstPosition);

        s_Cubies[index.x][index.y][index.z] = stored[i];
        s_Cubies[index.x][index.y][index.z]->pos = coordsToPosition(index.z, index.y, index.x);

        s_Cubies[index.x][index.y][index.z]->rotate(rotationAxis);
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
