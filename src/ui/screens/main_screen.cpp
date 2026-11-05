#include <iostream>

#include "ui/screens/main_screen.h"

#include "imgui.h"

#include "Library.h"
#include "LibraryItem.h"

MainScreen::MainScreen(Library& library, std::function<void()> onExit) 
    : m_library(library), m_onExit(std::move(onExit)) {}

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

    if (ImGui::BeginMenu("Book")) {
        if (ImGui::MenuItem("Get")) {
            std::vector<std::shared_ptr<Book>> items = m_library.getBooks();

            std::cout << "Num of books: " << items.size() << "\n";

            for (const auto& item : items) {
                if (item) {
                    std::cout << item->getTitle() << ", ";
                    std::cout << item->getCallNumber() << ", ";
                    std::cout << item->getIsbn() << ", ";
                    std::cout << static_cast<int>(item->getGenre()) << ", ";
                    std::cout << item->getAuthor() << ", ";
                    std::cout << item->getAvailability() << "\n";
                }
            }
        }

        if (ImGui::MenuItem("Add")) {
            std::shared_ptr<Book> book = std::make_shared<Book>("Title", "Author", "isbn", GenreType::Nonfiction, "Call Num", true);

            m_library.addItem(book);
        }
        if (ImGui::MenuItem("Remove")) {
            std::string callNumToRemove = "";
            std::cout << "Enter call number to remove: ";
            std::getline(std::cin, callNumToRemove);

            if (!m_library.removeItem(callNumToRemove)) {
                std::cout << "Failed to remove book.\n";
            }
            else {
                std::cout << "Book removed successfully!\n";
            }
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