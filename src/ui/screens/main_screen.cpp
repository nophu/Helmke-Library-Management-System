#include <algorithm>
#include <string>

#include "ui/screens/main_screen.h"

#include "imgui.h"

#include "Library.h"
#include "LibraryItem.h"

namespace {
    const char *const kGenreNames[] = {"Nonfiction", "Fiction", "Fantasy", "Mystery", "Romance"};
    const char *const kPatronTypeNames[] = {"Student", "Faculty", "Staff"};
    const PatronType kPatronTypes[] = {PatronType::Student, PatronType::Faculty, PatronType::Staff};

    std::string trimmed(const char *text) {
        std::string value(text);
        const auto first = value.find_first_not_of(" \t");
        if (first == std::string::npos) return {};
        const auto last = value.find_last_not_of(" \t");
        return value.substr(first, last - first + 1);
    }

    bool listEntry(const std::string &title, const std::string &subtitle, const bool selected) {
        const ImGuiStyle &style = ImGui::GetStyle();
        const float lineHeight = ImGui::GetTextLineHeight();
        const float entryHeight = lineHeight * 2.0f + style.FramePadding.y * 2.0f;

        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const bool clicked = ImGui::Selectable("##entry", selected, ImGuiSelectableFlags_None, ImVec2(0, entryHeight));

        const float textX = pos.x + style.FramePadding.x;
        const float textY = pos.y + style.FramePadding.y;

        ImDrawList *drawList = ImGui::GetWindowDrawList();
        drawList->AddText(ImVec2(textX, textY), ImGui::GetColorU32(ImGuiCol_Text), title.c_str());
        drawList->AddText(ImVec2(textX, textY + lineHeight), ImGui::GetColorU32(ImGuiCol_TextDisabled), subtitle.c_str());

        return clicked;
    }

    bool beginCenteredModal(const char *name) {
        const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        return ImGui::BeginPopupModal(name, nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    }

    void detailRow(const char *label, const std::string &value) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextDisabled("%s", label);
        ImGui::TableNextColumn();
        ImGui::TextWrapped("%s", value.empty() ? "-" : value.c_str());
    }

    void formError(const std::string &message) {
        if (message.empty()) return;
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", message.c_str());
    }
}

MainScreen::MainScreen(Library& library, std::function<void()> onExit) 
    : m_library(library), m_onExit(std::move(onExit)) {}

void MainScreen::onShow() {
    refreshBooks();
    refreshPatrons();
}

void MainScreen::refreshBooks() {
    m_books = m_library.getBooks();
    m_selectedBook = -1;
}

void MainScreen::refreshPatrons() {
    m_patrons = m_library.getPatrons();
    m_selectedPatron = -1;
}

void MainScreen::openAddBook() {
    m_bookTitle[0] = '\0';
    m_bookAuthor[0] = '\0';
    m_bookIsbn[0] = '\0';
    m_bookCallNumber[0] = '\0';
    m_bookGenre = 0;
    m_formError.clear();
    m_pendingPopup = Popup::AddBook;
}

void MainScreen::openRemoveBook() {
    if (m_selectedBook < 0 || m_selectedBook >= static_cast<int>(m_books.size())) return;
    const auto &book = m_books[m_selectedBook];
    if (!book) return;

    m_removeCallNumber = book->getCallNumber();
    m_removeTitle = book->getTitle();
    m_formError.clear();
    m_pendingPopup = Popup::RemoveBook;
}

void MainScreen::openNewPatron() {
    m_patronName[0] = '\0';
    m_patronEmail[0] = '\0';
    m_patronType = 0;
    m_formError.clear();
    m_pendingPopup = Popup::NewPatron;
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
    renderPopups();

    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float totalWidth = ImGui::GetContentRegionAvail().x - spacing;
    const float leftWidth = totalWidth * m_splitRatio;

    renderLeftPane(leftWidth);
    ImGui::SameLine();
    renderRightPane();

    ImGui::End();
}

void MainScreen::renderMenuBar() {
    const bool popupOpen = ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);
    if (!popupOpen && ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_N)) {
        openNewPatron();
    }

    if (!ImGui::BeginMenuBar()) return;

    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New Patron", "Ctrl+N")) {
            openNewPatron();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", "Alt+F4")) {
            if (m_onExit) m_onExit();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Book")) {
        if (ImGui::MenuItem("Get")) {
            refreshBooks();
            m_view = View::Books;
        }

        if (ImGui::MenuItem("Add")) {
            openAddBook();
        }

        const bool canRemove = m_view == View::Books && m_selectedBook >= 0;
        if (ImGui::MenuItem("Remove", nullptr, false, canRemove)) {
            openRemoveBook();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Patrons", nullptr, m_view == View::Patrons)) {
            refreshPatrons();
            m_view = View::Patrons;
        }
        if (ImGui::MenuItem("Books", nullptr, m_view == View::Books)) {
            refreshBooks();
            m_view = View::Books;
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
        if (ImGui::MenuItem("About")) {
            m_pendingPopup = Popup::About;
        }
        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
}

void MainScreen::renderPopups() {
    switch (m_pendingPopup) {
        case Popup::AddBook:    ImGui::OpenPopup("Add Book"); break;
        case Popup::RemoveBook: ImGui::OpenPopup("Remove Book"); break;
        case Popup::NewPatron:  ImGui::OpenPopup("New Patron"); break;
        case Popup::About:      ImGui::OpenPopup("About"); break;
        case Popup::None:       break;
    }
    m_pendingPopup = Popup::None;

    renderAddBookPopup();
    renderRemoveBookPopup();
    renderNewPatronPopup();
    renderAboutPopup();
}

void MainScreen::renderAddBookPopup() {
    if (!beginCenteredModal("Add Book")) return;

    ImGui::PushItemWidth(280.0f);
    if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();
    ImGui::InputText("Title", m_bookTitle, sizeof(m_bookTitle));
    ImGui::InputText("Author", m_bookAuthor, sizeof(m_bookAuthor));
    ImGui::InputText("ISBN", m_bookIsbn, sizeof(m_bookIsbn));
    ImGui::InputText("Call number", m_bookCallNumber, sizeof(m_bookCallNumber));
    ImGui::Combo("Genre", &m_bookGenre, kGenreNames, IM_ARRAYSIZE(kGenreNames));
    ImGui::PopItemWidth();

    formError(m_formError);
    ImGui::Spacing();

    if (ImGui::Button("Add", ImVec2(120, 0))) {
        const std::string title = trimmed(m_bookTitle);
        const std::string author = trimmed(m_bookAuthor);
        const std::string isbn = trimmed(m_bookIsbn);
        const std::string callNumber = trimmed(m_bookCallNumber);

        const auto sameCallNumber = [&callNumber](const std::shared_ptr<Book> &book) {
            return book && book->getCallNumber() == callNumber;
        };

        refreshBooks();
        if (title.empty()) {
            m_formError = "Title is required.";
        } else if (callNumber.empty()) {
            m_formError = "Call number is required.";
        } else if (std::any_of(m_books.begin(), m_books.end(), sameCallNumber)) {
            m_formError = "A book with that call number already exists.";
        } else {
            const auto genre = static_cast<GenreType>(m_bookGenre);
            m_library.addItem(std::make_shared<Book>(title, author, isbn, genre, callNumber, true));
            refreshBooks();

            if (std::any_of(m_books.begin(), m_books.end(), sameCallNumber)) {
                m_view = View::Books;
                ImGui::CloseCurrentPopup();
            } else {
                m_formError = "The book could not be saved.";
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void MainScreen::renderRemoveBookPopup() {
    if (!beginCenteredModal("Remove Book")) return;

    ImGui::Text("Remove \"%s\"?", m_removeTitle.c_str());
    ImGui::TextDisabled("Call number: %s", m_removeCallNumber.c_str());

    formError(m_formError);
    ImGui::Spacing();

    if (ImGui::Button("Remove", ImVec2(120, 0))) {
        if (m_library.removeItem(m_removeCallNumber)) {
            refreshBooks();
            ImGui::CloseCurrentPopup();
        } else {
            m_formError = "The book could not be removed.";
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void MainScreen::renderNewPatronPopup() {
    if (!beginCenteredModal("New Patron")) return;

    ImGui::PushItemWidth(280.0f);
    if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();
    ImGui::InputText("Name", m_patronName, sizeof(m_patronName));
    ImGui::InputText("Email", m_patronEmail, sizeof(m_patronEmail));
    ImGui::Combo("Type", &m_patronType, kPatronTypeNames, IM_ARRAYSIZE(kPatronTypeNames));
    ImGui::PopItemWidth();

    formError(m_formError);
    ImGui::Spacing();

    if (ImGui::Button("Create", ImVec2(120, 0))) {
        const std::string name = trimmed(m_patronName);
        const std::string email = trimmed(m_patronEmail);

        if (name.empty()) {
            m_formError = "Name is required.";
        } else if (!email.empty() && email.find('@') == std::string::npos) {
            m_formError = "Email must contain @.";
        } else {
            refreshPatrons();
            int nextId = 1;
            for (const Patron &patron : m_patrons) {
                nextId = std::max(nextId, patron.getID() + 1);
            }

            if (m_library.addPatron(Patron(nextId, name, email, kPatronTypes[m_patronType]))) {
                refreshPatrons();
                m_view = View::Patrons;
                ImGui::CloseCurrentPopup();
            } else {
                m_formError = "The patron could not be saved.";
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void MainScreen::renderAboutPopup() {
    if (!beginCenteredModal("About")) return;

    ImGui::TextUnformatted("Helmke Library Management System");
    ImGui::TextDisabled("Built with Dear ImGui %s, GLFW and SQLite", IMGUI_VERSION);
    ImGui::Spacing();

    if (ImGui::Button("Close", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void MainScreen::renderLeftPane(const float width) {
    ImGui::BeginChild("LeftPane", ImVec2(width, 0), ImGuiChildFlags_Borders);

    if (m_view == View::Books) {
        renderBookList();
    } else {
        renderPatronList();
    }

    ImGui::EndChild();
}

void MainScreen::renderBookList() {
    if (m_books.empty()) {
        ImGui::TextDisabled("No books yet");
    }

    for (int i = 0; i < static_cast<int>(m_books.size()); ++i) {
        const auto &book = m_books[i];
        if (!book) continue;

        ImGui::PushID(i);
        if (listEntry(book->getTitle(), book->getAuthor(), m_selectedBook == i)) {
            m_selectedBook = i;
        }
        ImGui::PopID();
    }
}

void MainScreen::renderPatronList() {
    if (m_patrons.empty()) {
        ImGui::TextDisabled("No patrons yet");
    }

    for (int i = 0; i < static_cast<int>(m_patrons.size()); ++i) {
        const Patron &patron = m_patrons[i];
        const std::string type = Patron::typeToString(patron.getType());
        const std::string subtitle = patron.getEmail().empty() ? type : patron.getEmail() + " - " + type;

        ImGui::PushID(i);
        if (listEntry(patron.getName(), subtitle, m_selectedPatron == i)) {
            m_selectedPatron = i;
        }
        ImGui::PopID();
    }
}

void MainScreen::renderRightPane() {
    ImGui::BeginChild("RightPane", ImVec2(0, 0), ImGuiChildFlags_Borders);

    const bool hasBook = m_view == View::Books &&
                         m_selectedBook >= 0 &&
                         m_selectedBook < static_cast<int>(m_books.size()) &&
                         m_books[m_selectedBook];

    if (hasBook) {
        renderBookDetails(*m_books[m_selectedBook]);
    } else if (m_view == View::Books) {
        ImGui::TextDisabled("Select a book to see its details");
    }

    ImGui::EndChild();
}

void MainScreen::renderBookDetails(const Book &book) {
    ImGui::TextWrapped("%s", book.getTitle().c_str());
    if (!book.getAuthor().empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
        ImGui::TextWrapped("%s", book.getAuthor().c_str());
        ImGui::PopStyleColor();
    }
    ImGui::Separator();

    const int genreIndex = static_cast<int>(book.getGenre());
    const bool knownGenre = genreIndex >= 0 && genreIndex < IM_ARRAYSIZE(kGenreNames);

    if (ImGui::BeginTable("BookDetails", 2, ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        detailRow("Call number", book.getCallNumber());
        detailRow("ISBN", book.getIsbn());
        detailRow("Genre", knownGenre ? kGenreNames[genreIndex] : "");
        detailRow("Status", book.getAvailability() ? "Available" : "Checked out");

        ImGui::EndTable();
    }
}
