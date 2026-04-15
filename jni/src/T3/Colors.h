#pragma once
#include <string>

/**
 * 终端颜色定义
 * 使用现代、明亮的配色方案
 */
namespace Colors {
    // 基础颜色
    const std::string RESET     = "\033[0m";
    const std::string BOLD      = "\033[1m";
    
    // 明亮主题色
    const std::string CYAN      = "\033[96m";   // 亮青色 - 用于标题、重要信息
    const std::string GREEN     = "\033[92m";   // 亮绿色 - 用于成功消息
    const std::string YELLOW    = "\033[93m";   // 亮黄色 - 用于警告、提示
    const std::string RED       = "\033[91m";   // 亮红色 - 用于错误
    const std::string MAGENTA   = "\033[95m";   // 亮洋红 - 用于特殊标记
    const std::string BLUE      = "\033[94m";   // 亮蓝色 - 用于信息
    const std::string WHITE     = "\033[97m";   // 亮白色 - 用于普通文本
    
    // 组合样式
    const std::string SUCCESS   = "\033[1;92m"; // 加粗亮绿色
    const std::string ERROR     = "\033[1;91m"; // 加粗亮红色
    const std::string WARNING   = "\033[1;93m"; // 加粗亮黄色
    const std::string INFO      = "\033[1;96m"; // 加粗亮青色
    const std::string TITLE     = "\033[1;95m"; // 加粗亮洋红
    
    // 图标符号
    const std::string ICON_SUCCESS  = "✓";
    const std::string ICON_ERROR    = "✗";
    const std::string ICON_WARNING  = "⚠";
    const std::string ICON_INFO     = "ℹ";
    const std::string ICON_ARROW    = "►";
    const std::string ICON_DOT      = "●";
    const std::string ICON_STAR     = "★";
}

// 便捷的输出函数
inline void printSuccess(const std::string& msg) {
    std::cout << Colors::SUCCESS << Colors::ICON_SUCCESS << " " << msg << Colors::RESET << std::endl;
}

inline void printError(const std::string& msg) {
    std::cout << Colors::ERROR << Colors::ICON_ERROR << " " << msg << Colors::RESET << std::endl;
}

inline void printWarning(const std::string& msg) {
    std::cout << Colors::WARNING << Colors::ICON_WARNING << " " << msg << Colors::RESET << std::endl;
}

inline void printInfo(const std::string& msg) {
    std::cout << Colors::INFO << Colors::ICON_INFO << " " << msg << Colors::RESET << std::endl;
}

inline void printTitle(const std::string& msg) {
    std::cout << std::endl;
    std::cout << Colors::TITLE << Colors::ICON_STAR << " " << msg << Colors::RESET << std::endl;
}

inline void printSection(const std::string& msg) {
    std::cout << std::endl;
    std::cout << Colors::CYAN << Colors::ICON_ARROW << " " << msg << Colors::RESET << std::endl;
}

