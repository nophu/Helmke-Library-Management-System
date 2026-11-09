#pragma once
#include <functional>

#include "message.h"

class MessageBus;

class MessageBusNode {
public:
    explicit MessageBusNode(MessageBus *messageBus);

    ~MessageBusNode();

protected:
    MessageBus *m_messageBus;

    std::function<void (const Message &)> getNotifyFunction();
    virtual void onNotify(const Message &message) = 0;
    void send(const Message &message) const;

private:
    size_t m_receiverId;
};
