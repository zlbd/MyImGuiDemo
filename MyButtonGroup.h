#pragma once

#include <imgui.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <queue>


// 颜色定义
constexpr ImVec4 NORMAL_COLOR(1.00f, 1.00f, 1.00f, 1.00f); // 白色
constexpr ImVec4 HOVER_COLOR(0.75f, 0.75f, 1.00f, 1.00f);  // 淡蓝色
constexpr ImVec4 HIGHLIGHT_COLOR(1.00f, 1.00f, 0.60f, 1.00f); // 淡黄色

class MyButtonGroup {
public:
    using ButtonConfig = std::tuple<std::string, float, std::function<void()>>;

    MyButtonGroup(const std::string& groupName, const std::vector<ButtonConfig>& buttonConfigs);
    void render();
    void setHighlight(const std::string& buttonName);
    void clickButton(const std::string& buttonName);
    const std::string& getGroupName() const { return groupName; }

private:
    struct Button {
        std::string name;
        float widthRatio;
        std::function<void()> callback;
        bool isHighlighted = false;
    };

    std::string groupName;
    std::vector<Button> buttons;
};

class MyButtonManager {
public:
    static void addGroup(MyButtonGroup* group);
    static void clickButton(const std::string& groupName, const std::string& buttonName);
    static void setHighlight(const std::string& groupName, const std::string& buttonName);

    // 添加延迟回调支持
    static void deferUIUpdate(const std::function<void()>& action);
    static void processDeferredUpdates();

private:
    static std::unordered_map<std::string, MyButtonGroup*>& getGroups();
    // 延迟回调队列
    static std::queue<std::function<void()>>& getDeferredQueue();
};