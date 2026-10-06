#include "ui/ui_manager.h"

#include <functional>

void UIManager::registerScreen(const std::string &name, std::function<std::unique_ptr<Screen>()> factory) {
    m_factories[name] = std::move(factory);
}

void UIManager::switchTo(const std::string &name) {
    m_pendingSwitch = name;
}

void UIManager::update(float deltaTime) {
    if (!m_pendingSwitch.empty()) {
        if (m_currentScreen != nullptr) {
            m_currentScreen->onClose();
        }

        m_currentScreen = m_factories.at(m_pendingSwitch)();
        m_currentScreen->onShow();
        m_pendingSwitch.clear();
    }

    if (m_currentScreen != nullptr) {
        m_currentScreen->update(deltaTime);
    }
}

void UIManager::render() const {
    if (m_currentScreen != nullptr) {
        m_currentScreen->render();
    }
}
