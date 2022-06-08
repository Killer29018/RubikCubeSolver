#ifndef IMGUI_SETTINGS_WINDOW_HPP
#define IMGUI_SETTINGS_WINDOW_HPP

#include "Window.hpp"

#include <cstdint>

class SettingsWindow : public ImguiWindow
{
private:
    uint16_t m_CubeSize;
public:
    SettingsWindow() = default;
    SettingsWindow(uint16_t cubeSize);

    ~SettingsWindow() = default;

    void renderImgui() override;
private:
};

#endif
