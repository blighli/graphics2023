#pragma once

#include <string>
#include <vector>

class MessageBox
{
public:
    MessageBox(const MessageBox&) = delete;
    MessageBox(MessageBox&&) = delete;
    MessageBox& operator=(const MessageBox&) = delete;
    MessageBox& operator=(MessageBox&&) = delete;
private:
    MessageBox() {}
    ~MessageBox() {}

    std::vector<std::string_view> m_messageLines{};
    float m_progress{.0f};
};