#ifndef CUBE_MANAGER_HPP
#define CUBE_MANAGER_HPP

#include <cstdint>
#include "KRE/KRE.hpp"

#include "QB.hpp"

#include <vector>

enum class RotationType 
{
    NONE    = 0,
    NORMAL  = 1,
    TWICE   = 2,
    PRIME   = 3,
};

class CubeManager
{
private:
    static QB**** s_Cubies;

    static std::vector<glm::ivec2> s_SwapIndices;

    static uint8_t s_SizeX, s_SizeY, s_SizeZ;
public:
    static void generate(uint8_t sizeX, uint8_t sizeY, uint8_t sizeZ);

    static void destroy();

    static void draw(KRE::Shader& shader);

    static void rotate(FaceFlag face, RotationType type);
private:
    CubeManager() = default;

    static glm::vec3 coordsToPosition(uint16_t x, uint16_t y, uint16_t z);
};

#endif
