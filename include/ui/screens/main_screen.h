#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "ui/screens/screen.h"
#include "Library.h"

class MainScreen : public Screen {
public:
    Library& m_library;
    explicit MainScreen(Library& library, std::function<void()> onExit);

    void onShow() override;
    void render() override;

private:
    enum class View { Books, Patrons };
    enum class Popup { None, AddBook, RemoveBook, NewPatron, About };

    void renderMenuBar();
    void renderLeftPane(float width);
    void renderBookList();
    void renderPatronList();
    void renderRightPane();
    void renderBookDetails(const Book &book);

    void renderPopups();
    void renderAddBookPopup();
    void renderRemoveBookPopup();
    void renderNewPatronPopup();
    void renderAboutPopup();

    void refreshBooks();
    void refreshPatrons();
    void openAddBook();
    void openRemoveBook();
    void openNewPatron();

    std::function<void()> m_onExit;
    std::vector<std::shared_ptr<Book>> m_books;
    std::vector<Patron> m_patrons;
    int m_selectedBook = -1;
    int m_selectedPatron = -1;
    View m_view = View::Books;
    Popup m_pendingPopup = Popup::None;
    std::string m_formError;

    char m_bookTitle[128] = {};
    char m_bookAuthor[128] = {};
    char m_bookIsbn[32] = {};
    char m_bookCallNumber[64] = {};
    int m_bookGenre = 0;

    std::string m_removeCallNumber;
    std::string m_removeTitle;

    char m_patronName[128] = {};
    char m_patronEmail[128] = {};
    int m_patronType = 0;

    float m_splitRatio = 0.60f; // 60% pane split
};
