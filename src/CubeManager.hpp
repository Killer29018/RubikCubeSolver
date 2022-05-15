#ifndef CUBE_MANAGER_HPP
#define CUBE_MANAGER_HPP

#include <cstdint>
#include "KRE/KRE.hpp"
#include "Enums.hpp"

#include "QB.hpp"

#include <vector>
#include <queue>

class CubeManager
{
private:
    static QB**** s_Cubies;

    static std::vector<glm::ivec2> s_SwapIndices;

    static uint32_t s_VAO, s_EBO;

    static uint8_t s_SizeX, s_SizeY, s_SizeZ;

    static std::queue<Move> s_Moves;
public:
    static void generate(uint8_t sizeX, uint8_t sizeY, uint8_t sizeZ);

    static void destroy();

    static void draw(KRE::Shader& shader);
    static void update(float dt);

    static void scramble(std::string input);

    static void addMove(Move move);
private:
    CubeManager() = default;

    static void rotate(float dt);

    static void generateVAO();

    static glm::vec3 coordsToPosition(uint16_t x, uint16_t y, uint16_t z);

    static glm::ivec3 getCurrentIndex(uint16_t constant, glm::ivec3 axis, glm::ivec2 swapPosition);

    static void swapCW(uint16_t constant, uint16_t width, glm::ivec3 rotationAxis, float percentage);
    static void swapCCW(uint16_t constant, uint16_t width, glm::ivec3 rotationAxis, float percentage);

    static void getSavedPositionsCW(std::vector<QB*>& stored, int width, uint16_t constant, glm::ivec3 axis);
    static void getSavedPositionsCCW(std::vector<QB*>& stored, int width, uint16_t constant, glm::ivec3 axis);
};

#endif
