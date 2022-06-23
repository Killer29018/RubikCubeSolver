#ifndef MOVE_MANAGER_HPP
#define MOVE_MANAGER_HPP

#include <vector>

#include "Enums.hpp"

class MoveManager
{
private:
    std::vector<Move> m_Moves;
    std::vector<Move> m_TotalMoves;

    bool m_Optimised = false;
public:
    MoveManager() = default;
    ~MoveManager() = default;

    void addMove(const Move& move);

    bool movesUpdated() { return !m_Optimised; }
    const std::vector<Move>& getAllMoves();

    Move* getMove();
    bool moveFinished();
    Move removeMove();

    bool isEmpty();
    void reset();
private:
    void optimiseMoves(std::vector<Move>& moves);
    bool removeMoves(Move& move1, const Move& move2);
    bool removeMoves(Move& move1, const Move& move2, const Move& move3);
};

#endif
