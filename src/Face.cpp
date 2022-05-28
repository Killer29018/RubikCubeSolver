#include "Face.hpp"

#include <glad/gl.h>
#include <iostream>

uint32_t Face::s_VAO[6];
uint32_t Face::s_EBO;

void Face::initialize()
{
    setupVAOs();
}

void Face::draw(KRE::Shader& shader, FaceEnum activeFaces, glm::mat4 model)
{
    shader.bind();

    shader.setUniformMatrix4("u_Model", model);

    glUniform4fv(glGetUniformLocation(shader.ID, "u_Colours"), 7, glm::value_ptr(faceColours[0]));

    for (int i = 0; i < 6; i++)
    {
        FaceEnum face = static_cast<FaceEnum>(std::pow(2, i));

        glBindVertexArray(s_VAO[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);
        int colour = 6;

        if ((activeFaces & face) == face)
        {
            colour = i;
        }

        shader.setUniformInt("u_CurrentColour", colour);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    }
}

glm::ivec3 Face::getFacing(FaceEnum facing)
{
    switch (facing)
    {
    case FaceEnum::FRONT:
        return glm::ivec3(0, 0, 1);
    case FaceEnum::BACK:
        return glm::vec3(0, 0, -1);
    case FaceEnum::LEFT:
        return glm::vec3(-1, 0, 0);
    case FaceEnum::RIGHT:
        return glm::vec3(1, 0, 0);
    case FaceEnum::UP:
        return glm::vec3(0, 1, 0);
    case FaceEnum::DOWN:
        return glm::vec3(0, -1, 0);
    }
}

void Face::setupVAOs()
{
    uint32_t indices[] = {
        0, 1, 2,
        1, 2, 3
    };

    glGenBuffers(1, &s_EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6, indices, GL_STATIC_DRAW);

    glGenVertexArrays(6, &s_VAO[0]);

    float upFace[] = {
        // Position
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
    };

    float downFace[] = {
        // Position
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
    };

    float frontFace[] = {
        // Position
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
    };

    float backFace[] ={
        // Position
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
    };

    float leftFace[] = {
        // Position
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
    };

    float rightFace[] = {
        // Position
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
    };

    setupVAO(s_VAO[static_cast<int>(FaceIndex::UP)], upFace, sizeof(upFace));
    setupVAO(s_VAO[static_cast<int>(FaceIndex::DOWN)], downFace, sizeof(downFace));
    setupVAO(s_VAO[static_cast<int>(FaceIndex::FRONT)], frontFace, sizeof(frontFace));
    setupVAO(s_VAO[static_cast<int>(FaceIndex::BACK)], backFace, sizeof(backFace));
    setupVAO(s_VAO[static_cast<int>(FaceIndex::LEFT)], leftFace, sizeof(leftFace));
    setupVAO(s_VAO[static_cast<int>(FaceIndex::RIGHT)], rightFace, sizeof(rightFace));
}

void Face::setupVAO(uint32_t vao, float* vertices, int size)
{
    uint32_t vbo;
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);

    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// uint32_t Face::s_EBO;
// const uint32_t Face::s_Indices[] = {
//     0, 1, 2,
//     1, 2, 3
// };

// void Face::generateData()
// {
//     glGenBuffers(1, &s_EBO);

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6, s_Indices, GL_STATIC_DRAW);
// }

// void Face::draw(KRE::Shader& shader)
// {
//     shader.setUniformVector3("u_Colour", faceColours[static_cast<uint32_t>(faceColour)]);
//     glBindVertexArray(m_VAO);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);
//     glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
// }

// void Face::uploadData(uint32_t& vbo, const float* vertexData, int size)
// {
//     glGenVertexArrays(1, &m_VAO);
//     glGenBuffers(1, &vbo);

//     glBindVertexArray(m_VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);

//     glBufferData(GL_ARRAY_BUFFER, size, vertexData, GL_STATIC_DRAW);

//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);

//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindVertexArray(0);
// }

// void TopFace::generateFace()
// {
//     const float vertices[] = {
//         // Position
//         -0.5f,  0.5f, -0.5f,
//         -0.5f,  0.5f,  0.5f,
//          0.5f,  0.5f, -0.5f,
//          0.5f,  0.5f,  0.5f,
//     };

//     facing = glm::ivec3(0, 1, 0);
//     uint32_t vbo;
//     uploadData(vbo, vertices, sizeof(vertices));
// }

// void BottomFace::generateFace()
// {
//     const float vertices[] = {
//         // Position
//         -0.5f, -0.5f, -0.5f,
//         -0.5f, -0.5f,  0.5f,
//          0.5f, -0.5f, -0.5f,
//          0.5f, -0.5f,  0.5f,
//     };

//     facing = glm::ivec3(0, -1, 0);
//     uint32_t vbo;
//     uploadData(vbo, vertices, sizeof(vertices));
// }

// void FrontFace::generateFace()
// {
//     const float vertices[] = {
//         // Position
//         -0.5f, -0.5f,  0.5f,
//         -0.5f,  0.5f,  0.5f,
//          0.5f, -0.5f,  0.5f,
//          0.5f,  0.5f,  0.5f,
//     };

//     facing = glm::ivec3(0, 0, 1);
//     uint32_t vbo;
//     uploadData(vbo, vertices, sizeof(vertices));
// }

// void BackFace::generateFace()
// {
//     const float vertices[] = {
//         // Position
//         -0.5f, -0.5f, -0.5f,
//         -0.5f,  0.5f, -0.5f,
//          0.5f, -0.5f, -0.5f,
//          0.5f,  0.5f, -0.5f,
//     };

//     facing = glm::ivec3(0, 0, -1);
//     uint32_t vbo;
//     uploadData(vbo, vertices, sizeof(vertices));
// }

// void LeftFace::generateFace()
// {
//     const float vertices[] = {
//         // Position
//         -0.5f, -0.5f, -0.5f,
//         -0.5f, -0.5f,  0.5f,
//         -0.5f,  0.5f, -0.5f,
//         -0.5f,  0.5f,  0.5f,
//     };

//     facing = glm::ivec3(-1, 0, 0);
//     uint32_t vbo;
//     uploadData(vbo, vertices, sizeof(vertices));
// }

// void RightFace::generateFace()
// {
//     const float vertices[] = {
//         // Position
//          0.5f, -0.5f, -0.5f,
//          0.5f, -0.5f,  0.5f,
//          0.5f,  0.5f, -0.5f,
//          0.5f,  0.5f,  0.5f,
//     };

//     facing = glm::ivec3(1, 0, 0);
//     uint32_t vbo;
//     uploadData(vbo, vertices, sizeof(vertices));
// }
