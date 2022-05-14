#ifndef FACE_HPP
#define FACE_HPP

#include "KRE/KRE.hpp"
#include "Colours.hpp"

#include <cstdint>

enum class FaceFlag : uint8_t
{
    Top     = 1 << 0,
    Bottom  = 1 << 1,
    Front   = 1 << 2,
    Back    = 1 << 3,
    Left    = 1 << 4,
    Right   = 1 << 5,
};

namespace std
{
    template<>
    struct hash<FaceFlag>
    {
        size_t operator()(const FaceFlag& f) const
        {
            return hash<int>()(static_cast<uint32_t>(f));
        }
    };
}

inline FaceFlag operator|(FaceFlag& lhs, const FaceFlag& rhs)
{
    return static_cast<FaceFlag>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

inline FaceFlag operator&(FaceFlag& lhs, const FaceFlag& rhs)
{
    return static_cast<FaceFlag>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

inline FaceFlag& operator|=(FaceFlag& lhs, const FaceFlag& rhs)
{
    return lhs = lhs | rhs;
}

class Face
{
protected:
    uint32_t m_VAO;
    static uint32_t s_EBO;
    static const uint32_t s_Indices[6];

    ColourIndex faceColour;
public:
    Face() : faceColour(ColourIndex::WHITE) {}
    Face(ColourIndex colour) : faceColour(colour) {}
    ~Face() {}

    static void generateData();

    virtual void generateFace() {};

    void draw(KRE::Shader& shader);
protected:
    void uploadData(uint32_t& vbo, const float* vertexData, int count);
};

class TopFace : public Face
{
public:
    TopFace(bool generate = false) : Face(ColourIndex::WHITE) { if (generate) generateFace(); }
    ~TopFace() {}

    void generateFace() override;
};

class BottomFace : public Face
{
public:
    BottomFace(bool generate = false) : Face(ColourIndex::YELLOW) { if (generate) generateFace(); }
    ~BottomFace() {}

    void generateFace() override;
};

class FrontFace : public Face
{
public:
    FrontFace(bool generate = false) : Face(ColourIndex::GREEN) { if (generate) generateFace(); }
    ~FrontFace() {}

    void generateFace() override;
};

class BackFace : public Face
{
public:
    BackFace(bool generate = false) : Face(ColourIndex::BLUE) { if (generate) generateFace(); }
    ~BackFace() {}

    void generateFace() override;
};

class LeftFace : public Face
{
public:
    LeftFace(bool generate = false) : Face(ColourIndex::ORANGE) { if (generate) generateFace(); }
    ~LeftFace() {}

    void generateFace() override;
};

class RightFace : public Face
{
public:
    RightFace(bool generate = false) : Face(ColourIndex::RED) { if (generate) generateFace(); }
    ~RightFace() {}

    void generateFace() override;
};

#endif
