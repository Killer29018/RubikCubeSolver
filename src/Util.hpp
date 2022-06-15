#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <vector>

inline int positiveMod(int dividend, int divisor)
{
    return (dividend % divisor + divisor) % divisor;
}

inline RotationEnum reverseRotation(const RotationEnum& rotation)
{
    switch (rotation)
    {
    case RotationEnum::NONE:
    case RotationEnum::TWICE:
        return rotation;

    case RotationEnum::PRIME:
        return RotationEnum::NORMAL;
    case RotationEnum::NORMAL:
        return RotationEnum::PRIME;
    }

    return RotationEnum::NONE;
}

inline FaceEnum oppositeFace(const FaceEnum& face)
{
    switch (face)
    {
    case FaceEnum::FRONT:
        return FaceEnum::BACK;
    case FaceEnum::RIGHT:
        return FaceEnum::LEFT;
    case FaceEnum::BACK:
        return FaceEnum::FRONT;
    case FaceEnum::LEFT:
        return FaceEnum::RIGHT;
    case FaceEnum::UP:
        return FaceEnum::DOWN;
    case FaceEnum::DOWN:
        return FaceEnum::UP;

    default:
        return FaceEnum::NONE;
    }
}

inline Move oppositeMove(const Move& move)
{
    Move returnMove;

    returnMove.face = oppositeFace(move.face);
    returnMove.slice = move.slice;
    returnMove.sliceMode = move.sliceMode;

    return returnMove;
}

inline std::string convertMoveToString(const Move& move)
{
    std::string body = "";
    std::string postFix = "";

    switch (move.rotation)
    {
    case RotationEnum::PRIME: 
        postFix = '\''; break;
    case RotationEnum::TWICE: 
        postFix = '2'; break;

    default:
        postFix = ""; break;
    }

    if (move.sliceMode == SliceMode::SINGLE)
    {
        if (move.slice == 0)
        {
            switch (move.face)
            {
            case FaceEnum::FRONT: 
                body = "F"; break;
            case FaceEnum::RIGHT: 
                body = "R"; break;
            case FaceEnum::BACK: 
                body = "B"; break;
            case FaceEnum::LEFT: 
                body = "L"; break;
            case FaceEnum::UP: 
                body = "U"; break;
            case FaceEnum::DOWN: 
                body = "D"; break;

            default:
                 break;
            }
        }

        if (move.slice == 1)
        {
            switch (move.face)
            {
            case FaceEnum::FRONT:
                body = "S"; break;
            case FaceEnum::LEFT:
                body = "M"; break;
            case FaceEnum::DOWN:
                body = "E"; break;

            default: 
                break;
            }
        }
    }
    else
    {
        switch (move.face)
        {
        case FaceEnum::FRONT:
            body = "f"; break;
        case FaceEnum::RIGHT:
            body = "r"; break;
        case FaceEnum::BACK:
            body = "b"; break;
        case FaceEnum::LEFT:
            body = "l"; break;
        case FaceEnum::UP:
            body = "u"; break;
        case FaceEnum::DOWN:
            body = "d"; break;

        default:
            break;
        }
    }

    return body + postFix;
}

inline std::string convertMovesToString(const std::vector<Move>& moves)
{
    std::string output = "";
    for (Move m : moves)
    {
        output += convertMoveToString(m) + " ";
    }
    
    return output;
}

#endif
