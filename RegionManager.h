#pragma once
#pragma once

#include <vector>
#include <string>
#include <functional>
#include "imgui.h"

// 区域类型
enum RegionType {
    REGION_ROOT,    // 根区域
    REGION_ROW,     // 行区域
    REGION_GROUP,   // 组区域
    REGION_LEAF     // 叶子区域
};

// 区域状态
struct RegionState {
    bool isHovered = false;
    bool isFocused = false;
};

// 区域结构
struct Region {
    std::string id;          // 唯一标识符
    std::string name;        // 区域名称
    RegionType type;         // 区域类型
    ImVec2 pos;              // 位置
    ImVec2 size;             // 大小
    RegionState state;       // 状态
    std::vector<Region> children; // 子区域
    Region* parent = nullptr;    // 父区域指针
    std::string groupId;     // 所属组ID

    // 判断是否是叶子节点
    bool IsLeaf() const;

    // 查找区域
    Region* FindRegion(const std::string& targetId);
};

// 唯一ID生成器
class IDGenerator {
private:
    int counter = 0;
public:
    std::string GetID(const std::string& prefix);
};
// 区域管理器
class RegionManager {
private:
    Region root;  // 根区域
    IDGenerator idGen; // ID生成器

    // 创建区域辅助函数
    Region CreateRegion(Region& parent, RegionType type,
        const std::string& name, const std::string& groupId = "");

    // 核心函数
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

