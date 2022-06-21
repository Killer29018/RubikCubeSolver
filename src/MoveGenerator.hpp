#ifndef MOVE_GENERATOR_HPP
#define MOVE_GENERATOR_HPP

#include "CubeManager.hpp"
#include "MoveManager.hpp"

class MoveGenerator
{
private:
    CubeManager* m_CubeManager;
    MoveManager* m_MoveManager;
public:
    MoveGenerator() = default;
    MoveGenerator(CubeManager* cubeManager, MoveManager* moveManager);

    void generateMoves();
};

#endif
