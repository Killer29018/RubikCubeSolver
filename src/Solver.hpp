#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include "Enums.hpp"
#include "QB.hpp"

class Solver
{
public:
private:
    static QB**** s_Cubies;
    static uint16_t s_SizeX, s_SizeY, s_SizeZ;
public:
    static void loadCube(QB**** cubies, uint16_t sizeX, uint16_t sizeY, uint16_t sizeZ);

    static void solve();

    static std::vector<glm::ivec3> findQB(FaceEnum face, QBTypeEnum faceType);
private:
    Solver() {}
};

#endif
