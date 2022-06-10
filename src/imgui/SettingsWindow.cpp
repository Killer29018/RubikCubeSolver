#include "SettingsWindow.hpp"

#include <iostream>

#include "../Application.hpp"

SettingsWindow::SettingsWindow( uint16_t cubeSize)
    : m_CubeSize(cubeSize)
{
}

void SettingsWindow::renderImgui()
{
    if (ImGui::Begin("Settings"))
    {
        ImGui::PushItemWidth(-1);
        ImGui::Text("Hello World!");

        uint16_t min = 2, max = 3;

        ImGui::Text("Size");
        if (ImGui::SliderScalar("##CUBE_SIZE", ImGuiDataType_U16, &m_CubeSize, &min, &max, "%d"))
        {
            Application::changeSize(m_CubeSize);
        }

        if (ImGui::Button("Solve", ImVec2(-1, 20)))
        {
            Solver::solve();
        }

        static char str[200] = "";

        ImGui::Text("Scramble Sequence");
        ImGui::InputText("##SCRAMBLE_TEXT", &str[0], 200);

        if (ImGui::Button("Scramble", ImVec2(-1, 20)))
        {
            std::string input = std::string(str);
            CubeManager::applyMoves(input);
        }

        float minF = 0.01f, maxF = 2.0f;
        ImGui::SliderScalar("##MOVE_SECONDS", ImGuiDataType_Float, &Move::seconds, &minF, &maxF);

        ImGui::PopItemWidth();
        ImGui::End();
    }
}
