#include "Application.hpp"

#include <iostream>

Application::Application(std::string title, glm::ivec2 windowSize)
{
    m_Title = title;
    this->windowSize = windowSize;

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


        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        constexpr GLenum bufs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

        glDrawBuffers(2, bufs);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        glClearColor(-1.0f, -1.0f, -1.0f, -1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawBuffers(2, bufs);

        m_CubeShader.bind();
        m_CubeShader.setUniformMatrix4("u_View", camera.getViewMatrix());

        CubeManager::draw(m_CubeShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_ScreenShader.bind();
        glBindVertexArray(m_ScreenVAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_ScreenTexture);
        // glBindTexture(GL_TEXTURE_2D, m_PickingTexture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(m_Window);

        if (mousePicked && !m_PreviousMousePicked)
        {
            // CubeManager::startMousePick(m_FBO, { mousePosition.x, windowSize.y - mousePosition.y });
            MousePicker::startPicking(m_FBO, { mousePosition.x, windowSize.y - mousePosition.y });
        }
        else if (!mousePicked && m_PreviousMousePicked)
        {
            MousePicker::endPicking();
        }

        m_PreviousMousePicked = mousePicked;
    }
}

void Application::init()
{
    camera = Camera(windowSize);

    mousePosition = glm::vec2(0.0f);

    setupGLFW();
    setupOpenGL();
    setupFramebuffer();
    setupScreenVAO();

    Face::initialize();

    camera.setAngle(45.0f, 45.0f);
    camera.distance = 3.0f;

    m_CubeShader.compilePath("res/shaders/cube.vert", "res/shaders/cube.frag");

    m_ScreenShader.compilePath("res/shaders/screen.vert", "res/shaders/screen.frag");

    m_CubeShader.bind();
    m_CubeShader.setUniformMatrix4("u_Projection", camera.getProjectionMatrix());

    m_ScreenShader.bind();
    m_ScreenShader.setUniformInt("u_ScreenTexture", 0);

    CubeManager::generate(3);
    Move::seconds = 0.1f;

    MousePicker::init(&camera, CubeManager::getCubies(), 3);
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

    m_Window = glfwCreateWindow(windowSize.x, windowSize.y, m_Title.c_str(), NULL, NULL);

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

    glViewport(0, 0, windowSize.x, windowSize.y);
}

void Application::setupScreenVAO()
{
    float vertices[] = {
        // Position     Tex Coords
        -1.0f, -1.0f,   0.0f, 0.0f,
        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,

        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f
    };

    uint32_t vbo;

    glGenVertexArrays(1, &m_ScreenVAO);
    glBindVertexArray(m_ScreenVAO);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Application::setupFramebuffer()
{
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    glGenTextures(1, &m_ScreenTexture);
    glBindTexture(GL_TEXTURE_2D, m_ScreenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSize.x, windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &m_PickingTexture);
    glBindTexture(GL_TEXTURE_2D, m_PickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowSize.x, windowSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowSize.x, windowSize.y);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ScreenTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_PickingTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::Framebuffer is incomplete\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);

    static float lastX = 0.0f;
    static float lastY = 0.0f;

    app->mousePosition = glm::vec2(xPos, yPos);

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

    glm::vec4 mouseOffset(xOffset, yOffset, 0.0f, 1.0f);

    MousePicker::mouseMoved({ mouseOffset.x, mouseOffset.y });

    if (!app->mouseMove)
        return;

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
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        if (app->mouseMove)
        {
            app->mouseMove = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        app->mousePicked = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        app->mousePicked = false;
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
        // CubeManager::applyMoves("M2 E2 S2");
        CubeManager::applyMoves("r2 R2 u2 U2 b2 B2");
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
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        CubeManager::doMove({ FaceEnum::LEFT, rotation, 1 });
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        CubeManager::doMove({ FaceEnum::DOWN, rotation, 1 });
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        CubeManager::doMove({ FaceEnum::FRONT, rotation, 1 });
}

void Application::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);

    app->camera.processScrollWheel(-yOffset);
}

