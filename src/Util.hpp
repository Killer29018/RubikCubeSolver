#ifndef UTIL_HPP
#define UTIL_HPP

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
    }

    return FaceEnum::NONE;
}

inline Move oppositeMove(const Move& move)
{
    Move returnMove;

    returnMove.face = oppositeFace(move.face);
    returnMove.slice = move.slice;
    returnMove.sliceMode = move.sliceMode;

    return returnMove;
}

#endif
