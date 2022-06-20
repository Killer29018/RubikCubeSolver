#ifndef IMGUI_SETTINGS_WINDOW_HPP
#define IMGUI_SETTINGS_WINDOW_HPP

#include "Window.hpp"

#include <cstdint>

#include "../MoveManager.hpp"

class SettingsWindow : public ImguiWindow
{
private:
    uint16_t m_CubeSize;

    MoveManager* moveManager;
public:
    SettingsWindow() = default;
    SettingsWindow(uint16_t cubeSize, MoveManager* moveManager);

    ~SettingsWindow() = default;

    void renderImgui() override;
private:
};

#endif
