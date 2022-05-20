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

private:
    Solver() {}
    static void solveCross();
    static void solveCorners();

    static std::vector<QB*> findQB(FaceEnum face, QBTypeEnum faceType);

    static int convertFaceToInt(FaceEnum face);

    static std::array<FaceEnum, 2> convertCornerToFaces(FaceEnum corner);
};

#endif
