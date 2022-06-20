#ifndef MOVE_MANAGER_HPP
#define MOVE_MANAGER_HPP

#include <vector>

#include "Enums.hpp"

class MoveManager
{
private:
    std::vector<Move> s_Moves;
public:
    MoveManager() = default;
    ~MoveManager() = default;

    void addMove(const Move& move);

    const std::vector<Move>& getAllMoves();

    Move* getMove();
    bool moveFinished();
    Move removeMove();

    bool isEmpty();
    void reset();
private:
    void optimiseMoves();
    bool removeMoves(Move& move1, const Move& move2);
    bool removeMoves(Move& move1, const Move& move2, const Move& move3);
};

#endif
