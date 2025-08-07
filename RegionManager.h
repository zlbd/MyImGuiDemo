#pragma once
#pragma once

#include <vector>
#include <string>
#include <functional>
#include "imgui.h"

// ��������
enum RegionType {
    REGION_ROOT,    // ������
    REGION_ROW,     // ������
    REGION_GROUP,   // ������
    REGION_LEAF     // Ҷ������
};

// ����״̬
struct RegionState {
    bool isHovered = false;
    bool isFocused = false;
};

// ����ṹ
struct Region {
    std::string id;          // Ψһ��ʶ��
    std::string name;        // ��������
    RegionType type;         // ��������
    ImVec2 pos;              // λ��
    ImVec2 size;             // ��С
    RegionState state;       // ״̬
    std::vector<Region> children; // ������
    Region* parent = nullptr;    // ������ָ��
    std::string groupId;     // ������ID

    // �ж��Ƿ���Ҷ�ӽڵ�
    bool IsLeaf() const;

    // ��������
    Region* FindRegion(const std::string& targetId);
};

// ΨһID������
class IDGenerator {
private:
    int counter = 0;
public:
    std::string GetID(const std::string& prefix);
};
// ���������
class RegionManager {
private:
    Region root;  // ������
    IDGenerator idGen; // ID������

    // ��������������
    Region CreateRegion(Region& parent, RegionType type,
        const std::string& name, const std::string& groupId = "");

    // ���ĺ���
    void CreateLayout();
    void UpdateLayout(Region& region, const ImVec2& pos, const ImVec2& size);
    void DrawRegion(Region& region);
    void OnRegionClicked(Region& region);
    void ClearAllFocus(Region& region);
    void SetGroupFocus(const std::string& groupId);
    void SetRowFocus(const std::string& rowId);
    void ForEachRegion(Region& region, const std::function<void(Region&)>& func);

public:
    RegionManager();
    void ReloadConfig();
    void DrawUI();
};

