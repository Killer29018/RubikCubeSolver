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
    const uint16_t size = 2;

    bool firstMouse = true;
    bool mouseMove = false;
    
    bool mousePicked = false;

    glm::vec2 mousePosition;
    glm::ivec2 windowSize;

    Camera camera;
private:
    GLFWwindow* m_Window;

    std::string m_Title;

    KRE::Shader m_CubeShader;
    KRE::Shader m_ScreenShader;

    uint32_t m_FBO;
    uint32_t m_ScreenTexture;
    uint32_t m_PickingTexture;
    uint32_t m_RBO;

    uint32_t m_ScreenVAO;

    bool m_PreviousMousePicked = false;
public:
    Application(std::string title, glm::ivec2 windowSize);
    ~Application();

    void run();

private:
    void init();

    void setupGLFW();
    void setupOpenGL();

    void setupFramebuffer();
    void setupScreenVAO();

    static void mouseCallback(GLFWwindow* window, double xPos, double yPos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
};

#endif
