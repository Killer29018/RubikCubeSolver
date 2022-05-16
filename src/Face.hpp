#ifndef FACE_HPP
#define FACE_HPP

#include "KRE/KRE.hpp"
#include "Colours.hpp"
#include "Enums.hpp"

#include <cstdint>

class Face
{
public:
    glm::ivec3 facing;
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
