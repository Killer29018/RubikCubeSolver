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

int main()
{
    GLFWwindow* window;
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);    

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Cubes", NULL, NULL);

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
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


    // camera.setPosition(glm::vec3(3.0f, 3.0f, 3.0f));
    // camera.processMouseMovement(-5.0f, -100.0f);
    camera.setCameraPerspective(KRE::CameraPerspective::PERSPECTIVE);
    camera.setCameraMovement(KRE::CameraMovementTypes::FREE_FLY);


    KRE::Shader shader;
    shader.compilePath("res/shaders/basic.vert", "res/shaders/basic.frag");


    glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
    // std::cout << "PROJECTION: \n" << glm::to_string(projection) << "\n";

    // glm::mat4 mat(1.0f);
    // mat = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    // std::cout << glm::to_string(mat) << "\n";
    
     // glm::mat4 projection(1.0f);

    shader.bind();
    shader.setUniformMatrix4("u_Projection", projection);

    Face::generateData();

    // QB cube;
    // cube.addFace(FaceFlag::Top);
    // cube.addFace(FaceFlag::Bottom);
    // cube.addFace(FaceFlag::Front);
    // cube.addFace(FaceFlag::Back);
    // cube.addFace(FaceFlag::Left);
    // cube.addFace(FaceFlag::Right);
    CubeManager::generate(3, 3, 3);
    // CubeManager::rotate(FaceFlag::Front, RotationType::NORMAL);
    // CubeManager::rotate(FaceFlag::Top, RotationType::NORMAL);
    // CubeManager::rotate(FaceFlag::Back, RotationType::NORMAL);

    while (!glfwWindowShouldClose(window))
    {
        KRE::Clock::tick();
        glfwPollEvents();
        
        processKeys(camera);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // VAO.bind();
        shader.bind();

        shader.setUniformMatrix4("u_View", camera.getViewMatrix());

        // glDrawElements(GL_TRIANGLES, indices.getCount(), GL_UNSIGNED_INT, NULL);
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

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        CubeManager::rotate(FaceFlag::Right, RotationType::NORMAL);
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        CubeManager::rotate(FaceFlag::Left, RotationType::NORMAL);
    if (key == GLFW_KEY_U && action == GLFW_PRESS)
        CubeManager::rotate(FaceFlag::Top, RotationType::NORMAL);
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        CubeManager::rotate(FaceFlag::Bottom, RotationType::NORMAL);
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        CubeManager::rotate(FaceFlag::Front, RotationType::NORMAL);
    if (key == GLFW_KEY_B && action == GLFW_PRESS)
        CubeManager::rotate(FaceFlag::Back, RotationType::NORMAL);
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
