#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <cstdint>

enum class RotationEnum
{
    PRIME   = -1,
    NONE    =  0,
    NORMAL  =  1,
    TWICE   =  2,
};

enum class FaceIndex : uint16_t
{
    FRONT   = 0,
    RIGHT   = 1,
    BACK    = 2,
    LEFT    = 3,
    UP      = 4,
    DOWN    = 5,
};

enum class FaceEnum : uint8_t
{
    FRONT   = 1 << 0, // 1
    RIGHT   = 1 << 1, // 2
    BACK    = 1 << 2, // 4
    LEFT    = 1 << 3, // 8
    UP      = 1 << 4, // 16
    DOWN    = 1 << 5, // 32
};

enum class QBTypeEnum : uint8_t
{
    CENTER  = 1,
    EDGE    = 2,
    CORNER  = 3,
};

enum class LocalEdgeEnum : uint8_t
{
    LEFT    = 0,
    TOP     = 1,
    RIGHT   = 2,
    BOTTOM  = 3,
};

enum class LocalCornerEnum : uint8_t
{
    TOP_LEFT      = 0,
    TOP_RIGHT     = 1,
    BOTTOM_RIGHT  = 2,
    BOTTOM_LEFT   = 3,
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
    inline static float seconds = 1.0f;
    FaceEnum face;
    RotationEnum rotation;

    float time = 0.0f;

    Move() = default;
    Move(FaceEnum face) : face(face), rotation(RotationEnum::NONE) {}
    Move(FaceEnum face, RotationEnum rotation) : face(face), rotation(rotation) {}
};

#endif
