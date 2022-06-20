#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include "Enums.hpp"
#include "QB.hpp"

#include "Util.hpp"
#include "MoveManager.hpp"

class Solver
{
public:
private:
    static QB** s_Cubies;
    static uint16_t s_Size;
public:
    static void loadCube(QB** cubies, uint16_t size);

    static void solve(MoveManager* moveManager);
private:
    Solver() {}
    static void alignCenters();
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

    static FaceEnum getOtherFace(QB* qb, FaceEnum knownSide);

    static int convertFaceToInt(FaceEnum face);
    static FaceEnum convertIntToFace(int faceInt);

    static bool cornerInCorrectPosition(QB* corner);
    static bool cornerInDiagonalPosition(FaceEnum face, QB* corner1, QB* corner2);
    static bool cornerInAdjacentPosition(FaceEnum face, QB* corner1, QB* corner2);

    static std::array<FaceEnum, 2> convertDualFaceToFaces(FaceEnum faces);


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
};


#endif
