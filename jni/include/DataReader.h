#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

struct CustomItemInfo {
    std::string displayName;       // 显示名称
    ImColor color = ImColor(255, 215, 0, 255); // 默认金色
    float fontSize = 23.0f;        // 默认字体大小
};

class DataReader {
private:
    std::unordered_map<std::string, CustomItemInfo> dataMap;
    std::vector<char> dataBuffer;

    // 辅助函数：解析颜色字符串 "R,G,B,A"
    ImColor parseColor(const std::string& colorStr) {
        int r = 255, g = 215, b = 0, a = 255;
        std::sscanf(colorStr.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a);
        return ImColor(r, g, b, a);
    }

public:
    bool loadDataFromFile(const std::string& filePath) {
        dataMap.clear();
        dataBuffer.clear();

        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        // 预估缓冲区大小
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        dataBuffer.reserve(fileSize);

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            // 按 '@' 分割
            std::vector<std::string> parts;
            size_t start = 0, end = 0;
            while ((end = line.find('@', start)) != std::string::npos) {
                parts.push_back(line.substr(start, end - start));
                start = end + 1;
            }
            parts.push_back(line.substr(start)); // 最后一段

            if (parts.size() < 2) continue;      // 至少要有类名和显示名称

            std::string className = parts[0];
            CustomItemInfo info;
            info.displayName = parts[1];

            if (parts.size() >= 3) {
                info.color = parseColor(parts[2]);
            }
            if (parts.size() >= 4) {
                info.fontSize = std::stof(parts[3]);
            }

            dataMap[className] = info;

            // 将字符串存入缓冲区（保持引用有效）
            size_t bufStart = dataBuffer.size();
            dataBuffer.insert(dataBuffer.end(), line.begin(), line.end());
        }
        return true;
    }

    // 根据类名获取完整信息
    const CustomItemInfo* getItemInfo(const std::string& className) const {
        auto it = dataMap.find(className);
        return (it != dataMap.end()) ? &(it->second) : nullptr;
    }

    // 简便方法：只获取显示名称
    std::string getDisplayName(const std::string& className) const {
        auto* info = getItemInfo(className);
        return info ? info->displayName : "";
    }
};