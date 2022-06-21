#include "Application.hpp"

#include <iostream>

bool Application::firstMouse = true;
bool Application::mouseMove = false;

bool Application::mousePicked = false;

glm::vec2 Application::mousePosition;
glm::ivec2 Application::windowSize;

Camera Application::camera;
GLFWwindow* Application::s_Window;

std::string Application::s_Title;

uint16_t Application::s_CubeSize = 3;

KRE::Shader Application::s_CubeShader;
KRE::Shader Application::s_ScreenShader;

uint32_t Application::s_FBO;
uint32_t Application::s_ScreenTexture;
uint32_t Application::s_PickingTexture;
uint32_t Application::s_RBO;

uint32_t Application::s_ScreenVAO;

bool Application::s_PreviousMousePicked = false;

CubeManager Application::s_CubeManager;
Solver Application::s_Solver;
MoveManager Application::s_MoveManager;

SettingsWindow Application::s_SettingsWindow;

Application::~Application()
{
    s_CubeManager.destroy();
    glfwTerminate();
}

void Application::init(std::string title, glm::ivec2 windowSize)
{
    s_Title = title;
    Application::windowSize = windowSize;

    camera = Camera(windowSize);

    mousePosition = glm::vec2(0.0f);

    setupGLFW();
    setupOpenGL();
    setupFramebuffer();
    setupScreenVAO();


    Face::initialize();

    camera.setAngle(45.0f, 45.0f);
    camera.distance = 3.0f;

    s_CubeShader.compilePath("res/shaders/cube.vert", "res/shaders/cube.frag");

    s_ScreenShader.compilePath("res/shaders/screen.vert", "res/shaders/screen.frag");

    s_CubeShader.bind();
    s_CubeShader.setUniformMatrix4("u_Projection", camera.getProjectionMatrix());

    s_ScreenShader.bind();
    s_ScreenShader.setUniformInt("u_ScreenTexture", 0);

    s_CubeManager = CubeManager(&s_MoveManager);
    s_CubeManager.generate(s_CubeSize);

    s_Solver = Solver(&s_CubeManager, &s_MoveManager);

    MousePicker::init(&camera, &s_CubeManager);

    s_SettingsWindow = SettingsWindow(&s_CubeManager, &s_Solver, &s_MoveManager);

    ImguiWindowManager::init(s_Window);
    ImguiWindowManager::addWindow(&s_SettingsWindow);
}

void Application::run()
{
    while (!glfwWindowShouldClose(s_Window))
    {
        KRE::Clock::tick();

        glfwPollEvents();

        s_CubeManager.update(KRE::Clock::deltaTime);


        glBindFramebuffer(GL_FRAMEBUFFER, s_FBO);
        constexpr GLenum bufs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

        // Both Buffers
        glDrawBuffers(2, bufs);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Just Picker Buffer
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        glClearColor(-1.0f, -1.0f, -1.0f, -1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Both Buffers
        glDrawBuffers(2, bufs);

        s_CubeShader.bind();
        s_CubeShader.setUniformMatrix4("u_View", camera.getViewMatrix());
        s_CubeShader.setUniformVector3("u_ViewPos", camera.getPosition());

        s_CubeManager.draw(s_CubeShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // Draw Main buffer to screen
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        s_ScreenShader.bind();
        glBindVertexArray(s_ScreenVAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s_ScreenTexture);
        // glBindTexture(GL_TEXTURE_2D, s_PickingTexture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);


        // Imgui Rendering
        ImguiWindowManager::preRender();
        ImguiWindowManager::render();
        ImguiWindowManager::postRender();


        glfwSwapBuffers(s_Window);


        // Mouse Picking
        if (mousePicked && !s_PreviousMousePicked)
        {
            MousePicker::startPicking(s_FBO, { mousePosition.x, windowSize.y - mousePosition.y });
        }
        else if (!mousePicked && s_PreviousMousePicked)
        {
            MousePicker::endPicking();
        }

        s_PreviousMousePicked = mousePicked;
    }
}

void Application::changeSize(uint16_t newSize)
{
    s_CubeSize = newSize;
    s_CubeManager.destroy();
    s_MoveManager.reset();
    s_CubeManager.generate(s_CubeSize);

    MousePicker::init(&camera, &s_CubeManager);
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

    s_Window = glfwCreateWindow(windowSize.x, windowSize.y, s_Title.c_str(), NULL, NULL);

    if (!s_Window)
    {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(s_Window);

    glfwSetCursorPosCallback(s_Window, Application::mouseCallback);
    glfwSetKeyCallback(s_Window, Application::keyCallback);
    glfwSetMouseButtonCallback(s_Window, Application::mouseButtonCallback);
    glfwSetScrollCallback(s_Window, Application::scrollCallback);

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

    glGenVertexArrays(1, &s_ScreenVAO);
    glBindVertexArray(s_ScreenVAO);

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
    glGenFramebuffers(1, &s_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, s_FBO);

    glGenTextures(1, &s_ScreenTexture);
    glBindTexture(GL_TEXTURE_2D, s_ScreenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSize.x, windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &s_PickingTexture);
    glBindTexture(GL_TEXTURE_2D, s_PickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowSize.x, windowSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &s_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, s_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowSize.x, windowSize.y);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_ScreenTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, s_PickingTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, s_RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::Framebuffer is incomplete\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    static float lastX = 0.0f;
    static float lastY = 0.0f;

    Application::mousePosition = glm::vec2(xPos, yPos);

    if (Application::firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        Application::firstMouse = false;
    }

    float xOffset = xPos - lastX; 
    float yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    glm::vec4 mouseOffset(xOffset, yOffset, 0.0f, 1.0f);

    MousePicker::mouseMoved({ mouseOffset.x, mouseOffset.y });

    if (!Application::mouseMove)
        return;

    Application::camera.processAngleChange(xOffset, yOffset);
}

void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        if (!Application::mouseMove)
        {
            Application::mouseMove = true;
            Application::firstMouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        if (Application::mouseMove)
        {
            Application::mouseMove = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        Application::mousePicked = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        Application::mousePicked = false;
    }
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        return;
    }
}

void Application::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    Application::camera.processScrollWheel(-yOffset);
}

