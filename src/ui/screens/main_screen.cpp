#include <iostream>
#include <string>

#include "ui/screens/main_screen.h"

#include "imgui.h"

#include "Library.h"
#include "LibraryItem.h"

MainScreen::MainScreen(Library& library, std::function<void()> onExit) 
    : m_library(library), m_onExit(std::move(onExit)) {}

void MainScreen::onShow() {
    refreshBooks();
}

void MainScreen::refreshBooks() {
    m_books = m_library.getBooks();
    m_selectedBook = -1;
}

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

void MainScreen::renderMenuBar() {
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
            refreshBooks();
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
                refreshBooks();
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

    if (m_books.empty()) {
        ImGui::TextDisabled("No books yet");
    }

    const ImGuiStyle &style = ImGui::GetStyle();
    const float lineHeight = ImGui::GetTextLineHeight();
    const float entryHeight = lineHeight * 2.0f + style.FramePadding.y * 2.0f;

    for (int i = 0; i < static_cast<int>(m_books.size()); ++i) {
        const auto &book = m_books[i];
        if (!book) continue;

        ImGui::PushID(i);

        const ImVec2 pos = ImGui::GetCursorScreenPos();
        if (ImGui::Selectable("##book", m_selectedBook == i, ImGuiSelectableFlags_None, ImVec2(0, entryHeight))) {
            m_selectedBook = i;
        }

        const std::string title = book->getTitle();
        const std::string author = book->getAuthor();
        const float textX = pos.x + style.FramePadding.x;
        const float textY = pos.y + style.FramePadding.y;

        ImDrawList *drawList = ImGui::GetWindowDrawList();
        drawList->AddText(ImVec2(textX, textY), ImGui::GetColorU32(ImGuiCol_Text), title.c_str());
        drawList->AddText(ImVec2(textX, textY + lineHeight), ImGui::GetColorU32(ImGuiCol_TextDisabled), author.c_str());

        ImGui::PopID();
    }

    ImGui::EndChild();
}

void MainScreen::renderRightPane() {
    ImGui::BeginChild("RightPane", ImVec2(0, 0), ImGuiChildFlags_Borders);
    ImGui::Text("Right pane (40%%)");
    ImGui::Separator();

    // Pane contents

    ImGui::EndChild();
}