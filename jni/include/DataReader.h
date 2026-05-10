#pragma once
#include <atomic>
#include <unordered_map>
#include <string>
#include <fstream>
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
        return ImColor(255, 215, 0, 255);
    }

    // 使用 find 快速分割
    static std::vector<std::string> split(const std::string& s, char delim) {
        std::vector<std::string> parts;
        size_t start = 0, end;
        while ((end = s.find(delim, start)) != std::string::npos) {
            parts.push_back(s.substr(start, end - start));
            start = end + 1;
        }
        parts.push_back(s.substr(start));
        return parts;
    }

    // 去除首尾空白
    static std::string trim(const std::string& s) {
        const char* whitespace = " \t\r\n";
        auto start = s.find_first_not_of(whitespace);
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(whitespace);
        return s.substr(start, end - start + 1);
    }

public:
    bool loadDataFromFile(const std::string& filePath) {
        dataMap.clear();

        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        std::string line;
        int lineCount = 0;
        bool hasValidData = false;

        // 可选预留空间（如果文件通常有几百条）
        dataMap.reserve(200);

        while (std::getline(file, line)) {
            lineCount++;
            if (line.empty()) continue;

            auto parts = split(line, '@');
            if (parts.size() < 2) continue;

            std::string className = trim(parts[0]);
            if (className.empty()) continue;

            CustomItemInfo info;
            info.displayName = trim(parts[1]);

            if (parts.size() >= 3) {
                std::string colorPart = trim(parts[2]);
                if (!colorPart.empty()) {
                    info.color = parseColor(colorPart);
                }
            }
            if (parts.size() >= 4) {
                std::string fontSizePart = trim(parts[3]);
                if (!fontSizePart.empty()) {
                    info.fontSize = std::stof(fontSizePart);
                }
            }

            dataMap[className] = info;
            hasValidData = true;
        }

        return hasValidData;
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

// 2. 添加 extern 声明
extern DataReader* g_CustomReader;
extern std::atomic<bool> g_CustomDataLoaded;