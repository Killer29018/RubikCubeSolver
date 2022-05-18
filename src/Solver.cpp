#include "Solver.hpp"

#include "CubeManager.hpp"

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
        FaceEnum otherFace = static_cast<FaceEnum>(static_cast<int>(faces) & ~static_cast<int>(face));

        FaceEnum facing = current->getFacingSide(face);
        FaceEnum otherFacing = current->getFacingSide(otherFace);

        if (pos.y == s_SizeY - 1) // Top Row
        {
            if (facing == FaceEnum::UP) // Facing Correct Way
            {
                std::cout << "In position\n";
                continue;
            }
        }
        else if (pos.y != 0) // Middle Row
        {
            LocalEdgeEnum localEdge = CubeManager::getLocalEdge(pos, otherFacing);

            if (otherFacing == otherFace)
            {
                switch(localEdge)
                {
                case LocalEdgeEnum::LEFT:
                    CubeManager::addMove({ otherFacing, RotationEnum::NONE });
                    break;
                case LocalEdgeEnum::RIGHT:
                    CubeManager::addMove({ otherFacing, RotationEnum::PRIME });
                    break;
                default:
                    assert(false && "Unreachable, qb shouldn't be in top or bottom position");
                }
                continue;
            }

            switch (localEdge)
            {
                case LocalEdgeEnum::LEFT:
                    CubeManager::addMove({ otherFacing, RotationEnum::PRIME });
                    break;
                case LocalEdgeEnum::RIGHT:
                    CubeManager::addMove({ otherFacing, RotationEnum::NORMAL });
                    break;
                default:
                    assert(false && "Unreachable. qb shouldn't be in top or bottom position");
            }
        }

        // QB is on bottom face
        if (otherFace == otherFacing) // Correct side
        {
            CubeManager::addMove({ otherFacing, RotationEnum::TWICE });
        }
        else // Wrong Side
        {
            int currentInt = (int)std::log2(static_cast<int>(otherFacing));
            int targetInt = (int)std::log2(static_cast<int>(otherFace));

            int change = ((targetInt - currentInt + 1) % 4) - 1;
            std::cout << currentInt << " : " << targetInt << " : " << change << "\n";

            RotationEnum rotation = static_cast<RotationEnum>(change);

            CubeManager::addMove({ FaceEnum::DOWN, rotation });
            CubeManager::addMove({ otherFace, RotationEnum::TWICE });
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
