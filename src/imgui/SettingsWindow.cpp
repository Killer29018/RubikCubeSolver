#include "SettingsWindow.hpp"

#include <iostream>

#include "../Application.hpp"

SettingsWindow::SettingsWindow(CubeManager* cubeManager, 
        Solver* solver, MoveManager* moveManager, MoveGenerator* moveGenerator)
    : m_CubeManager(cubeManager), m_Solver(solver), m_MoveManager(moveManager),
    m_MoveGenerator(moveGenerator)
{
    m_CubeSize = m_CubeManager->getSize();
}

void SettingsWindow::renderImgui()
{
    if (ImGui::Begin("Settings"))
    {
        ImGui::PushItemWidth(-1);

        ImGui::Text("%.3f", (1.0f / KRE::Clock::deltaTime));

        uint16_t min = 2, max = 3;

        constexpr size_t SCRAMBLE_SEQ_MAX_SIZE = 1000;
        static char scrambleSeq[SCRAMBLE_SEQ_MAX_SIZE] = "";

        ImGui::Text("Cube Size");
        if (ImGui::SliderScalar("##CUBE_SIZE", ImGuiDataType_U16, &m_CubeSize, &min, &max, "%d"))
        {
            Application::changeSize(m_CubeSize);
        }

        if (ImGui::Button("Solve", ImVec2(-1, 20)))
        {
            m_Solver->solve();
        }

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        if (ImGui::Button("Scramble", ImVec2(-1, 20)))
        {
            std::string input = std::string(scrambleSeq);
            m_CubeManager->applyMoves(input);
        }

        ImGui::Text("Scramble Sequence");
        ImGui::InputText("##SCRAMBLE_TEXT", &scrambleSeq[0], SCRAMBLE_SEQ_MAX_SIZE);

        float minF = 0.01f, maxF = 2.0f;
        ImGui::SliderScalar("##MOVE_SECONDS", ImGuiDataType_Float, &Move::seconds, &minF, &maxF);

        if (ImGui::Button("Random Scramble", ImVec2(-1, 20)))
        {
            m_MoveGenerator->generateMoves();
        }

        if (ImGui::Button("Output Solve", ImVec2(-1, 20)))
        {
            std::cout << convertMovesToString(m_MoveManager->getAllMoves()) << "\n";
        }

        ImGui::PopItemWidth();
        ImGui::End();
    }
}
