#include "MoveManager.hpp"
#include <iostream>

std::vector<Move> MoveManager::s_Moves;

void MoveManager::addMove(const Move &move)
{
    if (move.rotation == RotationEnum::NONE)
        return;

    s_Moves.push_back(move);

    optimiseMoves();
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

void MoveManager::optimiseMoves()
{
    if (s_Moves.size() <= 1)
        return;

    for (int i = s_Moves.size() - 1; i >= 1; i--)
    {
        if (i == 1)
        {
            if (s_Moves[0].time != 0)
                continue;
        }

        if (removeMoves(s_Moves[i - 1], s_Moves[i], 1))
        {
            s_Moves.erase(s_Moves.begin() + i);

            if (s_Moves[i - 1].rotation == RotationEnum::NONE)
            {
                s_Moves.erase(s_Moves.begin() + i - 1);
                i--;
            }


            continue;
        }
        else if (i >= 3 && removeMoves(s_Moves[i - 2], s_Moves[i], 2))
        {
            s_Moves.erase(s_Moves.begin() + i);
            continue;
        }
    }
}

bool MoveManager::removeMoves(Move& move1, const Move& move2, int offset)
{
    if (move1 == move2 && offset == 1) // Same Rotation immediately after eachother
    {
        if (move1.rotation == move2.rotation)
        {
            if (move1.rotation == RotationEnum::NORMAL || move1.rotation == RotationEnum::PRIME)
            {
                move1.rotation = RotationEnum::TWICE;
            }
            else if (move1.rotation == RotationEnum::TWICE)
                move1.rotation = RotationEnum::NONE;

            return true;
        }
    }
    return false;
}
