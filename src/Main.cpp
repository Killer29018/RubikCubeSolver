#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "KRE/KRE.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "CubeManager.hpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
static KRE::Camera camera(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
static bool firstMouse = true;
static float lastX = 0.0f;
static float lastY = 0.0f;

void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void processKeys(KRE::Camera& camera);

// TODO: Add two seperate arrays for CubeManager. One for animating, One for data

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

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rubix Cube", NULL, NULL);

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
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

    camera.setPosition({ 0.0f, 2.0f, 5.0f });
    camera.setCameraPerspective(KRE::CameraPerspective::PERSPECTIVE);
    camera.setCameraMovement(KRE::CameraMovementTypes::FREE_FLY);

    KRE::Shader shader;
    shader.compilePath("res/shaders/basic.vert", "res/shaders/basic.frag");

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    shader.bind();
    shader.setUniformMatrix4("u_Projection", projection);

    CubeManager::generate(3, 3, 3);
    Move::seconds = 0.2f;
    // CubeManager::scramble("L2 B2 U2 B' L2 F2 L2 D2 B' R2 B R2 D' F' U' R2 D2 L B R D2");
    // CubeManager::scramble("U B D' F2 D B' U' R2 D F2 D' R2 D F2 D' R2");
    // CubeManager::scramble("U D' R L' F B' U D'");

    while (!glfwWindowShouldClose(window))
    {
        KRE::Clock::tick();
        glfwPollEvents();
        CubeManager::update(KRE::Clock::deltaTime);
        
        processKeys(camera);

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

    camera.processMouseMovement(xOffset, yOffset);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        return;
    }

    if (action == GLFW_PRESS)
        KRE::Keyboard::pressKey(key);
    else if (action == GLFW_RELEASE)
        KRE::Keyboard::unpressKey(key);

    RotationEnum rotation = RotationEnum::NORMAL;

    if (mods == GLFW_MOD_SHIFT)
        rotation = RotationEnum::PRIME;

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        CubeManager::scramble("R2 L2 F2 B2 D2 U2");
    if (key == GLFW_KEY_V && action == GLFW_PRESS)
        Solver::solve();
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        CubeManager::addMove({ FaceEnum::RIGHT, rotation });
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        CubeManager::addMove({ FaceEnum::LEFT, rotation });
    if (key == GLFW_KEY_U && action == GLFW_PRESS)
        CubeManager::addMove({ FaceEnum::UP, rotation });
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        CubeManager::addMove({ FaceEnum::DOWN, rotation });
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        CubeManager::addMove({ FaceEnum::FRONT, rotation });
    if (key == GLFW_KEY_B && action == GLFW_PRESS)
        CubeManager::addMove({ FaceEnum::BACK, rotation });
}

void processKeys(KRE::Camera& camera)
{
    using namespace KRE;

    if (Keyboard::getKey(GLFW_KEY_W))
        camera.processKeyboard(CameraMovement::FORWARD, Clock::deltaTime);
    if (Keyboard::getKey(GLFW_KEY_A))
        camera.processKeyboard(CameraMovement::LEFT, Clock::deltaTime);
    if (Keyboard::getKey(GLFW_KEY_S))
        camera.processKeyboard(CameraMovement::BACK, Clock::deltaTime);
    if (Keyboard::getKey(GLFW_KEY_D))
        camera.processKeyboard(CameraMovement::RIGHT, Clock::deltaTime);
    if (Keyboard::getKey(GLFW_KEY_SPACE))
        camera.processKeyboard(CameraMovement::UP, Clock::deltaTime);
    if (Keyboard::getKey(GLFW_KEY_LEFT_CONTROL))
        camera.processKeyboard(CameraMovement::DOWN, Clock::deltaTime);

    if (Keyboard::getKey(GLFW_KEY_LEFT_SHIFT) && !camera.fastMovement)
        camera.fastMovement = true;
    else if (camera.fastMovement && !Keyboard::getKey(GLFW_KEY_LEFT_SHIFT))
        camera.fastMovement = false;
}
