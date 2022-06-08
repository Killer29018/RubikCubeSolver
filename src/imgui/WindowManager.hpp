#ifndef IMGUI_WINDOW_MANAGER_HPP
#define IMGUI_WINDOW_MANAGER_HPP

#include "GLFW/glfw3.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>

#include "Window.hpp"

class ImguiWindowManager
{
public:
    static ImGuiIO* io;
private:
    static std::vector<ImguiWindow*> m_ImguiWindows;

    static GLFWwindow* m_Window;

public:
    static void init(GLFWwindow* window);

    static void addWindow(ImguiWindow* window);

    static void preRender();
    static void render();
    static void postRender();
private:
    ImguiWindowManager() = default;
    ~ImguiWindowManager() = default;

    static void initImgui();
    static void setupStyle();
};

#endif
