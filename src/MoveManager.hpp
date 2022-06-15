#ifndef MOVE_MANAGER_HPP
#define MOVE_MANAGER_HPP

#include <vector>

#include "Enums.hpp"

class MoveManager
{
private:
    static std::vector<Move> s_Moves;
    static std::vector<Move> s_SolveMoves;
    static std::vector<Move> s_ScrambleMoves;

    static bool s_SolveStore;
    static bool s_ScrambleStore;
public:
    static void addMove(const Move& move);

    static Move* getMove();
    static bool moveFinished();
    static Move removeMove();

    static bool isEmpty();

    static const std::vector<Move>& getSolveMoves();
    static const std::vector<Move>& getScrambleMoves();

    static void startSolve();
    static void endSolve();

    static void startScramble();
    static void endScramble();
private:
    MoveManager() = default;
    ~MoveManager() = default;

    static void optimiseMoves(std::vector<Move>& moves);
    static bool removeMoves(Move& move1, const Move& move2);
    static bool removeMoves(Move& move1, const Move& move2, const Move& move3);
};

#endif
