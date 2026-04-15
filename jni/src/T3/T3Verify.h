#include "T3Api.h"
#include "Colors.h"
#include "T3Config.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
// 新增头文件，必须加
#include <mutex>

// 现有 static 全局变量（保持不变）
static T3Api* g_t3api = nullptr;
static std::string g_kami;
static std::string g_kami_save_file;

// 新增全局变量（不加 static，允许跨文件访问）
std::string g_globalExpireTime = "未知";
std::mutex g_expireTimeMutex;

// 新增全局函数（不加 static）
void UpdateGlobalExpireTime(const std::string& newTime) {
    std::lock_guard<std::mutex> lock(g_expireTimeMutex);
    g_globalExpireTime = newTime;
}

/**
 * 获取程序名称（从 /proc/self/cmdline 读取）
 * @return 程序名称，如果获取失败返回默认值
 */
inline std::string getProgramName() {
    std::ifstream cmdline("/proc/self/cmdline");
    if (cmdline.is_open()) {
        std::string line;
        std::getline(cmdline, line, '\0');  // 读取第一个字段（程序名）
        cmdline.close();
        
        if (!line.empty()) {
            // 提取文件名部分（去除路径）
            size_t lastSlash = line.find_last_of('/');
            if (lastSlash != std::string::npos) {
                return line.substr(lastSlash + 1);
            }
            return line;
        }
    }
    // 如果无法获取，返回默认值
    return "t3_kami_save";
}

/**
 * 获取基于程序名的唯一卡密保存文件路径
 * @return 卡密保存文件路径
 */
inline std::string getKamiSaveFilePath() {
    if (g_kami_save_file.empty()) {
        std::string progName = getProgramName();
        // 生成文件名：.t3_kami_<程序名>
        g_kami_save_file = ".t3_kami_" + progName;
    }
    return g_kami_save_file;
}

/**
 * 保存卡密到文件
 * @param kami 要保存的卡密
 * @return true 保存成功，false 保存失败
 */
inline bool saveKamiToFile(const std::string& kami) {
    if (kami.empty()) return false;
    std::string filePath = getKamiSaveFilePath();
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << kami;
        file.close();
        return true;
    }
    return false;
}

/**
 * 从文件读取卡密
 * @return 读取到的卡密，如果读取失败返回空字符串
 */
inline std::string loadKamiFromFile() {
    std::string filePath = getKamiSaveFilePath();
    std::ifstream file(filePath);
    if (file.is_open()) {
        std::string kami;
        std::getline(file, kami);
        file.close();
        return kami;
    }
    return "";
}

/**
 * T3验证函数 - 用户只需调用此函数即可完成所有验证
 * @return true 验证成功，可以继续执行程序；false 验证失败，应该退出程序
 */
inline bool T3Verify() {
    // 初始化API实例
    if (!g_t3api) {
        g_t3api = new T3Api();
    }
    
    // 显示ASCII艺术标识
//void display_banner() {
    printf("%s\n","  ____   __  __ __   __  _   _   _   _ ");
    printf("%s\n"," | __ )  \\ \\/ / \\ \\ / / | | | | | \\ | |");
    printf("%s\n"," |  _ \\   \\  /   \\ V /  | | | | |  \\| |");
    printf("%s\n"," | |_) |  /  \\    | |   | |_| | | |\\  |");
    printf("%s\n"," |____/  /_/\\_\\   |_|    \\___/  |_| \\_|");
    printf("%s\n","                                       ");

    printf("%s\n","        官方QQ[3 2 4 2 2 0 7 9 7]                                         ");
    printf("%s\n","                                                 ");

    // 显示标题
    std::cout << std::endl;
    std::cout << Colors::TITLE << "欢迎使用 BXYUN PRO" << Colors::RESET << std::endl;
    std::cout << std::endl;

    // 后续验证逻辑...
   

    
    // 获取设备信息
    std::string machine_code = T3Api::getDeviceCode();
    std::cout << std::endl;
    
    // 版本检查
    printSection("检查版本");
    std::string serverVer = g_t3api->getServerVersion();
    if (!serverVer.empty()) {
        if (g_t3api->checkVersion(T3Config::LocalVersion)) {
            printSuccess("版本正常");
        } else {
            printWarning("版本过低: 本地" + T3Config::LocalVersion + " / 服务器" + serverVer);
            std::cout << std::endl;
            if (T3Config::ExitOnVersionMismatch) {
                return false;  // 版本检查失败，退出程序
            }
        }
    } else {
        // 如果获取版本失败，根据配置决定是否退出
        if (T3Config::ExitOnVersionMismatch) {
            printError("无法获取服务器版本");
            return false;
        }
    }
    std::cout << std::endl;
    
    // 公告
    printSection("系统公告");
    std::string notice = g_t3api->getNotice();
    if (!notice.empty()) {
        std::cout << Colors::CYAN << "  " << notice << Colors::RESET << std::endl;
    } else {
        // 如果获取公告失败，根据配置决定是否退出
        if (T3Config::ExitOnNoticeFail) {
            printError("无法获取系统公告");
            return false;
        }
    }
    std::cout << std::endl;
    
    // 登录
    printTitle("单码卡密登录");
    
    // 尝试从文件读取保存的卡密
    std::string savedKami = loadKamiFromFile();
    std::string kami;
    
    // 如果有保存的卡密，询问用户是否使用
    if (!savedKami.empty()) {
        std::cout << Colors::CYAN << "  检测到已保存的卡密" << Colors::RESET << std::endl;
        std::cout << Colors::YELLOW << "  回车使用上次卡密，输入任意内容则重新输入卡密: " << Colors::RESET;
        std::string choice;
        std::getline(std::cin, choice);
        std::cout << std::endl;
        
        // 如果输入是0或空（回车），使用保存的卡密
        if (choice.empty()) {
            kami = savedKami;
            std::cout << Colors::GREEN << "  使用保存的卡密" << Colors::RESET << std::endl;
        } else {
            // 否则要求重新输入
            std::cout << Colors::YELLOW << "请输入您的卡密: " << Colors::RESET;
            std::getline(std::cin, kami);
            std::cout << std::endl;
        }
    } else {
        // 没有保存的卡密，直接要求输入
        std::cout << Colors::YELLOW << "请输入您的卡密: " << Colors::RESET;
        std::getline(std::cin, kami);
        std::cout << std::endl;
    }
    
    // 循环登录，直到成功
    while (true) {
        if (kami.empty()) {
            printError("卡密不能为空");
            std::cout << Colors::YELLOW << "请输入您的卡密: " << Colors::RESET;
            std::getline(std::cin, kami);
            std::cout << std::endl;
            continue;
        }
        
        std::string errorMsg;
// ===== 1. 登录逻辑中更新时间 =====
 if (g_t3api->singleLogin(kami, machine_code, &errorMsg)) {
     printSuccess("登录成功");
     std::string expireTime = g_t3api->getExpireTime();
     std::cout << Colors::BLUE << "  " << Colors::ICON_INFO << " 到期时间: " << Colors::WHITE << expireTime << Colors::RESET << std::endl;
     std::cout << std::endl;
     // 直接更新全局变量（核心修改，替代文件写入）
     UpdateGlobalExpireTime(expireTime);
     // 下面的原有逻辑保持不变
     std::string value = g_t3api->getValueContent(kami, T3Config::ValueId, T3Config::ValueName);
     if (!value.empty()) {
         std::cout << Colors::CYAN << "  变量内容: " << value << Colors::RESET << std::endl;
     }
     if (T3Config::EnableHeartbeat) {
         g_t3api->startHeartbeat(T3Config::HeartbeatInterval, T3Config::HeartbeatMaxFail);
     }
     g_kami = kami;
     saveKamiToFile(kami);
     return true;
 } else {
     // 登录失败逻辑不变
     printError("登录失败: " + errorMsg);
     std::cout << Colors::YELLOW << "请输入您的卡密: " << Colors::RESET;
     std::getline(std::cin, kami);
     std::cout << std::endl;
 }
    }
}

/**
 * 获取系统公告
 * @return 公告内容，如果获取失败返回空字符串
 */
inline std::string T3GetNotice() {
    if (!g_t3api) return "";
    return g_t3api->getNotice();
}

/**
 * 获取到期时间
 * @return 到期时间字符串，如果未登录返回空字符串
 */
inline std::string T3GetExpireTime() {
    if (!g_t3api) return "";
    return g_t3api->getExpireTime();
}

/**
 * 获取变量内容（使用默认配置）
 * @return 变量内容，如果获取失败返回空字符串
 */
inline std::string T3GetValue() {
    if (!g_t3api || g_kami.empty()) return "";
    return g_t3api->getValueContent(g_kami, T3Config::ValueId, T3Config::ValueName);
}

/**
 * 获取变量内容（指定变量ID和名称）
 * @param valueId 变量ID
 * @param valueName 变量名称
 * @return 变量内容，如果获取失败返回空字符串
 */
inline std::string T3GetValue(const std::string& valueId, const std::string& valueName) {
    if (!g_t3api || g_kami.empty()) return "";
    return g_t3api->getValueContent(g_kami, valueId, valueName);
}

/**
 * 获取服务器版本号
 * @return 服务器版本号，如果获取失败返回空字符串
 */
inline std::string T3GetServerVersion() {
    if (!g_t3api) return "";
    return g_t3api->getServerVersion();
}

/**
 * 检查版本（使用默认版本号）
 * @return true 版本正常，false 版本过低
 */
inline bool T3CheckVersion() {
    if (!g_t3api) return false;
    return g_t3api->checkVersion(T3Config::LocalVersion);
}

/**
 * 检查版本（指定版本号）
 * @param localVersion 本地版本号
 * @return true 版本正常，false 版本过低
 */
inline bool T3CheckVersion(const std::string& localVersion) {
    if (!g_t3api) return false;
    return g_t3api->checkVersion(localVersion);
}

