#include "MoveManager.hpp"
#include <iostream>

#include "Util.hpp"

void MoveManager::addMove(const Move &move)
{
    if (move.rotation == RotationEnum::NONE)
        return;

    m_Moves.push_back(move);
    m_TotalMoves.push_back(move);

    m_Optimised = false;

    optimiseMoves(m_Moves);
}

const std::vector<Move>& MoveManager::getAllMoves()
{
    if (!m_Optimised)
    {
        optimiseMoves(m_TotalMoves);
        m_Optimised = true;
    }

    return m_TotalMoves;
}

Move* MoveManager::getMove()
{
    return &m_Moves[0];
}

bool MoveManager::moveFinished()
{
    if (m_Moves[0].time >= 1.0f)
    {
        return true;
    }
    return false;
}

Move MoveManager::removeMove()
{
    Move m = m_Moves[0];
    m_Moves.erase(m_Moves.begin());
    return m;
}

bool MoveManager::isEmpty()
{
    return m_Moves.empty();
}

void MoveManager::reset()
{
    m_TotalMoves.clear();

    m_Optimised = true;
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
