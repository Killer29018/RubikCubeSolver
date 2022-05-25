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
    static void solveMiddleLayer();
    static void solveBottomCross();
    static void alignBottomCross();
    static void positionBottomCorners();
    static void reorientateBottomCorners();

    static void insertEdge(FaceEnum currentFace, LocalEdgeEnum targetEdge);

    static std::vector<QB*> findQB(FaceEnum face, QBTypeEnum faceType);
    static std::vector<QB*> findNotQB(FaceEnum face, QBTypeEnum faceType);
    static int getCountFacing(FaceEnum face, QBTypeEnum faceType);

    FaceEnum getOtherSide(QB* qb, FaceEnum knownSide);

    static int convertFaceToInt(FaceEnum face);
    static FaceEnum convertIntToFace(int faceInt);

    static bool cornerInCorrectPosition(QB* corner);


    static RotationEnum getRotationToTarget(int current, int target)
    {
        int change = positiveMod(target - current + 1, 4) - 1;

        return static_cast<RotationEnum>(change);
    }

    static RotationEnum getRotationToTarget(FaceEnum current, FaceEnum target)
    {
        return getRotationToTarget(convertFaceToInt(current), convertFaceToInt(target));
    }

    template<typename T>
    static RotationEnum getRotationToTarget(T current, T target)
    {
        return getRotationToTarget(static_cast<int>(current), static_cast<int>(target));
    }

    static std::array<FaceEnum, 2> convertDualFaceToFaces(FaceEnum faces);

    static int positiveMod(int dividend, int divisor)
    {
    return (dividend % divisor + divisor) % divisor;
    }
};


#endif
