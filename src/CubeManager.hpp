#ifndef CUBE_MANAGER_HPP
#define CUBE_MANAGER_HPP

#include <cstdint>
#include "KRE/KRE.hpp"
#include "Enums.hpp"
#include "Solver.hpp"
#include "MousePicker.hpp"
#include "MoveManager.hpp"

#include "QB.hpp"

#include <vector>
#include <queue>

class CubeManager
{
private:
    static QB** s_Cubies; // Graphical Positions
    static QB** s_CurrentCubies; // Positions in memory

    static std::vector<glm::ivec2> s_SwapIndices;

    static uint8_t s_Size;

    static bool s_MousePick;
public:
    static void generate(uint8_t size);

    static void destroy();

    static void draw(KRE::Shader& shader);
    static void update(float dt);

    static void applyMoves(std::string input);

    static void doMove(Move move);

    static glm::ivec2 getLocalPos(glm::ivec3 pos, FaceEnum face);
    static LocalEdgeEnum getLocalEdge(glm::ivec3 pos, FaceEnum face);
    static LocalCornerEnum getLocalCorner(glm::ivec3 pos, FaceEnum face);

    static QB** getCubies() { return s_CurrentCubies; }

    static size_t getIndex(uint16_t x, uint16_t y, uint16_t z);
    static size_t getIndex(glm::ivec3 index);
private:
    CubeManager() = default;

    static glm::vec3 coordsToPosition(uint16_t x, uint16_t y, uint16_t z);
    static glm::ivec3 getCurrentIndex(uint16_t constant, glm::ivec3 axis, glm::ivec2 swapPosition);

    static void rotateAnimate(float dt);
    static void rotateCurrent(Move& move);

    static void rotate(QB*** cubies, Move& move, float dt, bool animate);

    static void swapCW(QB*** cubies, uint16_t constant, glm::ivec3 rotationAxis, float percentage, int8_t angleMult, bool animate);
    static void swapCCW(QB*** cubies, uint16_t constant, glm::ivec3 rotationAxis, float percentage, int8_t angleMult, bool animate);

    static void getSavedPositionsCW(QB*** cubies, std::vector<QB*>& stored, uint16_t constant, glm::ivec3 axis);
    static void getSavedPositionsCCW(QB*** cubies, std::vector<QB*>& stored, uint16_t constant, glm::ivec3 axis);
};

#endif
