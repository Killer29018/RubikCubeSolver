#include "MoveManager.hpp"
#include <iostream>

#include "Util.hpp"

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

        if (removeMoves(s_Moves[i - 1], s_Moves[i]))
        {
            s_Moves.erase(s_Moves.begin() + i);

            if (s_Moves[i - 1].rotation == RotationEnum::NONE)
            {
                s_Moves.erase(s_Moves.begin() + i - 1);
                i--;
            }

            continue;
        }
        else if (i >= 2 && s_Moves[i - 2].time == 0 && removeMoves(s_Moves[i - 2], s_Moves[i - 1], s_Moves[i]))
        {
            s_Moves.erase(s_Moves.begin() + i);
            if (s_Moves[i - 2].rotation == RotationEnum::NONE)
            {
                s_Moves.erase(s_Moves.begin() + i - 2);
                i--;
            }
            continue;
        }
    }
}

bool MoveManager::removeMoves(Move& move1, const Move& move2)
{
    if (move1 == move2) // Same Face Rotation immediately after eachother
    {
        int finalRotation = static_cast<int>(move1.rotation) + static_cast<int>(move2.rotation);

        finalRotation = positiveMod(finalRotation, 4);
        RotationEnum rot = static_cast<RotationEnum>(finalRotation);

        move1.rotation = rot;
        return true;
    }

    return false;
}

bool MoveManager::removeMoves(Move& move1, const Move& move2, const Move& move3)
{
    if (move1 == move3 && move1 == oppositeMove(move2)) // Move1 and Move3 are same while move2 is opposite
    {
        return removeMoves(move1, move3);
    }
    return false;
}
