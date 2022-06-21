#ifndef CUBE_MANAGER_HPP
#define CUBE_MANAGER_HPP

#include <cstdint>
#include "KRE/KRE.hpp"
#include "Enums.hpp"
#include "MousePicker.hpp"
#include "MoveManager.hpp"

#include "QB.hpp"

#include <vector>
#include <queue>

class CubeManager
{
private:
    QB** m_Cubies; // Graphical Positions
    QB** m_CurrentCubies; // Positions in memory

    std::vector<glm::ivec2> m_SwapIndices;

    uint8_t m_Size;

    MoveManager* m_MoveManager;
public:
    CubeManager();
    CubeManager(MoveManager* moveManager);

    void generate(uint8_t size);

    void destroy();

    void draw(KRE::Shader& shader);
    void update(float dt);

    void applyMoves(std::string input);

    void doMove(Move move);

    glm::ivec2 getLocalPos(glm::ivec3 pos, FaceEnum face);
    LocalEdgeEnum getLocalEdge(glm::ivec3 pos, FaceEnum face);
    LocalCornerEnum getLocalCorner(glm::ivec3 pos, FaceEnum face);

    uint16_t getSize() { return m_Size; };

    QB* getQB(uint16_t x, uint16_t y, uint16_t z) { return m_CurrentCubies[getIndex(x, y, z)]; }
    QB* getQB(glm::ivec3 index) { return m_CurrentCubies[getIndex(index)]; }

    size_t getIndex(uint16_t x, uint16_t y, uint16_t z);
    size_t getIndex(glm::ivec3 index);
private:
    glm::vec3 coordsToPosition(uint16_t x, uint16_t y, uint16_t z);
    glm::ivec3 getCurrentIndex(uint16_t constant, glm::ivec3 axis, glm::ivec2 swapPosition);

    void rotateAnimate(float dt);
    void rotateCurrent(Move& move);

    void rotate(QB*** cubies, Move& move, float dt, bool animate);

    void swapCW(QB*** cubies, uint16_t constant, glm::ivec3 rotationAxis, 
            float percentage, int8_t angleMult, bool animate);
    void swapCCW(QB*** cubies, uint16_t constant, glm::ivec3 rotationAxis, 
            float percentage, int8_t angleMult, bool animate);

    void getSavedPositionsCW(QB*** cubies, std::vector<QB*>& stored, 
            uint16_t constant, glm::ivec3 axis);
    void getSavedPositionsCCW(QB*** cubies, std::vector<QB*>& stored, 
            uint16_t constant, glm::ivec3 axis);
};

#endif
