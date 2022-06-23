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

        static std::string scrambleMoves = "";
        static std::string solveMoves = "";

        uint16_t min = 2, max = 3;

        ImGui::Text("Cube Size");
        if (ImGui::SliderScalar("##CUBE_SIZE", ImGuiDataType_U16, &m_CubeSize, &min, &max, "%d"))
        {
            Application::changeSize(m_CubeSize);
        }

        if (ImGui::Button("Solve", ImVec2(-1, 20)))
        {
            m_Solver->solve();
            solveMoves = convertMovesToString(m_MoveManager->getAllMoves());
            m_MoveManager->reset();
        }

        float minF = 0.01f, maxF = 2.0f;
        ImGui::Text("Time for Moves (Seconds)");
        ImGui::SliderScalar("##MOVE_SECONDS", ImGuiDataType_Float, &Move::seconds, &minF, &maxF);

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        if (ImGui::CollapsingHeader("Scrambles"))
        {
            constexpr size_t SCRAMBLE_SEQ_MAX_SIZE = 1000;
            static char scrambleSeq[SCRAMBLE_SEQ_MAX_SIZE];

            if (ImGui::Button("Random Scramble", ImVec2(-1, 20)))
            {
                m_MoveGenerator->generateMoves();
            }

            if (ImGui::Button("Apply Scramble Sequence", ImVec2(-1, 20)))
            {
                std::string input = std::string(scrambleSeq);
                m_CubeManager->applyMoves(input);
            }

            ImGui::Text("Scramble Sequence");
            ImGui::PushTextWrapPos(200);
            ImGui::InputText("##SCRAMBLE_TEXT", &scrambleSeq[0], SCRAMBLE_SEQ_MAX_SIZE);
            ImGui::PopTextWrapPos();
        }

        if (ImGui::CollapsingHeader("Moves"))
        {
            if (m_MoveManager->movesUpdated())
            {
                scrambleMoves = convertMovesToString(m_MoveManager->getAllMoves());
            }

            ImGui::Text("Scramble Moves");
            ImGui::TextWrapped(scrambleMoves.c_str());

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Solve Moves");
            ImGui::TextWrapped(solveMoves.c_str());
        }

        ImGui::PopItemWidth();
        ImGui::End();
    }
}
