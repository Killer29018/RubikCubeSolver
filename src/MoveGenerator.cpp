#include "MoveGenerator.hpp"

#include <string>
#include <iostream>

MoveGenerator::MoveGenerator(CubeManager* cubeManager, MoveManager* moveManager)
    : m_CubeManager(cubeManager), m_MoveManager(moveManager) { }

void MoveGenerator::generateMoves()
{
    const size_t faceCount = 6;
    const size_t rotationCount = 3;

    m_MoveManager->reset();

    const size_t totalMoves = 25;

    for (size_t i = 0; i < totalMoves; i++)
    {
        int faceIndex = rand() % faceCount;
        FaceEnum face = static_cast<FaceEnum>(1 << faceIndex);

        int rotationIndex = rand() % rotationCount;
        if (rotationIndex == 0)
            rotationIndex--;

        RotationEnum rotation = static_cast<RotationEnum>(rotationIndex);

        Move m = Move(face, rotation);

        m_CubeManager->doMove(m);
    }
}
