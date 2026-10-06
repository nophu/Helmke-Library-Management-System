#include "ui/screens/main_screen.h"

#include "imgui.h"

MainScreen::MainScreen(std::function<void()> onExit) : m_onExit(std::move(onExit)) {}

void MainScreen::render() {
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoScrollbar;

    ImGui::Begin("MainScreen", nullptr, flags);

    renderMenuBar();

    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float totalWidth = ImGui::GetContentRegionAvail().x - spacing;
    const float leftWidth = totalWidth * m_splitRatio;

    renderLeftPane(leftWidth);
    ImGui::SameLine();
    renderRightPane();

    ImGui::End();
}

void MainScreen::renderMenuBar() const {
    if (!ImGui::BeginMenuBar()) return;

    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New Patron", "Ctrl+N")) {
            // TODO
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", "Alt+F4")) {
            if (m_onExit) m_onExit();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Patrons")) {
            // TODO: switch view
        }
        if (ImGui::MenuItem("Books")) {
            // TODO: switch view
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
        if (ImGui::MenuItem("About")) {
            // TODO
        }
        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
}

void MainScreen::renderLeftPane(const float width) {
    ImGui::BeginChild("LeftPane", ImVec2(width, 0), ImGuiChildFlags_Borders);
    ImGui::Text("Left pane (60%%)");
    ImGui::Separator();

    // Pane contents

    ImGui::EndChild();
}

void MainScreen::renderRightPane() {
    ImGui::BeginChild("RightPane", ImVec2(0, 0), ImGuiChildFlags_Borders);
    ImGui::Text("Right pane (40%%)");
    ImGui::Separator();

    // Pane contents

    ImGui::EndChild();
}