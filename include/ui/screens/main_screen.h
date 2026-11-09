#pragma once

#include <functional>
#include <memory>
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
    void renderMenuBar();
    void renderLeftPane(float width);
    void refreshBooks();
    static void renderRightPane();

    std::function<void()> m_onExit;
    std::vector<std::shared_ptr<Book>> m_books;
    int m_selectedBook = -1;
    float m_splitRatio = 0.60f; // 60% pane split
};