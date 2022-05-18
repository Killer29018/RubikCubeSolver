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
                if (otherFace != otherFacing) // Not correct position
                {
                    CubeManager::doMove({ otherFacing, RotationEnum::TWICE }); // Move to bottom face
                }
                else
                {
                    continue;
                }
            }
            else
            {
                // On top facing sideways
                int currentInt = convertFaceToInt(facing);
                int targetInt = convertFaceToInt(otherFace);

                if (currentInt == targetInt)
                {
                    CubeManager::doMove({ facing, RotationEnum::NORMAL });
                    CubeManager::doMove({ FaceEnum::UP, RotationEnum::PRIME });

                    facing = current->getFacingSide(otherFace);
                    CubeManager::doMove({ facing, RotationEnum::NORMAL });

                    CubeManager::doMove({ FaceEnum::UP, RotationEnum::NORMAL });
                    continue;
                }
                else if ((currentInt + targetInt) % 2 == 0) // Opposite sides
                {
                    CubeManager::doMove({ facing, RotationEnum::NORMAL });
                    CubeManager::doMove({ FaceEnum::UP, RotationEnum::NORMAL });
                    CubeManager::doMove({ current->getFacingSide(otherFace), RotationEnum::NORMAL });
                    CubeManager::doMove({ FaceEnum::UP, RotationEnum::PRIME });
                    continue;
                }
                else // Next to
                {
                    if (targetInt - currentInt == 1) // Right
                    {
                        CubeManager::doMove({ facing, RotationEnum::NORMAL });
                        CubeManager::doMove({ current->getFacingSide(otherFace), RotationEnum::NORMAL });
                    }
                    else if (targetInt - currentInt == -1) // Left
                    {
                        CubeManager::doMove({ facing, RotationEnum::PRIME });
                        CubeManager::doMove({ current->getFacingSide(otherFace), RotationEnum::PRIME });
                    }
                    continue;
                }
            }
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
                    CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::NORMAL });
                    CubeManager::doMove({ otherFacing, RotationEnum::NORMAL });
                    break;
                case LocalEdgeEnum::RIGHT:
                    CubeManager::doMove({ otherFacing, RotationEnum::NORMAL });
                    CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::NORMAL });
                    CubeManager::doMove({ otherFacing, RotationEnum::PRIME });
                    break;
                default:
                    assert(false && "Unreachable. qb shouldn't be in top or bottom position");
            }
        }

        // QB is on bottom side
        facing = current->getFacingSide(face);
        otherFacing = current->getFacingSide(otherFace);
        if (otherFace == otherFacing) // Correct side, Correctly facing due to recalculating facing direction
        {
            CubeManager::doMove({ otherFacing, RotationEnum::TWICE });
            continue;
        }
        else // Wrong Side or wrong direction
        {
            int currentInt = convertFaceToInt(otherFacing);
            int targetInt = convertFaceToInt(otherFace);

            if (facing == FaceEnum::DOWN) // Facing Down
            {
                int change = positiveMod(targetInt - currentInt + 1, 4) - 1;

                RotationEnum rotation = static_cast<RotationEnum>(change);

                CubeManager::doMove({ FaceEnum::DOWN, rotation });
                CubeManager::doMove({ otherFace, RotationEnum::TWICE });
                continue;
            }

            // Facing Sideways
            currentInt = convertFaceToInt(facing);
            if ((currentInt + targetInt) % 2 == 0) // Opposite Sides or same
            {
                CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::NORMAL });

                currentInt = positiveMod(currentInt + 1, 4);
                facing = static_cast<FaceEnum>(std::pow(2, currentInt));
            }

            int change = positiveMod(targetInt - currentInt + 1, 4) - 1;
            if (change == 1) // Turn CCW, Right of target
            {
                CubeManager::doMove({ facing, RotationEnum::PRIME });
                CubeManager::doMove({ otherFace, RotationEnum::NORMAL });
                CubeManager::doMove({ facing, RotationEnum::NORMAL });
            }
            else if (change == -1) // Turn CW, Left of target
            {
                CubeManager::doMove({ facing, RotationEnum::NORMAL });
                CubeManager::doMove({ otherFace, RotationEnum::PRIME });
                CubeManager::doMove({ facing, RotationEnum::PRIME });
            }
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

int Solver::convertFaceToInt(FaceEnum face)
{
    return (int)std::log2(static_cast<int>(face));
}
