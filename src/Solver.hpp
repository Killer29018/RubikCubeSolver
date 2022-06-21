#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include "Enums.hpp"
#include "QB.hpp"

#include "Util.hpp"
#include "MoveManager.hpp"
#include "CubeManager.hpp"

class Solver
{
private:
    CubeManager* m_CubeManager;
    MoveManager* m_MoveManager;

    uint16_t m_Size;
public:
    Solver();
    Solver(CubeManager* cubeManager, MoveManager* moveManager);

    void solve();
private:
    void alignCenters();
    void solveCross();
    void solveCorners();
    void solveMiddleLayer();
    void solveBottomCross();
    void alignBottomCross();
    void positionBottomCorners();
    void reorientateBottomCorners();

    void insertEdge(FaceEnum currentFace, LocalEdgeEnum targetEdge);

    std::vector<QB*> findQB(FaceEnum face, QBTypeEnum faceType);
    std::vector<QB*> findNotQB(FaceEnum face, QBTypeEnum faceType);
    int getCountFacing(FaceEnum face, QBTypeEnum faceType);

    FaceEnum getOtherFace(QB* qb, FaceEnum knownSide);

    int convertFaceToInt(FaceEnum face);
    FaceEnum convertIntToFace(int faceInt);

    bool cornerInCorrectPosition(QB* corner);
    bool cornerInDiagonalPosition(FaceEnum face, QB* corner1, QB* corner2);
    bool cornerInAdjacentPosition(FaceEnum face, QB* corner1, QB* corner2);

    std::array<FaceEnum, 2> convertDualFaceToFaces(FaceEnum faces);


    RotationEnum getRotationToTarget(int current, int target)
    {
        int change = positiveMod(target - current + 1, 4) - 1;

        return static_cast<RotationEnum>(change);
    }

    RotationEnum getRotationToTarget(FaceEnum current, FaceEnum target)
    {
        return getRotationToTarget(convertFaceToInt(current), convertFaceToInt(target));
    }

    template<typename T>
    RotationEnum getRotationToTarget(T current, T target)
    {
        return getRotationToTarget(static_cast<int>(current), static_cast<int>(target));
    }
};


#endif
