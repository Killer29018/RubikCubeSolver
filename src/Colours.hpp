#ifndef COLOURS_HPP
#define COLOURS_HPP

#include  <glm/glm.hpp>

enum class ColourIndex
{
    WHITE   = 0,
    YELLOW  = 1,
    GREEN   = 2,
    BLUE    = 3,
    RED     = 4,
    ORANGE  = 5,
};

const glm::vec3 faceColours[] = 
{
    { 1.0f, 1.0f, 1.0f }, // White
    { 0.917f, 0.985f, 0.0f }, // Yellow
    { 0.208f, 0.985f, 0.004f }, // Green
    { 0.0f, 0.173f, 0.985f }, // Blue
    { 0.985f, 0.002f, 0.057f }, // Red
    { 0.985f, 0.487f, 0.038f }, // Orange
};

#endif
