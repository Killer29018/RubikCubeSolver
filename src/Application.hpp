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

#include "imgui/WindowManager.hpp"
#include "imgui/SettingsWindow.hpp"

class Application
{
public:
    static bool firstMouse;
    static bool mouseMove;
    
    static bool mousePicked;

    static glm::vec2 mousePosition;
    static glm::ivec2 windowSize;

    static Camera camera;
private:
    static GLFWwindow* s_Window;

    static std::string s_Title;

    static uint16_t s_CubeSize;

    static KRE::Shader s_CubeShader;
    static KRE::Shader s_ScreenShader;

    static uint32_t s_FBO;
    static uint32_t s_ScreenTexture;
    static uint32_t s_PickingTexture;
    static uint32_t s_RBO;

    static uint32_t s_ScreenVAO;

    static bool s_PreviousMousePicked;

    static SettingsWindow s_SettingsWindow;
public:
    ~Application();

    static void init(std::string title, glm::ivec2 windowSize);

    static void run();

    static void changeSize(uint16_t newSize);
private:
    Application() {}

    static void setupGLFW();
    static void setupOpenGL();

    static void setupFramebuffer();
    static void setupScreenVAO();

    static void mouseCallback(GLFWwindow* window, double xPos, double yPos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
};

#endif
