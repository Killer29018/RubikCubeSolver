#include "Application.hpp"

#include <iostream>

Application::Application(std::string title, glm::ivec2 windowSize)
{
    m_Title = title;
    m_WindowSize = windowSize;

    init();
}

Application::~Application()
{
    CubeManager::destroy();
    glfwTerminate();
}

void Application::run()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        KRE::Clock::tick();

        glfwPollEvents();

        CubeManager::update(KRE::Clock::deltaTime);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_CubeShader.bind();
        m_CubeShader.setUniformMatrix4("u_View", camera.getViewMatrix());

        CubeManager::draw(m_CubeShader);

        glfwSwapBuffers(m_Window);
    }
}

void Application::init()
{
    camera = Camera(m_WindowSize);

    setupGLFW();
    setupOpenGL();

    Face::initialize();

    camera.setAngle(45.0f, 45.0f);
    camera.distance = 3.0f;

    m_CubeShader.compilePath("res/shaders/basic.vert", "res/shaders/basic.frag");

    m_CubeShader.bind();
    m_CubeShader.setUniformMatrix4("u_Projection", camera.getProjectionMatrix());

    CubeManager::generate(3, 3, 3);
    Move::seconds = 1.0f;
}

void Application::setupGLFW()
{
    if (!glfwInit())
        exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    m_Window = glfwCreateWindow(m_WindowSize.x, m_WindowSize.y, m_Title.c_str(), NULL, NULL);

    if (!m_Window)
    {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(m_Window);

    glfwSetWindowUserPointer(m_Window, (void*)this);

    glfwSetCursorPosCallback(m_Window, Application::mouseCallback);
    glfwSetKeyCallback(m_Window, Application::keyCallback);
    glfwSetMouseButtonCallback(m_Window, Application::mouseButtonCallback);
    glfwSetScrollCallback(m_Window, Application::scrollCallback);

    glfwSwapInterval(1);
}

void Application::setupOpenGL()
{
    int version = gladLoadGL(glfwGetProcAddress);

    if (!version)
    {
        std::cerr << "Failed to initialize GLAD\n";
        exit(-1);
    }

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << "\n";

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glViewport(0, 0, m_WindowSize.x, m_WindowSize.y);
}

void Application::mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    static float lastX = 0.0f;
    static float lastY = 0.0f;

    Application* app = (Application*)glfwGetWindowUserPointer(window);

    if (!app->mouseMove)
        return;

    if (app->firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        app->firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    app->camera.processAngleChange(xOffset, yOffset);
}

void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        if (!app->mouseMove)
        {
            app->mouseMove = true;
            app->firstMouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        if (app->mouseMove)
        {
            app->mouseMove = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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
        CubeManager::applyMoves("D L2 R2 U' B2 D L2 F2 L2 U2 B' R F' U R F D2 R F2 R' U'");
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

void Application::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);

    app->camera.processScrollWheel(-yOffset);
}

