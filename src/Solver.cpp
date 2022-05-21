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

    solveCorners();

    solveMiddleLayer();

    solveBottomCross();
}

void Solver::solveCross()
{
    FaceEnum face = FaceEnum::UP;
    std::vector<QB*> topEdges = findQB(face, QBTypeEnum::EDGE);

    for (int i = 0; i < topEdges.size(); i++)
    {
        QB* current = topEdges[i];
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
                else // Correct Position
                {
                    continue;
                }
            }
            else // Facing Wrong way
            {
                // On top facing sideways
                int currentInt = convertFaceToInt(facing);
                int targetInt = convertFaceToInt(otherFace);

                if (currentInt == targetInt) // Top face is looking at the right side
                {
                    CubeManager::doMove({ facing, RotationEnum::NORMAL });
                    CubeManager::doMove({ FaceEnum::UP, RotationEnum::PRIME });

                    facing = current->getFacingSide(otherFace);
                    CubeManager::doMove({ facing, RotationEnum::NORMAL });

                    CubeManager::doMove({ FaceEnum::UP, RotationEnum::NORMAL });
                    continue;
                }
                else if ((currentInt + targetInt) % 2 == 0) // Top face is looking at the opposite side
                {
                    CubeManager::doMove({ facing, RotationEnum::NORMAL });
                    CubeManager::doMove({ FaceEnum::UP, RotationEnum::NORMAL });
                    CubeManager::doMove({ current->getFacingSide(otherFace), RotationEnum::NORMAL });
                    CubeManager::doMove({ FaceEnum::UP, RotationEnum::PRIME });
                    continue;
                }
                else // Top face is looking at adjacent side
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

                currentInt = positiveMod(currentInt + 1, 4); // Rotate Clockwise
                facing = convertIntToFace(currentInt);
            }

            int change = positiveMod(targetInt - currentInt + 1, 4) - 1;
            RotationEnum rotation = static_cast<RotationEnum>(change);
            if (rotation == RotationEnum::NORMAL) // Turn CW, Right of target
            {
                CubeManager::doMove({ facing, RotationEnum::PRIME });
                CubeManager::doMove({ otherFace, RotationEnum::NORMAL });
                CubeManager::doMove({ facing, RotationEnum::NORMAL });
            }
            else if (rotation == RotationEnum::PRIME) // Turn CCW, Left of target
            {
                CubeManager::doMove({ facing, RotationEnum::NORMAL });
                CubeManager::doMove({ otherFace, RotationEnum::PRIME });
                CubeManager::doMove({ facing, RotationEnum::PRIME });
            }
        }
    }
}

void Solver::solveCorners()
{
    FaceEnum face = FaceEnum::UP;

    std::vector<QB*> topCorners = findQB(face, QBTypeEnum::CORNER);

    for (int i = 0; i < topCorners.size(); i++)
    {
        QB* current = topCorners[i];
        glm::ivec3 pos = current->index;

        FaceEnum faces = current->activeFaces;

        FaceEnum otherFaces = static_cast<FaceEnum>(static_cast<int>(faces) & ~static_cast<int>(face));
        std::array<FaceEnum, 2> corners = convertDualFaceToFaces(otherFaces);

        FaceEnum facing = current->getFacingSide(face);

        FaceEnum cornerFacing[2];
        cornerFacing[0] = current->getFacingSide(corners[0]);
        cornerFacing[1] = current->getFacingSide(corners[1]);

        if (pos.y == s_SizeY - 1) // Top Row
        {
            if (facing == FaceEnum::UP) // Facing correct way
            {
                if (cornerFacing[0] == corners[0] && cornerFacing[1] == corners[1])
                {
                    continue;
                }
                else // Facing Up but in wrong position
                {
                    LocalCornerEnum localCorner = CubeManager::getLocalCorner(pos, cornerFacing[0]);

                    switch (localCorner)
                    {
                    case LocalCornerEnum::TOP_LEFT:
                        CubeManager::doMove({ cornerFacing[0], RotationEnum::PRIME });
                        CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::PRIME });
                        CubeManager::doMove({ cornerFacing[0], RotationEnum::NORMAL });
                        break;
                    case LocalCornerEnum::TOP_RIGHT:
                        CubeManager::doMove({ cornerFacing[0], RotationEnum::NORMAL });
                        CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::NORMAL });
                        CubeManager::doMove({ cornerFacing[0], RotationEnum::PRIME });
                        break;

                    default:
                        assert(false && "Not possible");
                    }

                    // Corner is moved to bottom of cube with top face facing out
                }
            }
            else
            {
                // Top is facing sideways at top of cube
                LocalCornerEnum localCorner = CubeManager::getLocalCorner(pos, facing);

                switch(localCorner)
                {
                case LocalCornerEnum::TOP_LEFT:
                    CubeManager::doMove({ facing, RotationEnum::PRIME });
                    CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::PRIME });
                    CubeManager::doMove({ facing, RotationEnum::NORMAL });
                    break;
                case LocalCornerEnum::TOP_RIGHT:
                    CubeManager::doMove({ facing, RotationEnum::NORMAL });
                    CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::NORMAL });
                    CubeManager::doMove({ facing, RotationEnum::PRIME });
                    break;

                default:
                    assert(false && "Not possible");
                }

                // Corner is moved to bottom of cube with top face facing out
            }
        }

        // Corner is at bottom of cube
        if (facing == FaceEnum::DOWN)
        {
            // Rotate Corner so its facing out
            FaceEnum chosen = corners[0];
            FaceEnum chosenFacing = current->getFacingSide(chosen);

            FaceEnum target = corners[1];

            int currentInt = convertFaceToInt(chosenFacing);
            int targetInt = convertFaceToInt(target);

            int change = positiveMod(targetInt - currentInt + 1, 4) - 1;
            RotationEnum rotation = static_cast<RotationEnum>(change);

            CubeManager::doMove({ FaceEnum::DOWN, rotation });

            CubeManager::doMove({ chosen, RotationEnum::PRIME });
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::TWICE });
            CubeManager::doMove({ chosen, RotationEnum::NORMAL });
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::NORMAL });
        }

        // Corner has top face and another face facing outwards
        
        // Align the face that is facing out with the correct face
        FaceEnum chosen = corners[0];
        FaceEnum chosenFacing = current->getFacingSide(chosen);
        if (current->getFacingSide(chosen) == FaceEnum::DOWN)
        {
            chosen = corners[1];
            chosenFacing = current->getFacingSide(chosen);
        }

        if (chosenFacing != chosen)
        {
            int currentInt = convertFaceToInt(chosenFacing);
            int targetInt = convertFaceToInt(chosen);

            int change = positiveMod(targetInt - currentInt + 1, 4) - 1;
            RotationEnum rotation = static_cast<RotationEnum>(change);
            CubeManager::doMove({ FaceEnum::DOWN, rotation });
            chosenFacing = current->getFacingSide(chosen);
        }

        facing = current->getFacingSide(face);

        // Corner has top facing out, and the other side aligned
        LocalCornerEnum localCorner = CubeManager::getLocalCorner(current->index, chosenFacing);

        switch (localCorner)
        {
        case LocalCornerEnum::BOTTOM_RIGHT:
            CubeManager::doMove({ facing, RotationEnum::PRIME });
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::PRIME });
            CubeManager::doMove({ facing, RotationEnum::NORMAL });
            break;
        case LocalCornerEnum::BOTTOM_LEFT:
            CubeManager::doMove({ facing, RotationEnum::NORMAL });
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::NORMAL });
            CubeManager::doMove({ facing, RotationEnum::PRIME });
            break;

        default:
            assert(false && "Not possible");
        }
    }
}

void Solver::solveMiddleLayer()
{
    FaceEnum bottomFace = FaceEnum::DOWN;

    std::vector<QB*> nonBottomEdges = findNotQB(bottomFace, QBTypeEnum::EDGE);

    for (int i = 0; i < nonBottomEdges.size(); i++)
    {
        QB* current = nonBottomEdges[i];
        glm::ivec3 pos = current->index;

        if (current->hasFace(FaceEnum::UP)) // Already solved
            continue;

        // std::cout << "Non Top: " << current->index.x << " : " << current->index.y << " : " << current->index.z << "\n";

        std::array<FaceEnum, 2> faces = convertDualFaceToFaces(current->activeFaces);

        FaceEnum facing[2];
        facing[0] = current->getFacingSide(faces[0]);
        facing[1] = current->getFacingSide(faces[1]);

        if (pos.y != 0) // In the middle
        {
            if (facing[0] == faces[0] && facing[1] == faces[1]) // Correctly positioned
            {
                continue;
            }

            // Either in the correct slot facing wrong, or in the wrong slot
            // Move out of slot

            LocalEdgeEnum localEdge = CubeManager::getLocalEdge(pos, facing[0]);

            insertEdge(facing[0], localEdge);
        }

        // Piece is at bottom of cube
        FaceEnum chosenFace = faces[0];

        if (current->getFacingSide(chosenFace) == FaceEnum::DOWN)
        {
            chosenFace = faces[1];
        }
        FaceEnum chosenFacing = current->getFacingSide(chosenFace);

        if (chosenFacing != chosenFace) // Rotate into place
        {
            int currentInt = convertFaceToInt(chosenFacing);
            int targetInt = convertFaceToInt(chosenFace);

            int change = positiveMod(targetInt - currentInt + 1, 4) - 1;
            RotationEnum rotation = static_cast<RotationEnum>(change);

            CubeManager::doMove({ FaceEnum::DOWN, rotation });
        }

        // Piece is above where it needs to be
        int currentInt = convertFaceToInt(chosenFace);
        FaceEnum otherFace = static_cast<FaceEnum>(static_cast<int>(current->activeFaces) & ~(static_cast<int>(chosenFace)));
        int targetInt = convertFaceToInt(otherFace);
        int change = positiveMod(targetInt - currentInt + 1, 4) - 1;
        LocalEdgeEnum localEdge = static_cast<LocalEdgeEnum>(change + 1);

        insertEdge(chosenFace, localEdge);
    }
}

void Solver::solveBottomCross()
{
    std::vector<QB*> bottomEdges = findQB(FaceEnum::DOWN, QBTypeEnum::EDGE);

    const std::string moves = "F L D L' D' F'";

    int count = getCountFacing(FaceEnum::DOWN, QBTypeEnum::EDGE);
    if (count == 4) // Already a cross
        return;

    if (count == 0) // Dot
    {
        // Applies "F R U R' U' F'" on bottom face
        CubeManager::applyMoves(moves);
    }

    // Either L or Line
    // TODO: Find if L or Line
    // If L, Make sure its in the right place, Then run moves twice
    // If Line, make sure horizontal then run moves once

    std::array<QB*, 2> downEdges;
    int index = 0;
    for (int i = 0; i < bottomEdges.size(); i++)
    {
        QB* current = bottomEdges[i];
        if (current->getFacingSide(FaceEnum::DOWN) == FaceEnum::DOWN)
        {
            downEdges[index++] = current;
        }
    }
    LocalEdgeEnum localEdge1 = CubeManager::getLocalEdge(downEdges[0]->index, FaceEnum::DOWN);
    LocalEdgeEnum localEdge2 = CubeManager::getLocalEdge(downEdges[1]->index, FaceEnum::DOWN);

    if ((static_cast<int>(localEdge1) + static_cast<int>(localEdge2)) % 2 == 0) // Line
    {
        if (localEdge1 == LocalEdgeEnum::TOP || localEdge1 == LocalEdgeEnum::BOTTOM)
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::NORMAL });

        // Line is now in the correct direction
    }
    else
    {
        if ((localEdge1 == LocalEdgeEnum::RIGHT && localEdge2 == LocalEdgeEnum::BOTTOM) ||
            (localEdge1 == LocalEdgeEnum::BOTTOM && localEdge2 == LocalEdgeEnum::RIGHT ))
        {
            // L is in right place
        }
        else if (localEdge1 == LocalEdgeEnum::RIGHT || localEdge2 == LocalEdgeEnum::RIGHT)
        {
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::PRIME });
        }
        else if (localEdge1 == LocalEdgeEnum::BOTTOM || localEdge2 == LocalEdgeEnum::BOTTOM)
        {
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::PRIME });
        }
        else
        {
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::TWICE });
        }

        CubeManager::applyMoves(moves);
        // L has become a line in the correct orientation
    }

    // Line in correct orientation
    CubeManager::applyMoves(moves);
}

void Solver::insertEdge(FaceEnum currentFace, LocalEdgeEnum targetEdge)
{
    int currentFaceInt = convertFaceToInt(currentFace);
    switch (targetEdge)
    {
    case LocalEdgeEnum::LEFT:
        {
            FaceEnum leftFace = convertIntToFace(positiveMod(currentFaceInt - 1, 4));

            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::NORMAL });
            CubeManager::doMove({ leftFace, RotationEnum::NORMAL });
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::PRIME });
            CubeManager::doMove({ leftFace, RotationEnum::PRIME});
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::PRIME });
            CubeManager::doMove({ currentFace, RotationEnum::PRIME });
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::NORMAL });
            CubeManager::doMove({ currentFace, RotationEnum::NORMAL });

            break;
        }
    case LocalEdgeEnum::RIGHT:
        {
            FaceEnum rightFace = convertIntToFace(positiveMod(currentFaceInt + 1, 4));

            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::PRIME });
            CubeManager::doMove({ rightFace, RotationEnum::PRIME });
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::NORMAL });
            CubeManager::doMove({ rightFace, RotationEnum::NORMAL});
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::NORMAL });
            CubeManager::doMove({ currentFace, RotationEnum::NORMAL });
            CubeManager::doMove({ FaceEnum::DOWN, RotationEnum::PRIME });
            CubeManager::doMove({ currentFace, RotationEnum::PRIME });
            break;
        }

    default:
        assert(false && "Not posssible");
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

std::vector<QB*> Solver::findNotQB(FaceEnum face, QBTypeEnum faceType)
{
    std::vector<QB*> edges;
    for (int z = 0; z < s_SizeZ; z++)
    {
        for (int y = 0; y < s_SizeY; y++)
        {
            for (int x = 0; x < s_SizeX; x++)
            {
                size_t count = s_Cubies[z][y][x]->getFaceCount();
                if (count == static_cast<int>(faceType) && !s_Cubies[z][y][x]->hasFace(face))
                {
                    edges.push_back(s_Cubies[z][y][x]);
                }
            }
        }
    }

    return edges;
}

int Solver::getCountFacing(FaceEnum face, QBTypeEnum faceType)
{
    int count = 0;
    for (int z = 0; z < s_SizeZ; z++)
    {
        for (int y = 0; y < s_SizeY; y++)
        {
            for (int x = 0; x < s_SizeX; x++)
            {
                if (s_Cubies[z][y][x]->hasFace(face) && s_Cubies[z][y][x]->getFaceCount() == static_cast<int>(faceType))
                {
                    if (s_Cubies[z][y][x]->getFacingSide(face) == face)
                    {
                        count++;
                    }
                }
            }
        }
    }
    return count;
}

int Solver::convertFaceToInt(FaceEnum face)
{
    return (int)std::log2(static_cast<int>(face));
}

FaceEnum Solver::convertIntToFace(int faceInt)
{
    return static_cast<FaceEnum>(std::pow(2, faceInt));
}

std::array<FaceEnum, 2> Solver::convertDualFaceToFaces(FaceEnum corner)
{
    std::array<FaceEnum, 2> faces;
    FaceEnum found = FaceEnum::UP;
    if ((corner & FaceEnum::UP) == FaceEnum::UP)
    {
        found = FaceEnum::UP;
    }
    else if ((corner & FaceEnum::DOWN) == FaceEnum::DOWN)
    {
        found = FaceEnum::DOWN;
    }
    else if ((corner & FaceEnum::LEFT) == FaceEnum::LEFT)
    {
        found = FaceEnum::LEFT;
    }
    else if ((corner & FaceEnum::RIGHT) == FaceEnum::RIGHT)
    {
        found = FaceEnum::RIGHT;
    }
    else if ((corner & FaceEnum::FRONT) == FaceEnum::FRONT)
    {
        found = FaceEnum::FRONT;
    }
    else if ((corner & FaceEnum::BACK) == FaceEnum::BACK)
    {
        found = FaceEnum::BACK;
    }

    faces[0] = found;
    faces[1] = static_cast<FaceEnum>(static_cast<int>(corner) & ~static_cast<int>(found));

    return faces;
}
