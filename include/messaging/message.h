#pragma once

#include <string>
#include <any>

class Message {
public:
    explicit Message(std::string event, std::any data = {})
        : m_event(std::move(event)), m_data(std::move(data)) {}

    [[nodiscard]] const std::string &getEvent() const { return m_event; }
    [[nodiscard]] bool is(const std::string &event) const { return m_event == event; }

    template <class T>
    const T *data() const { return std::any_cast<T>(&m_data); }

private:
    std::string m_event;
    std::any m_data;
};
