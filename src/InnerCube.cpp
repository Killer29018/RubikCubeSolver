#include "InnerCube.hpp"

uint32_t InnerCube::s_VAO;
uint32_t InnerCube::s_EBO;

void InnerCube::generate()
{
    const float vertices[] = 
    {
        -0.5f, -0.5f, -0.5f, // 0
        -0.5f, -0.5f,  0.5f, // 1
        -0.5f,  0.5f, -0.5f, // 2
        -0.5f,  0.5f,  0.5f, // 3
         0.5f, -0.5f, -0.5f, // 4
         0.5f, -0.5f,  0.5f, // 5
         0.5f,  0.5f, -0.5f, // 6
         0.5f,  0.5f,  0.5f, // 7
    };

    const uint32_t indices[] = 
    {
        // Top
        0, 1, 4, 1, 4, 5,

        // Bottom
        2, 3, 6, 3, 6, 7,

        // Left
        0, 1, 2, 1, 2, 3,

        // Right
        4, 5, 6, 5, 6, 7,

        // Front
        1, 3, 5, 3, 5, 7,

        // Back
        0, 2, 4, 2, 4, 6,
    };

    uint32_t vbo;
    glGenVertexArrays(1, &s_VAO);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &s_EBO);

    glBindVertexArray(s_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void InnerCube::draw(KRE::Shader& shader, glm::mat4 model)
{
    shader.bind();

    model = glm::scale(model, glm::vec3(1.0f));
    shader.setUniformMatrix4("u_Model", model);
    shader.setUniformVector3("u_Colour", { 0.1f, 0.1f, 0.1f });

    glBindVertexArray(s_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);

    glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, NULL);
}
