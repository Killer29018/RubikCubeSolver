#ifndef IMGUI_SETTINGS_WINDOW_HPP
#define IMGUI_SETTINGS_WINDOW_HPP

#include "Window.hpp"

#include <cstdint>

#include "../MoveManager.hpp"
#include "../CubeManager.hpp"
#include "../Solver.hpp"
#include "../MoveGenerator.hpp"

class SettingsWindow : public ImguiWindow
{
private:
    uint16_t m_CubeSize;

    CubeManager* m_CubeManager;
    Solver* m_Solver;
    MoveManager* m_MoveManager;
    MoveGenerator* m_MoveGenerator;
public:
    SettingsWindow() = default;
    SettingsWindow(CubeManager* cubeManager, Solver* solver, 
            MoveManager* moveManager, MoveGenerator* moveGenerator);

    ~SettingsWindow() = default;

    void renderImgui() override;
private:
};

#endif
