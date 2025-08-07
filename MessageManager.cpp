// MessageManager.cpp
#include "MessageManager.h"
#include <imgui.h>

void MessageManager::addMessage(const std::string& message) {
    getMessages().push_back(message);
}

void MessageManager::clearMessages() {
    getMessages().clear();
}

void MessageManager::renderMessages() {
    auto& messages = getMessages();
    for (const auto& msg : messages) {
        ImGui::Text("%s", msg.c_str());
    }
}

std::vector<std::string>& MessageManager::getMessages() {
    static std::vector<std::string> messages;
    return messages;
}