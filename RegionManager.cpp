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
    // ����������
    root.id = "root";
    root.name = "Root";
    root.type = REGION_ROOT;
    root.parent = nullptr;

    // ��һ��
    Region row1 = CreateRegion(root, REGION_ROW, "Row1");
    root.children.push_back(row1);

    // ��һ�е�Ҷ������
    Region& row1Ref = root.children[0];
    row1Ref.children.push_back(CreateRegion(row1Ref, REGION_LEAF, "A1"));
    row1Ref.children.push_back(CreateRegion(row1Ref, REGION_LEAF, "A2"));

    // �ڶ���
    Region row2 = CreateRegion(root, REGION_ROW, "Row2");
    root.children.push_back(row2);

    // A1��
    Region groupA1 = CreateRegion(row2, REGION_GROUP, "A1 Group", "A1");
    Region& row2Ref = root.children[1];
    row2Ref.children.push_back(groupA1);

    // A1���Ҷ������
    Region& groupA1Ref = row2Ref.children[0];
    groupA1Ref.children.push_back(CreateRegion(groupA1Ref, REGION_LEAF, "A1B1", "A1"));
    groupA1Ref.children.push_back(CreateRegion(groupA1Ref, REGION_LEAF, "A1B2", "A1"));

    // A2��
    Region groupA2 = CreateRegion(row2Ref, REGION_GROUP, "A2 Group", "A2");
    row2Ref.children.push_back(groupA2);

    // A2���Ҷ������
    Region& groupA2Ref = row2Ref.children[1];
    groupA2Ref.children.push_back(CreateRegion(groupA2Ref, REGION_LEAF, "A2B1", "A2"));
    groupA2Ref.children.push_back(CreateRegion(groupA2Ref, REGION_LEAF, "A2B2", "A2"));
    groupA2Ref.children.push_back(CreateRegion(groupA2Ref, REGION_LEAF, "A2B3", "A2"));
}

void RegionManager::UpdateLayout(Region& region, const ImVec2& pos, const ImVec2& size) {
    region.pos = pos;
    region.size = size;

    if (region.children.empty()) return;

    // �����������;������ַ�ʽ
    if (region.type == REGION_ROW) {
        // �в��֣�ˮƽ�ָ�
        const float childWidth = size.x / region.children.size();
        for (size_t i = 0; i < region.children.size(); i++) {
            Region& child = region.children[i];
            UpdateLayout(child,
                { pos.x + i * childWidth, pos.y },
                { childWidth, size.y });
        }
    }
    else if (region.type == REGION_GROUP) {
        // �鲼�֣�ˮƽ�ָ�������
        const float childWidth = size.x / region.children.size();
        for (size_t i = 0; i < region.children.size(); i++) {
            Region& child = region.children[i];
            UpdateLayout(child,
                { pos.x + i * childWidth, pos.y },
                { childWidth, size.y });
        }
    }
    else if (region.type == REGION_ROOT) {
        // �����֣���ֱ�ָ���
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
    // ����������
    if (region.type == REGION_ROOT) {
        for (auto& child : region.children) {
            DrawRegion(child);
        }
        return;
    }

    // ȷ��ID������Ψһ
    ImGui::PushID(region.id.c_str());

    // ������ɫ
    ImColor color;
    if (region.state.isFocused) {
        color = ImColor(1.0f, 0.7f, 0.4f, 1.0f); // ����ɫ: ����ɫ
    }
    else if (region.state.isHovered) {
        color = ImColor(0.95f, 0.95f, 0.95f, 1.0f); // ��ͣɫ: ǳ��ɫ
    }
    else {
        color = ImColor(0.92f, 0.92f, 0.92f, 1.0f); // Ĭ��ɫ: �ӽ���ɫ�Ļ�ɫ
    }

    // �������򱳾�
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(region.pos,
        { region.pos.x + region.size.x, region.pos.y + region.size.y },
        color);

    // ���Ʊ߿�
    drawList->AddRect(region.pos,
        { region.pos.x + region.size.x, region.pos.y + region.size.y },
        ImColor(0.7f, 0.7f, 0.7f, 1.0f),
        0.0f, 0, 1.5f);

    // ������������
    if (!region.name.empty()) {
        ImVec2 textSize = ImGui::CalcTextSize(region.name.c_str());
        float scale = std::min(1.0f,
            std::min((region.size.x - 10.0f) / textSize.x,
                (region.size.y - 10.0f) / textSize.y));

        ImVec2 textPos(
            region.pos.x + (region.size.x - textSize.x * scale) * 0.5f,
            region.pos.y + (region.size.y - ImGui::GetTextLineHeight() * scale) * 0.5f
        );

        // �����ı�
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::SetWindowFontScale(scale);
        drawList->AddText(textPos, ImColor(0.1f, 0.1f, 0.1f, 1.0f), region.name.c_str());
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopFont();
    }

    if (region.IsLeaf()) {
        // ��ӽ���
        ImGui::SetCursorScreenPos(region.pos);
        ImGui::InvisibleButton(region.id.c_str(), region.size);

        // ��ͣ���
        region.state.isHovered = ImGui::IsItemHovered();

        // �������
        if (ImGui::IsItemClicked()) {
            // �������¼�
            OnRegionClicked(region);
        }
    }

   

    // �ݹ����������
    for (auto& child : region.children) {
        DrawRegion(child);
    }

    ImGui::PopID(); // ����ID������
}

void RegionManager::OnRegionClicked(Region& region) {
    // ��������Ϣ
    std::cout << "Clicked region: " << region.name << std::endl;

    // ������н���
    ClearAllFocus(root);

    // ���õ�ǰ����Ϊ����
    region.state.isFocused = true;

    // �������ϵ
    if (!region.groupId.empty()) {
        // �����齹��
        SetGroupFocus(region.groupId);
    }

    // �����й�ϵ
    if (region.type == REGION_LEAF && region.parent && region.parent->type == REGION_ROW) {
        // �����н���
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
    // �������������ҵ�ƥ�����ID
    ForEachRegion(root, [&](Region& r) {
        if (r.groupId == groupId && r.type == REGION_LEAF) {
            r.state.isFocused = true;
        }
        });
}

void RegionManager::SetRowFocus(const std::string& rowId) {
    // �������������ҵ�ƥ�����ID
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
    // ��ȡ������������
    ImVec2 contentPos = ImGui::GetCursorScreenPos();
    ImVec2 contentSize = ImGui::GetContentRegionAvail();

    // ������С����ߴ�
    if (contentSize.x < 400) contentSize.x = 400;
    if (contentSize.y < 300) contentSize.y = 300;

    // ���²���
    UpdateLayout(root, contentPos, contentSize);

    // ������������
    DrawRegion(root);
}