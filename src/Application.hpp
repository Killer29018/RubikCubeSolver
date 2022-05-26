#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include "KRE/KRE.hpp"

#include <cstdint>

#include "Camera.hpp"
#include "CubeManager.hpp"
#include "Solver.hpp"

class Application
{
public:
    bool firstMouse = true;
    bool mouseMove = false;

    Camera camera;
private:
    GLFWwindow* m_Window;

    std::string m_Title;
    glm::ivec2 m_WindowSize;


    KRE::Shader m_CubeShader;
public:
    Application(std::string title, glm::ivec2 windowSize);
    ~Application();

    void run();

private:
    void init();

    void setupGLFW();
    void setupOpenGL();

    static void mouseCallback(GLFWwindow* window, double xPos, double yPos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
};

#endif
