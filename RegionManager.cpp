#include "RegionManager.h"
#include <iostream>

bool Region::IsLeaf() const {
    return type == REGION_LEAF;
}

Region* Region::FindRegion(const std::string& targetId) {
    if (id == targetId) return this;
    for (auto& child : children) {
        if (Region* found = child.FindRegion(targetId)) {
            return found;
        }
    }
    return nullptr;
}

std::string IDGenerator::GetID(const std::string& prefix) {
    return prefix + "##" + std::to_string(counter++);
}

Region RegionManager::CreateRegion(Region& parent, RegionType type,
    const std::string& name, const std::string& groupId) {
    Region newRegion;
    newRegion.id = idGen.GetID(name);
    newRegion.name = name;
    newRegion.type = type;
    newRegion.parent = &parent;
    newRegion.groupId = groupId;
    return newRegion;
}

void RegionManager::CreateLayout() {
    // 创建根区域
    root.id = "root";
    root.name = "Root";
    root.type = REGION_ROOT;
    root.parent = nullptr;

    // 第一行
    Region row1 = CreateRegion(root, REGION_ROW, "Row1");
    root.children.push_back(row1);

    // 第一行的叶子区域
    Region& row1Ref = root.children[0];
    row1Ref.children.push_back(CreateRegion(row1Ref, REGION_LEAF, "A1"));
    row1Ref.children.push_back(CreateRegion(row1Ref, REGION_LEAF, "A2"));

    // 第二行
    Region row2 = CreateRegion(root, REGION_ROW, "Row2");
    root.children.push_back(row2);

    // A1组
    Region groupA1 = CreateRegion(row2, REGION_GROUP, "A1 Group", "A1");
    Region& row2Ref = root.children[1];
    row2Ref.children.push_back(groupA1);

    // A1组的叶子区域
    Region& groupA1Ref = row2Ref.children[0];
    groupA1Ref.children.push_back(CreateRegion(groupA1Ref, REGION_LEAF, "A1B1", "A1"));
    groupA1Ref.children.push_back(CreateRegion(groupA1Ref, REGION_LEAF, "A1B2", "A1"));

    // A2组
    Region groupA2 = CreateRegion(row2Ref, REGION_GROUP, "A2 Group", "A2");
    row2Ref.children.push_back(groupA2);

    // A2组的叶子区域
    Region& groupA2Ref = row2Ref.children[1];
    groupA2Ref.children.push_back(CreateRegion(groupA2Ref, REGION_LEAF, "A2B1", "A2"));
    groupA2Ref.children.push_back(CreateRegion(groupA2Ref, REGION_LEAF, "A2B2", "A2"));
    groupA2Ref.children.push_back(CreateRegion(groupA2Ref, REGION_LEAF, "A2B3", "A2"));
}

void RegionManager::UpdateLayout(Region& region, const ImVec2& pos, const ImVec2& size) {
    region.pos = pos;
    region.size = size;

    if (region.children.empty()) return;

    // 根据区域类型决定布局方式
    if (region.type == REGION_ROW) {
        // 行布局：水平分割
        const float childWidth = size.x / region.children.size();
        for (size_t i = 0; i < region.children.size(); i++) {
            Region& child = region.children[i];
            UpdateLayout(child,
                { pos.x + i * childWidth, pos.y },
                { childWidth, size.y });
        }
    }
    else if (region.type == REGION_GROUP) {
        // 组布局：水平分割子区域
        const float childWidth = size.x / region.children.size();
        for (size_t i = 0; i < region.children.size(); i++) {
            Region& child = region.children[i];
            UpdateLayout(child,
                { pos.x + i * childWidth, pos.y },
                { childWidth, size.y });
        }
    }
    else if (region.type == REGION_ROOT) {
        // 根布局：垂直分割行
        const float rowHeight = size.y / region.children.size();
        for (size_t i = 0; i < region.children.size(); i++) {
            Region& child = region.children[i];
            UpdateLayout(child,
                { pos.x, pos.y + i * rowHeight },
                { size.x, rowHeight });
        }
    }
}

void RegionManager::DrawRegion(Region& region) {
    // 跳过根区域
    if (region.type == REGION_ROOT) {
        for (auto& child : region.children) {
            DrawRegion(child);
        }
        return;
    }

    // 确保ID作用域唯一
    ImGui::PushID(region.id.c_str());

    // 计算颜色
    ImColor color;
    if (region.state.isFocused) {
        color = ImColor(1.0f, 0.7f, 0.4f, 1.0f); // 焦点色: 淡橘色
    }
    else if (region.state.isHovered) {
        color = ImColor(0.95f, 0.95f, 0.95f, 1.0f); // 悬停色: 浅灰色
    }
    else {
        color = ImColor(0.92f, 0.92f, 0.92f, 1.0f); // 默认色: 接近白色的灰色
    }

    // 绘制区域背景
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(region.pos,
        { region.pos.x + region.size.x, region.pos.y + region.size.y },
        color);

    // 绘制边框
    drawList->AddRect(region.pos,
        { region.pos.x + region.size.x, region.pos.y + region.size.y },
        ImColor(0.7f, 0.7f, 0.7f, 1.0f),
        0.0f, 0, 1.5f);

    // 绘制区域名称
    if (!region.name.empty()) {
        ImVec2 textSize = ImGui::CalcTextSize(region.name.c_str());
        float scale = std::min(1.0f,
            std::min((region.size.x - 10.0f) / textSize.x,
                (region.size.y - 10.0f) / textSize.y));

        ImVec2 textPos(
            region.pos.x + (region.size.x - textSize.x * scale) * 0.5f,
            region.pos.y + (region.size.y - ImGui::GetTextLineHeight() * scale) * 0.5f
        );

        // 绘制文本
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::SetWindowFontScale(scale);
        drawList->AddText(textPos, ImColor(0.1f, 0.1f, 0.1f, 1.0f), region.name.c_str());
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopFont();
    }

    if (region.IsLeaf()) {
        // 添加交互
        ImGui::SetCursorScreenPos(region.pos);
        ImGui::InvisibleButton(region.id.c_str(), region.size);

        // 悬停检测
        region.state.isHovered = ImGui::IsItemHovered();

        // 点击处理
        if (ImGui::IsItemClicked()) {
            // 处理点击事件
            OnRegionClicked(region);
        }
    }

   

    // 递归绘制子区域
    for (auto& child : region.children) {
        DrawRegion(child);
    }

    ImGui::PopID(); // 结束ID作用域
}

void RegionManager::OnRegionClicked(Region& region) {
    // 输出点击信息
    std::cout << "Clicked region: " << region.name << std::endl;

    // 清除所有焦点
    ClearAllFocus(root);

    // 设置当前区域为焦点
    region.state.isFocused = true;

    // 处理组关系
    if (!region.groupId.empty()) {
        // 设置组焦点
        SetGroupFocus(region.groupId);
    }

    // 处理行关系
    if (region.type == REGION_LEAF && region.parent && region.parent->type == REGION_ROW) {
        // 设置行焦点
        SetRowFocus(region.parent->id);
    }
}

void RegionManager::ClearAllFocus(Region& region) {
    region.state.isFocused = false;
    for (auto& child : region.children) {
        ClearAllFocus(child);
    }
}

void RegionManager::SetGroupFocus(const std::string& groupId) {
    // 遍历所有区域，找到匹配的组ID
    ForEachRegion(root, [&](Region& r) {
        if (r.groupId == groupId && r.type == REGION_LEAF) {
            r.state.isFocused = true;
        }
        });
}

void RegionManager::SetRowFocus(const std::string& rowId) {
    // 遍历所有区域，找到匹配的行ID
    ForEachRegion(root, [&](Region& r) {
        if (r.parent && r.parent->id == rowId && r.type == REGION_LEAF) {
            r.state.isFocused = true;
        }
        });
}

void RegionManager::ForEachRegion(Region& region, const std::function<void(Region&)>& func) {
    func(region);
    for (auto& child : region.children) {
        ForEachRegion(child, func);
    }
}

RegionManager::RegionManager() {
    CreateLayout();
}

void RegionManager::ReloadConfig() {
    root = Region();
    root.children.clear();
    CreateLayout();
}

void RegionManager::DrawUI() {
    // 获取窗口内容区域
    ImVec2 contentPos = ImGui::GetCursorScreenPos();
    ImVec2 contentSize = ImGui::GetContentRegionAvail();

    // 设置最小区域尺寸
    if (contentSize.x < 400) contentSize.x = 400;
    if (contentSize.y < 300) contentSize.y = 300;

    // 更新布局
    UpdateLayout(root, contentPos, contentSize);

    // 绘制所有区域
    DrawRegion(root);
}