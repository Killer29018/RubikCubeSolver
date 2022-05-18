#include "Solver.hpp"

#include "CubeManager.hpp"

QB**** Solver::s_Cubies;
uint16_t Solver::s_SizeX;
uint16_t Solver::s_SizeY;
uint16_t Solver::s_SizeZ;

static int positiveMod(int i, int n)
{
    return (i % n + n) % n;
}

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
    std::vector<QB*> whiteEdges = findQB(face, QBTypeEnum::EDGE);

    for (int i = 0; i < whiteEdges.size(); i++)
    {
        QB* current = whiteEdges[i];
        glm::ivec3 pos = current->index;

        FaceEnum faces = current->activeFaces;
        FaceEnum otherFace = static_cast<FaceEnum>(static_cast<int>(faces) & ~static_cast<int>(face));

        FaceEnum facing = current->getFacingSide(face);
        FaceEnum otherFacing = current->getFacingSide(otherFace);

        if (pos.y == s_SizeY - 1) // Top Row
        {
            if (facing == FaceEnum::UP) // Facing Correct Way
            {
                continue;
            }

            // On top facing sideways
            assert(false && "Not implemented"); // TODO
        }
        else if (pos.y != 0) // Middle Row
        {
            LocalEdgeEnum localEdge = CubeManager::getLocalEdge(pos, otherFacing);

            if (otherFacing == otherFace)
            {
                // In the middle row with the otherface on the correct side, so move to top of cube
                switch(localEdge)
                {
                case LocalEdgeEnum::LEFT:
                    CubeManager::doMove({ otherFacing, RotationEnum::NORMAL });
                    break;
                case LocalEdgeEnum::RIGHT:
                    CubeManager::doMove({ otherFacing, RotationEnum::PRIME });
                    break;
                default:
                    assert(false && "Unreachable, qb shouldn't be in top or bottom position");
                }

                continue;
            }

            // In the middle row with the otherface not on the correct side, so move to bottom of cube
            switch (localEdge)
            {
                case LocalEdgeEnum::LEFT:
                    CubeManager::doMove({ otherFacing, RotationEnum::PRIME });
                    break;
                case LocalEdgeEnum::RIGHT:
                    CubeManager::doMove({ otherFacing, RotationEnum::NORMAL });
                    break;
                default:
                    assert(false && "Unreachable. qb shouldn't be in top or bottom position");
            }
        }

        // QB is on bottom face
        if (otherFace == otherFacing) // Correct side
        {
            if (facing == FaceEnum::DOWN) // Facing Down
            {
                CubeManager::doMove({ otherFacing, RotationEnum::TWICE });
                continue;
            }

            // Facing Sideways
            assert(false && "Not Implemented"); // TODO
        }
        else // Wrong Side
        {
            if (facing == FaceEnum::DOWN) // Facing Down
            {
                int currentInt = (int)std::log2(static_cast<int>(otherFacing));
                int targetInt = (int)std::log2(static_cast<int>(otherFace));

                int change = positiveMod(targetInt - currentInt + 1, 4) - 1;

                RotationEnum rotation = static_cast<RotationEnum>(change);

                CubeManager::doMove({ FaceEnum::DOWN, rotation });
                CubeManager::doMove({ otherFace, RotationEnum::TWICE });
                continue;
            }

            // Facing Sideways
            assert(false && "Not implemented solving on bottom face wrong direction"); // TODO
        }
    }
}

std::vector<QB*> Solver::findQB(FaceEnum face, QBTypeEnum faceType)
{
    std::vector<QB*> edges;
    for (int z = 0; z < s_SizeZ; z++)
    {
        for (int y = 0; y < s_SizeY; y++)
        {
            for (int x = 0; x < s_SizeX; x++)
            {
                size_t count = s_Cubies[z][y][x]->getFaceCount();
                if (count == static_cast<int>(faceType) && s_Cubies[z][y][x]->hasFace(face))
                {
                    edges.push_back(s_Cubies[z][y][x]);
                }
            }
        }
    }

    return edges;
}
