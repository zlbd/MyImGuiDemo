#include "MyButtonGroup.h"

MyButtonGroup::MyButtonGroup(const std::string& groupName,
    const std::vector<ButtonConfig>& buttonConfigs)
    : groupName(groupName) {
    for (const auto& config : buttonConfigs) {
        buttons.push_back({
            std::get<0>(config),  // name
            std::get<1>(config),  // widthRatio
            std::get<2>(config),  // callback
            false                 // isHighlighted
            });
    }
}

void MyButtonGroup::render() {
    ImGui::PushID(groupName.c_str());

    // ��ȡ���ÿ�ȣ����ǹ������ȣ�
    float totalWidth = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ScrollbarSize;
    float spacing = ImGui::GetStyle().ItemSpacing.x;
    int buttonCount = static_cast<int>(buttons.size());

    // �����ܱ���
    float totalRatio = 0.0f;
    for (const auto& btn : buttons) {
        totalRatio += btn.widthRatio;
    }

    // ������ÿ�ȣ���ȥ��ࣩ
    float availableWidth = totalWidth - spacing * (buttonCount - 1);

    // Ԥ����ÿ����ť�����ؿ�ȣ�ʹ��������
    std::vector<float> buttonWidths;
    float usedWidth = 0.0f;
    for (int i = 0; i < buttonCount - 1; ++i) {
        float width = (buttons[i].widthRatio / totalRatio) * availableWidth;
        width = std::floor(width); // ʹ����������
        buttonWidths.push_back(width);
        usedWidth += width;
    }
    // ���һ����ťʹ��ʣ���ȣ�ȷ���ܿ��׼ȷ��
    float lastWidth = availableWidth - usedWidth;
    buttonWidths.push_back(lastWidth);

    // ��Ⱦ��ť
    for (int i = 0; i < buttonCount; ++i) {
        auto& btn = buttons[i];
        float width = buttonWidths[i];
        ImVec2 buttonSize(width, 40);

        // ��ȡ��ǰλ��
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImVec2 rectMin = cursorPos;
        ImVec2 rectMax(cursorPos.x + width, cursorPos.y + buttonSize.y);

        // ������ɫ
        ImVec4 color = btn.isHighlighted ? HIGHLIGHT_COLOR : NORMAL_COLOR;
        if (ImGui::IsMouseHoveringRect(rectMin, rectMax)) {
            color = btn.isHighlighted ?
                ImVec4(HIGHLIGHT_COLOR.x * 0.9f, HIGHLIGHT_COLOR.y * 0.9f, HIGHLIGHT_COLOR.z * 0.8f, 1.0f) :
                HOVER_COLOR;
        }

        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

        // ��Ⱦ��ť
        if (ImGui::Button(btn.name.c_str(), buttonSize)) {
            // ���������ť�ĸ���
            for (auto& b : buttons) {
                b.isHighlighted = false;
            }
            btn.isHighlighted = true;
            if (btn.callback) {
                btn.callback();
            }
        }

        ImGui::PopStyleColor(3);

        // ͬһ������Ⱦ�����һ����ť�󲻼ӣ�
        if (i < buttonCount - 1) {
            ImGui::SameLine(0.0f, spacing);
        }
    }

    ImGui::PopID();
}

void MyButtonGroup::setHighlight(const std::string& buttonName) {
    for (auto& btn : buttons) {
        btn.isHighlighted = (btn.name == buttonName);
    }
}

void MyButtonGroup::clickButton(const std::string& buttonName) {
    for (auto& btn : buttons) {
        if (btn.name == buttonName) {
            // ���������ť�ĸ���
            for (auto& b : buttons) {
                b.isHighlighted = false;
            }
            btn.isHighlighted = true;
            if (btn.callback) {
                btn.callback();
            }
            break;
        }
    }
}

// MyButtonManager ʵ��
void MyButtonManager::addGroup(MyButtonGroup* group) {
    getGroups()[group->getGroupName()] = group;
}

void MyButtonManager::clickButton(const std::string& groupName, const std::string& buttonName) {
    auto& groups = getGroups();
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        it->second->clickButton(buttonName);
    }
}

void MyButtonManager::setHighlight(const std::string& groupName, const std::string& buttonName) {
    auto& groups = getGroups();
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        it->second->setHighlight(buttonName);
    }
}

std::unordered_map<std::string, MyButtonGroup*>& MyButtonManager::getGroups() {
    static std::unordered_map<std::string, MyButtonGroup*> groups;
    return groups;
}

// �ӳٻص�ʵ��
void MyButtonManager::deferUIUpdate(const std::function<void()>& action) {
    getDeferredQueue().push(action);
}

void MyButtonManager::processDeferredUpdates() {
    auto& queue = getDeferredQueue();
    while (!queue.empty()) {
        queue.front()();
        queue.pop();
    }
}

std::queue<std::function<void()>>& MyButtonManager::getDeferredQueue() {
    static std::queue<std::function<void()>> deferredQueue;
    return deferredQueue;
}
