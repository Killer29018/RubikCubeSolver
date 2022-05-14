#include "Face.hpp"

#include <glad/gl.h>
#include <iostream>

uint32_t Face::s_EBO;
const uint32_t Face::s_Indices[] = {
    0, 1, 2,
    1, 2, 3
};

void Face::generateData()
{
    glGenBuffers(1, &s_EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6, s_Indices, GL_STATIC_DRAW);
}

void Face::draw(KRE::Shader& shader)
{
    shader.setUniformVector3("u_Colour", faceColours[static_cast<uint32_t>(faceColour)]);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Face::uploadData(uint32_t& vbo, const float* vertexData, int size)
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &vbo);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);

    glBufferData(GL_ARRAY_BUFFER, size, vertexData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TopFace::generateFace()
{
    const float vertices[] = {
        // Position         Normals
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
    };

    uint32_t vbo;
    uploadData(vbo, vertices, sizeof(vertices));
}

void BottomFace::generateFace()
{
    const float vertices[] = {
        // Position         Normals
        -0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,
    };

    uint32_t vbo;
    uploadData(vbo, vertices, sizeof(vertices));
}

void FrontFace::generateFace()
{
    const float vertices[] = {
        // Position         Normals
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
    };

    uint32_t vbo;
    uploadData(vbo, vertices, sizeof(vertices));
}

void BackFace::generateFace()
{
    const float vertices[] = {
        // Position         Normals
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
    };

    uint32_t vbo;
    uploadData(vbo, vertices, sizeof(vertices));
}

void LeftFace::generateFace()
{
    const float vertices[] = {
        // Position         Normals
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
    };

    uint32_t vbo;
    uploadData(vbo, vertices, sizeof(vertices));
}

void RightFace::generateFace()
{
    const float vertices[] = {
        // Position         Normals
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
    };

    uint32_t vbo;
    uploadData(vbo, vertices, sizeof(vertices));
}
