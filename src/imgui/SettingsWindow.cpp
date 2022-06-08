#include "SettingsWindow.hpp"

#include <iostream>

#include "../Application.hpp"

SettingsWindow::SettingsWindow(Application* application, uint16_t cubeSize)
    : m_CubeSize(cubeSize), m_Application(application)
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
        if (ImGui::SliderScalar("##Size", ImGuiDataType_U16, &m_CubeSize, &min, &max, "%d"))
        {
            m_Application->changeSize(m_CubeSize);
        }

        ImGui::PopItemWidth();
        ImGui::End();
    }
}
