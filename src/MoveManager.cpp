#include "MoveManager.hpp"
#include <iostream>

std::vector<Move> MoveManager::s_Moves;

void MoveManager::addMove(const Move &move)
{
    if (move.rotation == RotationEnum::NONE)
        return;

    s_Moves.push_back(move);
}

Move* MoveManager::getMove()
{
    return &s_Moves[0];
}

bool MoveManager::moveFinished()
{
    if (s_Moves[0].time >= 1.0f)
    {
        return true;
    }
    return false;
}

Move MoveManager::removeMove()
{
    Move m = s_Moves[0];
    s_Moves.erase(s_Moves.begin());
    return m;
}

bool MoveManager::isEmpty()
{
    return s_Moves.empty();
}
