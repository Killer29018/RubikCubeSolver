#ifndef IMGUI_SETTINGS_WINDOW_HPP
#define IMGUI_SETTINGS_WINDOW_HPP

#include "Window.hpp"

#include <cstdint>

class Application;

class SettingsWindow : public ImguiWindow
{
private:
    uint16_t m_CubeSize;
    Application* m_Application;
public:
    SettingsWindow() = default;
    SettingsWindow(Application* app, uint16_t cubeSize);

    ~SettingsWindow() = default;

    void renderImgui() override;
private:
};

#endif
