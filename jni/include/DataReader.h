#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>

struct CustomItemInfo {
    std::string displayName;
    ImColor color = ImColor(255, 215, 0, 255);
    float fontSize = 23.0f;
};

class DataReader {
private:
    std::unordered_map<std::string, CustomItemInfo> dataMap;

    ImColor parseColor(const std::string& colorStr) const {
        if (colorStr.empty()) return ImColor(255, 215, 0, 255);
        int r = 255, g = 215, b = 0, a = 255;
        if (std::sscanf(colorStr.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a) >= 3) {
            return ImColor(r, g, b, a);
        }
        printf("[DataReader] 颜色解析失败: %s\n", colorStr.c_str());
        return ImColor(255, 215, 0, 255); // 容错默认色
    }

public:
    bool loadDataFromFile(const std::string& filePath) {
        dataMap.clear();

        printf("[DataReader] 正在尝试打开文件: %s\n", filePath.c_str());
        std::ifstream file(filePath);
        if (!file.is_open()) {
            printf("[DataReader] ❌ 文件打开失败！请检查路径和权限\n");
            return false;
        }

        std::string line;
        int lineCount = 0;
        bool hasValidData = false;

        while (std::getline(file, line)) {
            lineCount++;
            if (line.empty()) continue;

            // 使用 '@' 分割，同时去除首尾空白
            std::vector<std::string> parts;
            std::string segment;
            std::istringstream ss(line);
            while (std::getline(ss, segment, '@')) {
                // 去除首尾空格
                const auto start = segment.find_first_not_of(" \t");
                const auto end   = segment.find_last_not_of(" \t");
                if (start != std::string::npos)
                    parts.push_back(segment.substr(start, end - start + 1));
                else
                    parts.push_back("");
            }

            if (parts.size() < 2) {
                printf("[DataReader] 第 %d 行格式错误 (至少需要 类名@显示名称) : %s\n", lineCount, line.c_str());
                continue;
            }

            std::string className = parts[0];
            CustomItemInfo info;
            info.displayName = parts[1];

            if (parts.size() >= 3 && !parts[2].empty()) {
                info.color = parseColor(parts[2]);
            }
            if (parts.size() >= 4 && !parts[3].empty()) {
                info.fontSize = std::stof(parts[3]);
            }

            dataMap[className] = info;
            hasValidData = true;
            printf("[DataReader] 加载成功: %s -> %s\n", className.c_str(), info.displayName.c_str());
        }

        if (hasValidData) {
            printf("[DataReader] ✅ 共加载 %zu 条自定义物资\n", dataMap.size());
            return true;
        } else {
            printf("[DataReader] ⚠️ 文件中没有有效数据，请检查格式\n");
            return false;
        }
    }

    const CustomItemInfo* getItemInfo(const std::string& className) const {
        auto it = dataMap.find(className);
        return (it != dataMap.end()) ? &(it->second) : nullptr;
    }

    std::string getDisplayName(const std::string& className) const {
        auto* info = getItemInfo(className);
        return info ? info->displayName : "";
    }

    void clear() { dataMap.clear(); }
};