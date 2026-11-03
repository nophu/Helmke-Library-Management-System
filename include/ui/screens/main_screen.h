#pragma once

#include <functional>

#include "ui/screens/screen.h"
#include "Library.h"

class MainScreen : public Screen {
public:
    Library& m_library;
    explicit MainScreen(Library& library, std::function<void()> onExit);

    void render() override;

private:
    void renderMenuBar() const;
    static void renderLeftPane(float width);
    static void renderRightPane();

    std::function<void()> m_onExit;
    float m_splitRatio = 0.60f; // 60% pane split
};