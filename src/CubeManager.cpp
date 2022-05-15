#include "CubeManager.hpp"

#include <glm/gtx/string_cast.hpp>

QB**** CubeManager::s_Cubies;
std::vector<glm::ivec2> CubeManager::s_SwapIndices;

uint32_t CubeManager::s_VAO;
uint32_t CubeManager::s_EBO;

uint8_t CubeManager::s_SizeX;
uint8_t CubeManager::s_SizeY;
uint8_t CubeManager::s_SizeZ;

std::queue<Move> CubeManager::s_Moves;

void CubeManager::generate(uint8_t sizeX, uint8_t sizeY, uint8_t sizeZ)
{
    Face::generateData();

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

                s_Cubies[i][j][k] = new QB(pos);
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

    // glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, NULL);
            
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
    for (size_t i = 0; i < input.size() - 1;)
    {
        char c = input[i];
        char nextC = input[i + 1];

        int times = 0;

        RotationEnum rotation = RotationEnum::NORMAL;
        switch (nextC)
        {
        case '\'':
            times = 3;
            i += 3;
            break;
        case ' ':
            times = 1;
            i += 2;
            break;
        case '2':
            times = 2;
            i += 3;
            break;
        }

        for (int i = 0; i < times; i++)
        {
            switch (c)
            {
                case 'U': addMove({ FaceEnum::UP, rotation }); break;
                case 'D': addMove({ FaceEnum::DOWN, rotation }); break;
                case 'L': addMove({ FaceEnum::LEFT, rotation }); break;
                case 'R': addMove({ FaceEnum::RIGHT, rotation }); break;
                case 'F': addMove({ FaceEnum::FRONT, rotation }); break;
                case 'B': addMove({ FaceEnum::BACK, rotation }); break;
            }
        }
    }
}

void CubeManager::addMove(Move move)
{
    s_Moves.push(move);
}

void CubeManager::rotate(float dt)
{
    if (s_Moves.empty())
        return;

    Move& move = s_Moves.front();

    float percentage = 1.0f - move.time;
     
    glm::ivec3 rotationAxis;
    uint32_t position;

    for (int i = 0; i < static_cast<int>(move.rotation); i++)
    {
        switch (move.face)
        {
            case FaceEnum::UP:
                rotationAxis = glm::vec3(0, -1, 0);
                position = s_SizeY - 1;
                swapCW(position, s_SizeX, rotationAxis, percentage);
                break;

            case FaceEnum::DOWN:
                rotationAxis = glm::vec3(0, 1, 0);
                position = 0;
                swapCCW(position, s_SizeX, rotationAxis, percentage);
                break;

            case FaceEnum::RIGHT:
                rotationAxis = glm::vec3(-1, 0, 0);
                position = s_SizeX - 1;
                swapCCW(position, s_SizeZ, rotationAxis, percentage);
                break;

            case FaceEnum::LEFT:
                rotationAxis = glm::vec3(1, 0, 0);
                position = 0;
                swapCW(position, s_SizeZ, rotationAxis, percentage);
                break;

            case FaceEnum::FRONT:
                rotationAxis = glm::vec3(0, 0, -1);
                position = s_SizeZ - 1;
                swapCCW(position, s_SizeX, rotationAxis, percentage);
                break;

            case FaceEnum::BACK:
                rotationAxis = glm::vec3(0, 0, 1);
                position = 0;
                swapCW(position, s_SizeX, rotationAxis, percentage);
                break;
        }
    }

    if (move.time <= 0)
    {
        s_Moves.pop();
    }
    move.time -= dt;
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

void CubeManager::swapCW(uint16_t constant, uint16_t width, glm::ivec3 rotationAxis, float percentage)
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

        s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage);
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

        s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage);
    }

    currentIndex = getCurrentIndex(constant, rotationAxis, { 1, 1 });
    s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage);
}

void CubeManager::swapCCW(uint16_t constant, uint16_t width, glm::ivec3 rotationAxis, float percentage)
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

        s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage);
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

        s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage);
    }
    currentIndex = getCurrentIndex(constant, rotationAxis, { 1, 1 });
    s_Cubies[currentIndex.x][currentIndex.y][currentIndex.z]->rotate(rotationAxis, percentage);
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
