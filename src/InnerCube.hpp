#ifndef INNER_CUBE_HPP
#define INNER_CUBE_HPP

#include <glad/gl.h>
#include <cstdint>

#include <glm/glm.hpp>

#include "KRE/KRE.hpp"

class InnerCube
{
public:
private:
    static uint32_t s_VAO, s_EBO;

public:
    static void generate();

    static void draw(KRE::Shader& shader, glm::mat4 model);
private:
    InnerCube() {}
};

#endif
