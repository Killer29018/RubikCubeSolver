#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <cstdint>

enum class RotationEnum
{
    NONE    = 0,
    NORMAL  = 1,
    TWICE   = 2,
    PRIME   = 3,
};

enum class FaceEnum : uint8_t
{
    UP      = 1 << 0,
    DOWN    = 1 << 1,
    FRONT   = 1 << 2,
    BACK    = 1 << 3,
    LEFT    = 1 << 4,
    RIGHT   = 1 << 5,
};

namespace std
{
    template<>
    struct hash<FaceEnum>
    {
        size_t operator()(const FaceEnum& f) const
        {
            return hash<int>()(static_cast<uint32_t>(f));
        }
    };
}

inline FaceEnum operator|(const FaceEnum& lhs, const FaceEnum& rhs)
{
    return static_cast<FaceEnum>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

inline FaceEnum operator&(const FaceEnum& lhs, const FaceEnum& rhs)
{
    return static_cast<FaceEnum>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

inline FaceEnum& operator|=(FaceEnum& lhs, const FaceEnum& rhs)
{
    return lhs = lhs | rhs;
}

struct Move
{
    FaceEnum face;
    RotationEnum rotation;

    float time = 1.0f;

    Move() = default;
    Move(FaceEnum face) : face(face), rotation(RotationEnum::NONE) {}
    Move(FaceEnum face, RotationEnum rotation) : face(face), rotation(rotation) {}
};

#endif
