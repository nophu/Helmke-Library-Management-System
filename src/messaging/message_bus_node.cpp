#include "messaging/message_bus_node.h"

#include "messaging/message_bus.h"

MessageBusNode::MessageBusNode(MessageBus *messageBus) {
    this->m_messageBus = messageBus;
    this->m_messageBus->addReceiver(this->getNotifyFunction());
    m_receiverId = m_messageBus->addReceiver(getNotifyFunction());
}

MessageBusNode::~MessageBusNode() {
    m_messageBus->removeReceiver(m_receiverId);
}

std::function<void (const Message &)> MessageBusNode::getNotifyFunction() {
    auto messageListener = [this](const Message &message) -> void {
        this->onNotify(message);
    };

    return messageListener;
}

void MessageBusNode::send(const Message &message) const {
    this->m_messageBus->sendMessage(message);
}