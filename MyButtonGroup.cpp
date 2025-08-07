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

    // 获取可用宽度（考虑滚动条等）
    float totalWidth = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ScrollbarSize;
    float spacing = ImGui::GetStyle().ItemSpacing.x;
    int buttonCount = static_cast<int>(buttons.size());

    // 计算总比例
    float totalRatio = 0.0f;
    for (const auto& btn : buttons) {
        totalRatio += btn.widthRatio;
    }

    // 计算可用宽度（减去间距）
    float availableWidth = totalWidth - spacing * (buttonCount - 1);

    // 预计算每个按钮的像素宽度（使用整数）
    std::vector<float> buttonWidths;
    float usedWidth = 0.0f;
    for (int i = 0; i < buttonCount - 1; ++i) {
        float width = (buttons[i].widthRatio / totalRatio) * availableWidth;
        width = std::floor(width); // 使用整数像素
        buttonWidths.push_back(width);
        usedWidth += width;
    }
    // 最后一个按钮使用剩余宽度（确保总宽度准确）
    float lastWidth = availableWidth - usedWidth;
    buttonWidths.push_back(lastWidth);

    // 渲染按钮
    for (int i = 0; i < buttonCount; ++i) {
        auto& btn = buttons[i];
        float width = buttonWidths[i];
        ImVec2 buttonSize(width, 40);

        // 获取当前位置
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImVec2 rectMin = cursorPos;
        ImVec2 rectMax(cursorPos.x + width, cursorPos.y + buttonSize.y);

        // 设置颜色
        ImVec4 color = btn.isHighlighted ? HIGHLIGHT_COLOR : NORMAL_COLOR;
        if (ImGui::IsMouseHoveringRect(rectMin, rectMax)) {
            color = btn.isHighlighted ?
                ImVec4(HIGHLIGHT_COLOR.x * 0.9f, HIGHLIGHT_COLOR.y * 0.9f, HIGHLIGHT_COLOR.z * 0.8f, 1.0f) :
                HOVER_COLOR;
        }

        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

        // 渲染按钮
        if (ImGui::Button(btn.name.c_str(), buttonSize)) {
            // 清除其他按钮的高亮
            for (auto& b : buttons) {
                b.isHighlighted = false;
            }
            btn.isHighlighted = true;
            if (btn.callback) {
                btn.callback();
            }
        }

        ImGui::PopStyleColor(3);

        // 同一行内渲染（最后一个按钮后不加）
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
            // 清除其他按钮的高亮
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

// MyButtonManager 实现
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

// 延迟回调实现
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
