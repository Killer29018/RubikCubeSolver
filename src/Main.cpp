#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "KRE/KRE.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Camera.hpp"
#include "CubeManager.hpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

static bool firstMouse = true;
static bool mouseMove = false;
static float lastX = 0.0f;
static float lastY = 0.0f;

static Camera camera({ SCREEN_WIDTH, SCREEN_HEIGHT });

void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

int main()
{
    GLFWwindow* window;
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rubik Cube", NULL, NULL);

    if (!window)
    {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    int version = gladLoadGL(glfwGetProcAddress);
    if (!version)
    {
        std::cerr << "Failed to initialize glad\n";
        return -1;
    }

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << "\n";

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    camera.setAngle(45.0f, 45.0f);

    KRE::Shader shader;
    shader.compilePath("res/shaders/basic.vert", "res/shaders/basic.frag");

    shader.bind();
    shader.setUniformMatrix4("u_Projection", camera.getProjectionMatrix());

    CubeManager::generate(3, 3, 3);
    Move::seconds = 0.05f;
    // Move::seconds = 1.00f;

    while (!glfwWindowShouldClose(window))
    {
        KRE::Clock::tick();
        glfwPollEvents();
        CubeManager::update(KRE::Clock::deltaTime);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.bind();
        shader.setUniformMatrix4("u_View", camera.getViewMatrix());

        CubeManager::draw(shader);

        glfwSwapBuffers(window);
    }

    CubeManager::destroy();

    glfwTerminate();
    return 0;
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (!mouseMove)
        return;

    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.processAngleChange(xOffset, yOffset);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        return;
    }

    RotationEnum rotation = RotationEnum::NORMAL;

    if (mods == GLFW_MOD_SHIFT)
        rotation = RotationEnum::PRIME;

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        CubeManager::applyMoves("L' D' L D' L' D2 L D' D2 L' D' L D' L' D2 L D'");
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        CubeManager::applyMoves("R' U2 B L' F U' B D F U D' L D2 F' R B' D F' U' B' U D'");
    if (key == GLFW_KEY_V && action == GLFW_PRESS)
        Solver::solve();
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        CubeManager::doMove({ FaceEnum::RIGHT, rotation });
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        CubeManager::doMove({ FaceEnum::LEFT, rotation });
    if (key == GLFW_KEY_U && action == GLFW_PRESS)
        CubeManager::doMove({ FaceEnum::UP, rotation });
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        CubeManager::doMove({ FaceEnum::DOWN, rotation });
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        CubeManager::doMove({ FaceEnum::FRONT, rotation });
    if (key == GLFW_KEY_B && action == GLFW_PRESS)
        CubeManager::doMove({ FaceEnum::BACK, rotation });
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        if (!mouseMove)
        {
            mouseMove = true;
            firstMouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        if (mouseMove)
        {
            mouseMove = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.processScrollWheel(-yOffset);
}
