#include "Solver.hpp"

QB**** Solver::s_Cubies;
uint16_t Solver::s_SizeX;
uint16_t Solver::s_SizeY;
uint16_t Solver::s_SizeZ;

void Solver::loadCube(QB**** cubies, uint16_t sizeX, uint16_t sizeY, uint16_t sizeZ)
{
    s_Cubies = cubies;

    s_SizeX = sizeX;
    s_SizeY = sizeY;
    s_SizeZ = sizeZ;
}

void Solver::solve()
{
    solveCross();
}

void Solver::solveCross()
{
    FaceEnum face = FaceEnum::UP;
    std::vector<glm::ivec3> whiteEdges = findQB(face, QBTypeEnum::EDGE);

    for (glm::ivec3 pos : whiteEdges)
    {
        QB* current = s_Cubies[pos.z][pos.y][pos.x];

        FaceEnum faces = current->activeFaces;
        FaceEnum otherFace = (faces & face);

        FaceEnum facing = current->getSide(face);
        FaceEnum otherFacing = current->getSide(otherFace);

        if (pos.y == 2 && facing == FaceEnum::UP)
        {
            std::cout << "In position\n";
            continue;
        }
    }
}

std::vector<glm::ivec3> Solver::findQB(FaceEnum face, QBTypeEnum faceType)
{
    std::vector<glm::ivec3> edges;
    for (int z = 0; z < s_SizeZ; z++)
    {
        for (int y = 0; y < s_SizeY; y++)
        {
            for (int x = 0; x < s_SizeX; x++)
            {
                size_t count = s_Cubies[z][y][x]->getFaceCount();
                if (count == static_cast<int>(faceType) && s_Cubies[z][y][x]->hasFace(face))
                {
                    edges.emplace_back(x, y, z);
                }
            }
        }
    }

    return edges;
}
