#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "screens/screen.h"

class UIManager {
public:
    void registerScreen(const std::string &name, std::function<std::unique_ptr<Screen>()> factory);
    void switchTo(const std::string &name);
    void update(float deltaTime);
    void render() const;

private:
    std::unordered_map<std::string, std::function<std::unique_ptr<Screen>()>> m_factories;
    std::unique_ptr<Screen> m_currentScreen;
    std::string m_pendingSwitch; // Queues a screen to switch when one is already rendering
};
