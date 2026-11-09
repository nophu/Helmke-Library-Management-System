#include "messaging/message_bus.h"

size_t MessageBus::addReceiver(std::function<void(const Message &)> messageReceiver) {
    m_receivers.emplace_back(++m_nextId, std::move(messageReceiver));
    return m_nextId;
}

void MessageBus::removeReceiver(size_t id) {
    m_receivers.erase(std::remove_if(m_receivers.begin(), m_receivers.end(),
        [id](const auto &p) { return p.first == id; }), m_receivers.end());
}

void MessageBus::sendMessage(Message message) {
    m_messages.push(std::move(message));
}

void MessageBus::notify() {
    std::queue<Message> batch;
    batch.swap(m_messages);

    while (!batch.empty()) {
        for (auto &r : m_receivers) r.second(batch.front());
        batch.pop();
    }
}