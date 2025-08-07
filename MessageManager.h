// MessageManager.h
#pragma once
#include <string>
#include <vector>
#include <functional>

class MessageManager {
public:
    static void addMessage(const std::string& message);
    static void clearMessages();
    static void renderMessages();

private:
    static std::vector<std::string>& getMessages();
};
