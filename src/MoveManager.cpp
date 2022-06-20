#include "MoveManager.hpp"
#include <iostream>

#include "Util.hpp"

std::vector<Move> MoveManager::s_Moves;
std::vector<Move> MoveManager::s_SolveMoves;
std::vector<Move> MoveManager::s_ScrambleMoves;

bool MoveManager::s_SolveStore = false;
bool MoveManager::s_ScrambleStore = false;

void MoveManager::addMove(const Move &move)
{
    if (move.rotation == RotationEnum::NONE)
        return;

    s_Moves.push_back(move);

    if (s_SolveStore)
    {
        s_SolveMoves.push_back(move);
    }
    if (s_ScrambleStore)
    {
        s_ScrambleMoves.push_back(move);
    }

    optimiseMoves(s_Moves);
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

const std::vector<Move>& MoveManager::getSolveMoves()
{
    return s_SolveMoves;
}

const std::vector<Move>& MoveManager::getScrambleMoves()
{
    return s_ScrambleMoves;
}

void MoveManager::startSolve()
{
    s_SolveMoves.clear();
    s_SolveStore = true;
}

void MoveManager::endSolve()
{
    s_SolveStore = false;
    optimiseMoves(s_SolveMoves);
}

void MoveManager::startScramble()
{
    s_ScrambleMoves.clear();
    s_ScrambleStore = true;
}

void MoveManager::endScramble()
{
    s_ScrambleStore = false;
    optimiseMoves(s_ScrambleMoves);
}

void MoveManager::optimiseMoves(std::vector<Move>& moves)
{
    if (moves.size() <= 1)
        return;

    for (int i = moves.size() - 1; i >= 1; i--)
    {
        if (i == 1)
        {
            if (moves[0].time != 0)
                continue;
        }

        if (removeMoves(moves[i - 1], moves[i]))
        {
            moves.erase(moves.begin() + i);

            if (moves[i - 1].rotation == RotationEnum::NONE)
            {
                moves.erase(moves.begin() + i - 1);
                i--;
            }

            continue;
        }
        else if (i >= 2 && moves[i - 2].time == 0 && removeMoves(moves[i - 2], moves[i - 1], moves[i]))
        {
            moves.erase(moves.begin() + i);
            if (moves[i - 2].rotation == RotationEnum::NONE)
            {
                moves.erase(moves.begin() + i - 2);
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
        int finalRotation = static_cast<int>(move1.rotation) + static_cast<int>(move2.rotation) + 1;

        finalRotation = positiveMod(finalRotation, 4) - 1;
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
