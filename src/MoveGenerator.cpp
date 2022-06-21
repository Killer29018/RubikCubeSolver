#include "MoveGenerator.hpp"

#include <string>
#include <iostream>

MoveGenerator::MoveGenerator(CubeManager* cubeManager, MoveManager* moveManager)
    : m_CubeManager(cubeManager), m_MoveManager(moveManager) { }

void MoveGenerator::generateMoves()
{
    const std::string moves[] = { "F", "R", "B", "L", "U", "D" };
    const size_t moveCount = 6;
    const std::string prefix[] = { "", "\'", "" };
    const size_t prefixCount = 3;

    m_MoveManager->reset();

    std::string finalMoves = "";

    constexpr size_t totalMoves = 20;

    for (size_t i = 0; i < totalMoves; i++)
    {
        int moveIndex = rand() % moveCount;
        int prefixIndex = rand() % prefixCount;

        finalMoves += moves[moveIndex] + prefix[prefixIndex];

        if (i < totalMoves - 1)
            finalMoves += " ";
    }

    std::cout << finalMoves << "\n";
}
