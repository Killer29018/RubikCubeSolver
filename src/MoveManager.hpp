#ifndef MOVE_MANAGER_HPP
#define MOVE_MANAGER_HPP

#include <vector>

#include "Enums.hpp"

class MoveManager
{
private:
    static std::vector<Move> s_Moves;
public:
    static void addMove(const Move& move);

    static Move* getMove();
    static bool moveFinished();
    static Move removeMove();

    static bool isEmpty();
private:
    MoveManager() = default;
    ~MoveManager() = default;

    static void optimiseMoves();
    static bool removeMoves(Move& move1, const Move& move2, int offset = 1);
};

#endif
