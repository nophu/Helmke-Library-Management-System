#pragma once

#include <functional>
#include <queue>
#include <vector>

#include "message.h"

#include <utility>

class MessageBus {
public:
    MessageBus() = default;
    ~MessageBus() = default;

    size_t addReceiver(std::function<void (const Message &)> messageReceiver);
    void removeReceiver(size_t id);
    void sendMessage(Message message);
    void notify();

private:
    std::vector<std::pair<size_t, std::function<void (const Message &)>>> m_receivers;
    std::queue<Message> m_messages;
    size_t m_nextId = 0;
};
