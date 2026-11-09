#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "messaging/message_bus_node.h"
#include "screens/screen.h"
#include "messaging/events.h"

class UIManager : public MessageBusNode {
public:
    explicit UIManager(MessageBus *messageBus) : MessageBusNode(messageBus) {}

    void registerScreen(const std::string &name, std::function<std::unique_ptr<Screen>()> factory);
    void switchTo(const std::string &name);
    void update(float deltaTime);
    void render() const;

protected:
    void onNotify(const Message &m) override {
        if (m.is(Events::SwitchScreen))
            if (const auto *name = m.data<std::string>()) switchTo(*name);
    }

private:
    std::unordered_map<std::string, std::function<std::unique_ptr<Screen>()>> m_factories;
    std::unique_ptr<Screen> m_currentScreen;
    std::string m_pendingSwitch; // Queues a screen to switch when one is already rendering
};
