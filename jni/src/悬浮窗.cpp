#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>
#include <random> // 添加random头文件
#include <future> // 添加future头文件
#include "json.hpp" // 添加JSON库
#include "imgui.h" 
//本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险
#include "图片调用.h"
#include "辅助类.h"
#include "Utils/Utils.h"
#include "custom.hpp"
#include "imgui_tricks.hpp"
#include <sys/epoll.h>
#include <linux/input.h>
#include <vector>
#include <string>
// 声明为 extern
#include <linux/input.h>
//#include "悬浮窗.cpp"
//#include "悬浮窗动画.h"

static bool 悬浮窗 = true;

// 在你的主文件（main.cpp或其他主文件）中添加
//#include "悬浮窗动画.h"




using json = nlohmann::json; // 添加json命名空间

// 声明在main.cpp中定义的函数
extern std::string getIMEI();
extern std::string dccd5a00d1e7c3717afecb8cf06932b8b(const std::string&);
extern std::string t510d4f87b189757294b61eafde7dcc01(const std::string&, const std::string&);
extern std::string a3b5f410c2d8621923a66cede4bc8ac34(const std::string&);
extern std::string q648fa2f9ee26290c776d0545d10ca5f8(const std::string&);
extern std::string jf9fa77eb23cb2049c9e7f7fae967d770(const std::string&, const std::string&);
extern std::string m367456673340c0cb888250365b54a6ce(const std::string&);
extern std::string httppost(const std::string&, const std::string&, const std::string&);
extern std::string t3ecb948ff5e870506e78160a95a1ec24(const std::string&);
//extern std::string g11bf581d48633826202451738f490782(const std::string&, const std::string&);
extern std::string wef51bd54b4960d4881e233acf0a25f83(const std::string&);
extern std::string p3eca7b0968b8c1535746e24ba2547b6c(const std::string&);
// 修改函数声明：
// 原来的声明（可能）：
//extern std::string g11bf581d48633826202451738f490782(const std::string&, const std::string&);

// 修改为与main.cpp中一致的声明：
extern std::string g11bf581d48633826202451738f490782(const std::string&, const std::string);

// 注意第二个参数没有const&，这与错误信息显示的一致
enum UITheme {
    THEME_DEFAULT = 0,  // 默认白色主题
 //   THEME_LAVENDER = 1,  // 薰衣草主题
    THEME_SAKURA = 1,     // 樱花粉主题
};



static int currentTheme = THEME_DEFAULT; // 当前主题
static const char* themeNames[] = {"默认","樱花粉"};

// 应用主题函数
void ApplyTheme(int theme) {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // 通用玻璃效果设置
    style.WindowRounding = 12.0f;       // 增加圆角，更像玻璃
    style.ChildRounding = 10.0f;
    style.FrameRounding = 8.0f;
    style.PopupRounding = 10.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 8.0f;
    
    // 边框设置
    style.WindowBorderSize = 0.0f;      // 我们使用自定义边框
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    
    switch(theme) {
    case THEME_DEFAULT:
        // 将蓝色调改为绿色调：交换 G 和 B 分量，使绿色主导
        style.Colors[ImGuiCol_Text] = ImVec4(0.22f, 0.45f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.22f, 0.45f, 0.35f, 0.58f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.98f, 0.94f, 0.96f, 0.70f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.99f, 0.96f, 0.97f, 0.65f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.94f, 0.96f, 0.75f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.72f, 0.92f, 0.85f, 0.40f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.72f, 0.92f, 0.85f, 0.15f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.92f, 0.95f, 0.98f, 0.60f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.85f, 0.90f, 0.96f, 0.70f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.86f, 0.94f, 0.75f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.90f, 0.93f, 0.80f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.94f, 0.86f, 0.90f, 0.85f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.96f, 0.90f, 0.93f, 0.75f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.96f, 0.90f, 0.93f, 0.75f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.92f, 0.95f, 0.98f, 0.50f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.72f, 0.92f, 0.85f, 0.70f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.65f, 0.88f, 0.78f, 0.80f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.58f, 0.85f, 0.72f, 0.90f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.22f, 0.45f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.72f, 0.92f, 0.85f, 0.80f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.65f, 0.88f, 0.78f, 0.95f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.88f, 0.92f, 0.96f, 0.65f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.94f, 0.88f, 0.92f, 0.75f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.78f, 0.86f, 0.94f, 0.85f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.88f, 0.90f, 0.95f, 0.65f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.80f, 0.85f, 0.92f, 0.75f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.75f, 0.80f, 0.88f, 0.85f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.72f, 0.92f, 0.85f, 0.50f);
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.92f, 0.85f, 0.70f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.72f, 0.92f, 0.85f, 0.90f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.72f, 0.92f, 0.85f, 0.50f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.72f, 0.92f, 0.85f, 0.70f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.72f, 0.92f, 0.85f, 0.90f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.88f, 0.92f, 0.96f, 0.65f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.94f, 0.88f, 0.92f, 0.85f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.78f, 0.86f, 0.94f, 0.90f);
        style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.88f, 0.92f, 0.96f, 0.65f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.78f, 0.86f, 0.94f, 0.75f);
        break;

      /*      
        case THEME_LAVENDER: // 薰衣草主题 - 玻璃效果
            // 文字颜色
            style.Colors[ImGuiCol_Text] = ImVec4(0.25f, 0.15f, 0.35f, 1.00f);
            style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.25f, 0.15f, 0.35f, 0.58f);
            
            // 窗口背景 - 玻璃效果
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.90f, 0.98f, 0.70f);
            style.Colors[ImGuiCol_ChildBg] = ImVec4(0.98f, 0.96f, 1.00f, 0.65f);
            style.Colors[ImGuiCol_PopupBg] = ImVec4(0.95f, 0.90f, 0.98f, 0.75f);
            
            // 边框颜色
            style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.60f, 0.85f, 0.40f);
            style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.70f, 0.60f, 0.85f, 0.15f);
            
            // 框架背景
            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.92f, 0.88f, 0.96f, 0.60f);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.88f, 0.82f, 0.94f, 0.70f);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.82f, 0.74f, 0.90f, 0.75f);
            
            // 标题栏
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.90f, 0.85f, 0.95f, 0.80f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.85f, 0.78f, 0.92f, 0.85f);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.90f, 0.85f, 0.95f, 0.75f);
            
            // 菜单栏
            style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.90f, 0.85f, 0.95f, 0.75f);
            
            // 滚动条
            style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.92f, 0.88f, 0.96f, 0.50f);
            style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.70f, 0.60f, 0.85f, 0.70f);
            style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.60f, 0.50f, 0.75f, 0.80f);
            style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.40f, 0.65f, 0.90f);
            
            // 复选框
            style.Colors[ImGuiCol_CheckMark] = ImVec4(0.25f, 0.15f, 0.35f, 1.00f);
            
            // 滑块
            style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.60f, 0.85f, 0.80f);
            style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.50f, 0.75f, 0.95f);
            
            // 按钮
            style.Colors[ImGuiCol_Button] = ImVec4(0.85f, 0.78f, 0.92f, 0.65f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.78f, 0.70f, 0.88f, 0.75f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.70f, 0.62f, 0.82f, 0.85f);
            
            // 头部
            style.Colors[ImGuiCol_Header] = ImVec4(0.75f, 0.68f, 0.85f, 0.65f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.62f, 0.82f, 0.75f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.65f, 0.55f, 0.78f, 0.85f);
            
            // 分隔符
            style.Colors[ImGuiCol_Separator] = ImVec4(0.70f, 0.60f, 0.85f, 0.50f);
            style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.60f, 0.85f, 0.70f);
            style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.60f, 0.85f, 0.90f);
            
            // 调整手柄
            style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.70f, 0.60f, 0.85f, 0.50f);
            style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.70f, 0.60f, 0.85f, 0.70f);
            style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.70f, 0.60f, 0.85f, 0.90f);
            
            // 标签
            style.Colors[ImGuiCol_Tab] = ImVec4(0.85f, 0.78f, 0.92f, 0.65f);
            style.Colors[ImGuiCol_TabHovered] = ImVec4(0.78f, 0.70f, 0.88f, 0.85f);
            style.Colors[ImGuiCol_TabActive] = ImVec4(0.70f, 0.62f, 0.82f, 0.90f);
            style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.85f, 0.78f, 0.92f, 0.65f);
            style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.70f, 0.62f, 0.82f, 0.75f);
            
            break;*/
            
        case THEME_SAKURA: // 樱花粉主题 - 玻璃效果
            // 文字颜色
            style.Colors[ImGuiCol_Text] = ImVec4(0.35f, 0.15f, 0.25f, 1.00f);
            style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.15f, 0.25f, 0.58f);
            
            // 窗口背景 - 玻璃效果
            style.Colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 0.95f, 0.96f, 0.70f);
            style.Colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 0.98f, 0.99f, 0.65f);
            style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 0.95f, 0.96f, 0.75f);
            
            // 边框颜色
            style.Colors[ImGuiCol_Border] = ImVec4(0.95f, 0.70f, 0.80f, 0.40f);
            style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.95f, 0.70f, 0.80f, 0.15f);
            
            // 框架背景
            style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 0.92f, 0.94f, 0.60f);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.98f, 0.88f, 0.92f, 0.70f);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.96f, 0.82f, 0.88f, 0.75f);
            
            // 标题栏
            style.Colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 0.90f, 0.92f, 0.80f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.98f, 0.85f, 0.88f, 0.85f);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.90f, 0.92f, 0.75f);
            
            // 菜单栏
            style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.00f, 0.90f, 0.92f, 0.75f);
            
            // 滚动条
            style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(1.00f, 0.92f, 0.94f, 0.50f);
            style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.95f, 0.70f, 0.80f, 0.70f);
            style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.85f, 0.60f, 0.70f, 0.80f);
            style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.75f, 0.50f, 0.60f, 0.90f);
            
            // 复选框
            style.Colors[ImGuiCol_CheckMark] = ImVec4(0.35f, 0.15f, 0.25f, 1.00f);
            
            // 滑块
            style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.95f, 0.70f, 0.80f, 0.80f);
            style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.85f, 0.60f, 0.70f, 0.95f);
            
            // 按钮
            style.Colors[ImGuiCol_Button] = ImVec4(1.00f, 0.85f, 0.88f, 0.65f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.98f, 0.78f, 0.84f, 0.75f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.96f, 0.70f, 0.78f, 0.85f);
            
            // 头部
            style.Colors[ImGuiCol_Header] = ImVec4(0.98f, 0.82f, 0.86f, 0.65f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.96f, 0.75f, 0.82f, 0.75f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.94f, 0.68f, 0.78f, 0.85f);
            
            // 分隔符
            style.Colors[ImGuiCol_Separator] = ImVec4(0.95f, 0.70f, 0.80f, 0.50f);
            style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.95f, 0.70f, 0.80f, 0.70f);
            style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.95f, 0.70f, 0.80f, 0.90f);
            
            // 调整手柄
            style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.95f, 0.70f, 0.80f, 0.50f);
            style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.95f, 0.70f, 0.80f, 0.70f);
            style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.95f, 0.70f, 0.80f, 0.90f);
            
            // 标签
            style.Colors[ImGuiCol_Tab] = ImVec4(1.00f, 0.85f, 0.88f, 0.65f);
            style.Colors[ImGuiCol_TabHovered] = ImVec4(0.98f, 0.78f, 0.84f, 0.85f);
            style.Colors[ImGuiCol_TabActive] = ImVec4(0.96f, 0.70f, 0.78f, 0.90f);
            style.Colors[ImGuiCol_TabUnfocused] = ImVec4(1.00f, 0.85f, 0.88f, 0.65f);
            style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.96f, 0.70f, 0.78f, 0.75f);
            
            break;
    }
}


// 提示框结构体
struct Notification {
    std::string message;
    std::chrono::steady_clock::time_point start_time;
    float duration; // 显示时长（秒）
    float alpha;    // 透明度
    bool isSuccess; // true为开启成功(绿色)，false为关闭(红色)
};

static std::vector<Notification> notifications;
static const float NOTIFICATION_DURATION = 2.0f; // 默认显示2秒
// 无痕读取相关变量
static bool 无痕读取开启 = false;
static std::string 备份目录 = "/data/local/tmp/adb_backup/";
static std::string 目标目录 = "/data/adb/";
static int ColorSettings = 1;
extern 绘制 绘制;
//static bool 悬浮窗 = false;
static bool 自瞄控件 = false;
ImVec2 Pos2;
ImVec2 windowSize, windowSize_max;
static bool 窗口状态 = false;
static bool 广角设置 = false;

// 解绑相关变量
static bool 显示解绑窗口 = false;
static char 解绑卡密输入[128] = "";
static bool 解绑确认中 = false;
static std::string 解绑结果消息 = "";






bool showWindow = false; // 控制水印显示的开关


float g_FrameTime = 0.0f; // 计算帧率用
int g_FPS = 0;
int g_FrameCount = 0;
std::chrono::steady_clock::time_point g_LastTime = std::chrono::steady_clock::now();
// （添加到文件开头的全局变量区域）
// 举报查询功能变量







// 必须的头文件，缺一不可
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/input.h>
#include <thread>

// 全局悬浮窗状态（仅定义一次）
bool g_FloatWindow = false;

// 悬浮窗显示/隐藏占位函数
void ShowFloatWindow() {
    printf("悬浮窗显示\n");
}
void HideFloatWindow() {
    printf("悬浮窗隐藏\n");
}

// 音量键监听函数（修复权限日志、按键识别）
void VolumeKeyMonitor() {
    int fdArray[10];
    memset(fdArray, -1, sizeof(fdArray));
    int openCount = 0;
    for (int i = 0; i < 10; i++) {
        char dev[64];
        snprintf(dev, sizeof(dev), "/dev/input/event%d", i);
        fdArray[i] = open(dev, O_RDWR | O_NONBLOCK);
        if (fdArray[i] >= 0) {
            printf("成功打开输入设备: %s\n", dev);
            openCount++;
        } else {
            printf("打开设备失败: %s, 错误码: %d（需root权限）\n", dev, errno);
        }
    }

    if (openCount == 0) {
        printf("未打开任何输入设备，音量键监听失效\n");
        return;
    }

    struct input_event ev;
    while (1) {
        for (int i = 0; i < 10; i++) {
            if (fdArray[i] < 0) continue;

            ssize_t bytes = read(fdArray[i], &ev, sizeof(ev));
            if (bytes != sizeof(ev)) continue;

            // 处理音量键
         if (ev.type == EV_KEY && ev.value == 1) {
                if (ev.code == KEY_VOLUMEUP || ev.code == 115) {
                    g_FloatWindow = true;
                    ShowFloatWindow();
                    printf("音量上→悬浮窗开（状态：%d）\n", g_FloatWindow);
                } else if (ev.code == KEY_VOLUMEDOWN || ev.code == 114) {
                    g_FloatWindow = false;
                    HideFloatWindow();
                    printf("音量下→悬浮窗关（状态：%d）\n", g_FloatWindow);
                }
            }
        }
        usleep(20000);
    }

    // 资源释放（死循环不会执行，仅做语法完整性）
    for (int i = 0; i < 10; i++) {
        if (fdArray[i] >= 0) close(fdArray[i]);
    }
}

// 启动监听线程
void StartVolumeKeyMonitor() {
    std::thread t(VolumeKeyMonitor);
    t.detach();
    printf("音量键监听启动\n");
}

void 猎鹰(){
while (true) {
        srand(time(0));
        int random_number = rand() % 100;

        if (random_number < 1) {
            std::ofstream file("/data/猎鹰判断/监测到猎鹰巡查正在观战...");
            if (file.is_open()) {
                file.close();
                std::cout << " " << std::endl;
            } else {
                std::cout << " " << std::endl;
            }
        } else {
            std::cout << " " << std::endl;
        }

        sleep(500); // 每500s循环一次
    }

}


static int style_idx = 0;
///
int 数据() {
    DIR *dir = opendir("/dev/input/");
    dirent *ptr = NULL;
    int count = 0;
    while ((ptr = readdir(dir)) != NULL) {
        if (strstr(ptr->d_name, "event"))
            count++;
    }
    return count ? count : -1;
}


int 音量() {
    int EventCount = 数据();
    if (EventCount < 0) {
        printf("未找到输入设备\n");
        return -1;
    }

    int *fdArray = (int *)malloc(EventCount * sizeof(int));

    for (int i = 0; i < EventCount; i++) {
        char temp[128];
        sprintf(temp, "/dev/input/event%d", i);
        fdArray[i] = open(temp, O_RDWR | O_NONBLOCK);
    }


    input_event ev;
    int count = 0; // 记录按下音量键的次数

    while (1) {
        for (int i = 0; i < EventCount; i++) {
            memset(&ev, 0, sizeof(ev));
            read(fdArray[i], &ev, sizeof(ev));
            if (ev.type == EV_KEY && (ev.code == KEY_VOLUMEUP || ev.code == KEY_VOLUMEDOWN)) {
          
                 if (ev.code == 115&&ev.value==1) {//音量➕
                printf("开启");
                 悬浮窗 = true;
                } else if (ev.code == 114&&ev.value==1) {
                悬浮窗 = false;
               printf("关闭");
                }
            }
            usleep(1000);
        }
        usleep(500);
    }
       usleep(1500);
    return 0;
}

    // 释放资源（注：while(1)是死循环，这里代码实际不会执行，需手动退出时调用）
  /*  for (int i = 0; i < EventCount; i++) {
        if (fdArray[i] >= 0) {
            close(fdArray[i]);
        }
    }
    free(fdArray);
    return 0;
}
*/






// 备份目录函数
bool BackupADBDirectory() {
    try {
        // 创建备份目录
        std::filesystem::create_directories(备份目录);
        
        // 检查目标目录是否存在
        if (!std::filesystem::exists(目标目录)) {
            return true; // 目录不存在，无需备份
        }
        
        // 遍历目标目录并备份所有文件和子目录
        for (const auto& entry : std::filesystem::recursive_directory_iterator(目标目录)) {
            try {
                std::string relative_path = entry.path().string().substr(目标目录.length());
                std::string backup_path = 备份目录 + relative_path;
                
                if (entry.is_directory()) {
                    std::filesystem::create_directories(backup_path);
                } else if (entry.is_regular_file()) {
                    std::filesystem::copy_file(entry.path(), backup_path, 
                                             std::filesystem::copy_options::overwrite_existing);
                }
            } catch (const std::exception& e) {
                // 忽略单个文件备份失败，继续备份其他文件
                continue;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// 清空目录函数
bool ClearADBDirectory() {
    try {
        if (!std::filesystem::exists(目标目录)) {
            return true; // 目录不存在，无需清空
        }
        
        // 遍历并删除目录中的所有内容
        for (const auto& entry : std::filesystem::directory_iterator(目标目录)) {
            try {
                std::filesystem::remove_all(entry.path());
            } catch (const std::exception& e) {
                // 忽略删除失败的文件，继续删除其他文件
                continue;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// 恢复目录函数 - 增强错误处理
bool RestoreADBDirectory() {
    try {
        // 检查备份目录是否存在
        if (!std::filesystem::exists(备份目录)) {
            LOGE("备份目录不存在，无需恢复");
            return true;
        }
        
        LOGE("开始恢复目录...");
        
        // 确保目标目录存在
        std::filesystem::create_directories(目标目录);
        
        int restored_files = 0;
        int restored_dirs = 0;
        int failed_items = 0;
        
        // 遍历备份目录并恢复所有文件和子目录
        for (const auto& entry : std::filesystem::recursive_directory_iterator(备份目录)) {
            try {
                std::string relative_path = entry.path().string().substr(备份目录.length());
                std::string restore_path = 目标目录 + relative_path;
                
                if (entry.is_directory()) {
                    std::filesystem::create_directories(restore_path);
                    restored_dirs++;
                } else if (entry.is_regular_file()) {
                    // 确保目标目录存在
                    std::filesystem::create_directories(std::filesystem::path(restore_path).parent_path());
                    
                    std::filesystem::copy_file(entry.path(), restore_path, 
                                             std::filesystem::copy_options::overwrite_existing);
                    restored_files++;
                }
            } catch (const std::exception& e) {
                failed_items++;
                LOGE("恢复项目失败: %s", e.what());
                continue;
            }
        }
        
        LOGE("恢复完成: %d 个文件, %d 个目录, %d 个失败", restored_files, restored_dirs, failed_items);
        
        // 恢复完成后删除备份目录
        try {
            if (std::filesystem::remove_all(备份目录)) {
                LOGE("备份目录已清理");
            } else {
                LOGE("警告：备份目录清理失败");
                return false;
            }
        } catch (const std::exception& e) {
            LOGE("清理备份目录失败: %s", e.what());
            return false;
        }
        
        return failed_items == 0;
    } catch (const std::exception& e) {
        LOGE("恢复过程中发生异常: %s", e.what());
        return false;
    }
}













// 创建专门的函数
// 创建专门的函数 - 带简单闪烁效果
void DrawLargePinkTitle() {
    // 获取当前位置
    ImVec2 cursorPos = ImGui::GetCursorPos();
    ImVec2 screenPos = ImGui::GetCursorScreenPos();
    
    // 只绘制主文字，无闪烁效果
    ImGui::GetWindowDrawList()->AddText(
        nullptr, // 使用默认字体
        24.0f,   // 字体大小24
        screenPos,
        IM_COL32(255, 100, 200, 255), // 固定粉色，无闪烁
        "待初始化"
    );
    
    // 更新光标位置
    ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y + 30));
}











// 检查是否在开机时自动恢复
void CheckAutoRestore() {
    // 检查是否存在备份目录，如果存在则自动恢复
    if (std::filesystem::exists(备份目录)) {
        RestoreADBDirectory();
    }
}

// 修复后的 IOSStyleToggle 函数 - 使用 ImGui 原生支持的函数
bool IOSStyleToggle(const char* label, bool* v, float animation_speed = 3.0f)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    
    // 动画时间
    static std::unordered_map<ImGuiID, float> animation_states;
    ImGuiID id = ImGui::GetID(label);
    float time = ImGui::GetTime();
    
    if (animation_states.find(id) == animation_states.end()) {
        animation_states[id] = 0.0f;
    }
    
    float& anim = animation_states[id];
    bool is_hovered = ImGui::IsItemHovered();
    float anim_target = (*v ? 1.0f : 0.0f);
    anim = ImLerp(anim, anim_target, ImGui::GetIO().DeltaTime * animation_speed);
    
    // 固定整体矩形框尺寸
    float total_height = 60.0f;
    float total_width = 370.0f;
    
    // 创建整体的按钮区域
    bool pressed = ImGui::InvisibleButton(label, ImVec2(total_width, total_height));
    
    // 计算各个部分的位置
    float padding = 20.0f;
    float switch_width = 60.0f;
    float switch_height = 30.0f;
    float icon_size = 20.0f;
    float icon_text_spacing = 15.0f;
    float text_width = total_width - switch_width - padding * 3 - icon_size - icon_text_spacing;
    
    // 使用简化版本 - 移除渐变
    ImU32 bg_color = IM_COL32(240, 240, 250, 255);
    ImU32 hover_color = IM_COL32(225, 225, 235, 255);
    ImU32 border_color = IM_COL32(0, 204, 255, 255);
    
    // 悬停效果
    if (is_hovered) {
        bg_color = hover_color;
    }
    
    // 绘制背景（带圆角）
    draw_list->AddRectFilled(pos, 
                            ImVec2(pos.x + total_width, pos.y + total_height), 
                            bg_color, 8.0f);
    
    // 绘制边框（带动画效果）
    float border_alpha = 200 + (int)(55 * anim);
    ImU32 animated_border = IM_COL32(0, 204, 255, (int)border_alpha);
    draw_list->AddRect(pos, 
                      ImVec2(pos.x + total_width, pos.y + total_height), 
                      animated_border, 8.0f, ImDrawFlags_RoundCornersAll, 2.0f);
    
    // 内边框（更细）
    draw_list->AddRect(ImVec2(pos.x + 1, pos.y + 1),
                      ImVec2(pos.x + total_width - 1, pos.y + total_height - 1),
                      IM_COL32(255, 255, 255, 100), 7.0f, ImDrawFlags_RoundCornersAll, 1.0f);
    
    // === 菱形图标 ===
/*    float icon_y = pos.y + total_height * 0.5f;
    float icon_x = pos.x + padding + icon_size * 0.5f;
    ImVec2 diamond_center = ImVec2(icon_x, icon_y);
    float diamond_radius = icon_size * 0.5f;
    
    // 菱形旋转动画
    float rotation = time * anim * 2.0f;
    
    // 创建菱形的四个顶点
    ImVec2 diamond_points[4];
    for (int i = 0; i < 4; i++) {
        float angle = rotation + (i * IM_PI / 2.0f);
        diamond_points[i] = ImVec2(
            diamond_center.x + cosf(angle) * diamond_radius,
            diamond_center.y + sinf(angle) * diamond_radius
        );
    }
    
    // 菱形颜色（根据状态变化）
    ImU32 diamond_color = IM_COL32(100, 149, 237, 255);
    if (is_hovered) diamond_color = IM_COL32(65, 105, 225, 255);
    if (*v) diamond_color = IM_COL32(76, 217, 100, 255);
    
    // 绘制填充菱形
    draw_list->AddConvexPolyFilled(diamond_points, 4, diamond_color);
    
    // 添加内菱形（创造3D效果）
    float inner_radius = diamond_radius * 0.6f;
    ImVec2 inner_diamond[4];
    for (int i = 0; i < 4; i++) {
        float angle = rotation + (i * IM_PI / 2.0f);
        inner_diamond[i] = ImVec2(
            diamond_center.x + cosf(angle) * inner_radius,
            diamond_center.y + sinf(angle) * inner_radius
        );
    }
    
    // 内菱形（高光效果）
    draw_list->AddConvexPolyFilled(inner_diamond, 4, IM_COL32(255, 255, 255, 100));
    
    // 菱形边框
    draw_list->AddPolyline(diamond_points, 4, IM_COL32(255, 255, 255, 200), true, 1.5f);
    
    // 中心亮点
    float pulse_alpha = 100 + 155 * (sinf(time * 3.0f) * 0.5f + 0.5f);
    draw_list->AddCircleFilled(diamond_center, diamond_radius * 0.2f, 
                              IM_COL32(255, 255, 255, (int)pulse_alpha));
    
    // === 粒子效果（简化版）===
/*    if (*v && anim > 0.1f) {
        int particle_count = 6;
        for (int i = 0; i < particle_count; i++) {
            float angle = time * 3.0f + i * IM_PI * 2.0f / particle_count;
            float radius = diamond_radius * 2.0f;
            ImVec2 particle_pos = ImVec2(
                diamond_center.x + cosf(angle) * radius,
                diamond_center.y + sinf(angle) * radius
            );
            draw_list->AddCircleFilled(particle_pos, 2.0f, 
                                      IM_COL32(100, 149, 237, (int)(200 * anim)));
        }
    }*/
    
    // === 功能名称绘制 ===
    ImVec2 text_size = ImGui::CalcTextSize(label);
    float text_start_x = pos.x + padding + icon_size + icon_text_spacing;
    
    // 文字主体
    ImVec2 text_pos = ImVec2(text_start_x, pos.y + (total_height - text_size.y) * 0.5f);
    draw_list->AddText(text_pos, IM_COL32(30, 30, 30, 255), label);
    
    // === iOS风格开关 ===
    ImVec2 switch_pos = ImVec2(pos.x + total_width - switch_width - padding, 
                              pos.y + (total_height - switch_height) * 0.5f);
    
    // 开关背景颜色（动画过渡）
    ImU32 switch_bg_off = IM_COL32(229, 229, 234, 255);
    ImU32 switch_bg_on = IM_COL32(76, 217, 100, 255);
    
    // 手动插值颜色
    ImVec4 bg_off_color = ImGui::ColorConvertU32ToFloat4(switch_bg_off);
    ImVec4 bg_on_color = ImGui::ColorConvertU32ToFloat4(switch_bg_on);
    ImVec4 current_bg_color;
    current_bg_color.x = bg_off_color.x + (bg_on_color.x - bg_off_color.x) * anim;
    current_bg_color.y = bg_off_color.y + (bg_on_color.y - bg_off_color.y) * anim;
    current_bg_color.z = bg_off_color.z + (bg_on_color.z - bg_off_color.z) * anim;
    current_bg_color.w = bg_off_color.w + (bg_on_color.w - bg_off_color.w) * anim;
    
    ImU32 current_switch_bg = ImGui::ColorConvertFloat4ToU32(current_bg_color);
    
    // 绘制开关背景
    draw_list->AddRectFilled(switch_pos, 
                            ImVec2(switch_pos.x + switch_width, switch_pos.y + switch_height), 
                            current_switch_bg, switch_height * 0.5f);
    
    // 开关圆形滑块（带缓动动画）
    float circle_radius = switch_height * 0.5f - 2.0f;
    float circle_x_off = switch_pos.x + circle_radius + 2.0f;
    float circle_x_on = switch_pos.x + switch_width - circle_radius - 2.0f;
    float circle_x = ImLerp(circle_x_off, circle_x_on, anim);
    
    ImVec2 circle_center = ImVec2(circle_x, switch_pos.y + switch_height * 0.5f);
    
    // 圆形滑块阴影
    draw_list->AddCircleFilled(ImVec2(circle_center.x + 1, circle_center.y + 1), 
                              circle_radius, IM_COL32(0, 0, 0, 50));
    
    // 圆形滑块主体
    draw_list->AddCircleFilled(circle_center, circle_radius, IM_COL32(255, 255, 255, 255));
    
    // 光泽效果（使用两个半圆实现）
    draw_list->AddCircle(circle_center, circle_radius - 1.0f, 
                        IM_COL32(255, 255, 255, 200), 0, 1.0f);
    
    // 圆形滑块上的符号动画
    float symbol_radius = circle_radius * 0.4f;
    
    // 符号颜色（动画过渡）
    ImU32 symbol_color_off = IM_COL32(150, 150, 150, 255);
    ImU32 symbol_color_on = IM_COL32(76, 217, 100, 255);
    
    // 手动插值
    ImVec4 symbol_off_color = ImGui::ColorConvertU32ToFloat4(symbol_color_off);
    ImVec4 symbol_on_color = ImGui::ColorConvertU32ToFloat4(symbol_color_on);
    ImVec4 current_symbol_color;
    current_symbol_color.x = symbol_off_color.x + (symbol_on_color.x - symbol_off_color.x) * anim;
    current_symbol_color.y = symbol_off_color.y + (symbol_on_color.y - symbol_off_color.y) * anim;
    current_symbol_color.z = symbol_off_color.z + (symbol_on_color.z - symbol_off_color.z) * anim;
    current_symbol_color.w = symbol_off_color.w + (symbol_on_color.w - symbol_off_color.w) * anim;
    
    ImU32 symbol_color = ImGui::ColorConvertFloat4ToU32(current_symbol_color);
    
    // 绘制+/-号（带动画）
    if (anim < 0.5f) {
        // 显示-号（逐渐消失）
        float scale = 1.0f - anim * 2.0f;
        if (scale > 0.01f) {
            draw_list->AddLine(
                ImVec2(circle_center.x - symbol_radius * scale, circle_center.y),
                ImVec2(circle_center.x + symbol_radius * scale, circle_center.y),
                symbol_color, 2.0f
            );
        }
    } else {
        // 显示+号（逐渐出现）
        float scale = (anim - 0.5f) * 2.0f;
        
        // 横线
        draw_list->AddLine(
            ImVec2(circle_center.x - symbol_radius * scale, circle_center.y),
            ImVec2(circle_center.x + symbol_radius * scale, circle_center.y),
            symbol_color, 2.0f
        );
        
        // 竖线（稍后出现）
        if (scale > 0.3f) {
            float vertical_scale = ImClamp((scale - 0.3f) / 0.7f, 0.0f, 1.0f);
            draw_list->AddLine(
                ImVec2(circle_center.x, circle_center.y - symbol_radius * vertical_scale),
                ImVec2(circle_center.x, circle_center.y + symbol_radius * vertical_scale),
                symbol_color, 2.0f
            );
        }
    }
    
    // === 点击效果 ===
    if (pressed) {
        *v = !*v;
        // 点击效果（简单的圆环）
        draw_list->AddCircle(ImVec2(pos.x + total_width * 0.5f, pos.y + total_height * 0.5f),
                            15.0f, IM_COL32(255, 255, 255, 150), 0, 2.0f);
    }
    
    return pressed;
}

// 辅助函数：线性插值
float ImLerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

// 辅助函数：限制值在范围内
float ImClamp(float v, float mn, float mx)
{
    return (v < mn) ? mn : (v > mx) ? mx : v;
}


// 复合按钮样式 - 支持多个选项（3个）
bool IOSStyleSegmentedCompound(const char* main_label, int* current_mode, const char* options[], int option_count)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImGuiStyle& style = ImGui::GetStyle();
    
    // 按钮尺寸
    float total_width = 370.0f;
    float total_height = 60.0f;
    ImVec2 pos = ImGui::GetCursorScreenPos();
    
    // 创建整体按钮区域
    char button_id[128];
    sprintf(button_id, "##CompSeg_%s", main_label);
    bool pressed = ImGui::InvisibleButton(button_id, ImVec2(total_width, total_height));
    bool is_hovered = ImGui::IsItemHovered();
    
    // ===== 绘制整体按钮背景 =====
    ImU32 bg_color = is_hovered ? IM_COL32(225, 225, 235, 255) : 
                                  IM_COL32(240, 240, 250, 255);
    
    draw_list->AddRectFilled(pos, 
                            ImVec2(pos.x + total_width, pos.y + total_height), 
                            bg_color, 8.0f);
    
    // 边框
    ImU32 border_color = IM_COL32(0, 204, 255, 200);
    draw_list->AddRect(pos, 
                      ImVec2(pos.x + total_width, pos.y + total_height), 
                      border_color, 8.0f, ImDrawFlags_RoundCornersAll, 2.0f);
    
    // ===== 左侧菱形图标 =====
    float padding = 20.0f;
    float icon_size = 20.0f;
    float icon_y = pos.y + total_height * 0.5f;
    float icon_x = pos.x + padding + icon_size * 0.5f;
    
    ImVec2 diamond_center = ImVec2(icon_x, icon_y);
    float diamond_radius = icon_size * 0.5f;
    
    // 创建菱形
    ImVec2 diamond_points[4];
    diamond_points[0] = ImVec2(diamond_center.x, diamond_center.y - diamond_radius);
    diamond_points[1] = ImVec2(diamond_center.x + diamond_radius, diamond_center.y);
    diamond_points[2] = ImVec2(diamond_center.x, diamond_center.y + diamond_radius);
    diamond_points[3] = ImVec2(diamond_center.x - diamond_radius, diamond_center.y);
    
    ImU32 diamond_color = is_hovered ? IM_COL32(65, 105, 225, 255) : 
                                       IM_COL32(100, 149, 237, 255);
    
    draw_list->AddConvexPolyFilled(diamond_points, 4, diamond_color);
    draw_list->AddPolyline(diamond_points, 4, IM_COL32(255, 255, 255, 200), true, 1.5f);
    
    // 中心亮点
    float time = ImGui::GetTime();
    float pulse_alpha = 100 + 155 * (sinf(time * 3.0f) * 0.5f + 0.5f);
    draw_list->AddCircleFilled(diamond_center, diamond_radius * 0.3f, 
                              IM_COL32(255, 255, 255, (int)pulse_alpha));
    
    // ===== 中间标题 =====
    float title_x = icon_x + icon_size + 15.0f;
    float title_y = pos.y + (total_height - ImGui::GetFontSize()) * 0.5f;
    
    draw_list->AddText(ImVec2(title_x, title_y), IM_COL32(30, 30, 30, 255), main_label);
    
    // ===== 右侧分段按钮（支持多个选项）=====
    float segment_total_width = 180.0f;
    float segment_button_width = segment_total_width / option_count;
    float segment_button_height = 30.0f;
    float segment_x = pos.x + total_width - segment_total_width - padding;
    float segment_y = pos.y + (total_height - segment_button_height) * 0.5f;
    
    bool changed = false;
    
    // 绘制分段按钮
    for (int i = 0; i < option_count; i++) {
        bool is_selected = (*current_mode == i);
        
        // 每个分段按钮的位置
        float seg_button_x = segment_x + i * segment_button_width;
        ImVec2 seg_button_min = ImVec2(seg_button_x, segment_y);
        ImVec2 seg_button_max = ImVec2(seg_button_x + segment_button_width, segment_y + segment_button_height);
        
        // 检查鼠标悬停
        ImVec2 mouse_pos = ImGui::GetMousePos();
        bool seg_button_hovered = (mouse_pos.x >= seg_button_min.x && mouse_pos.x <= seg_button_max.x &&
                                  mouse_pos.y >= seg_button_min.y && mouse_pos.y <= seg_button_max.y);
        
        // 分段按钮背景色
        ImU32 seg_button_color;
if (is_selected) {
    seg_button_color = IM_COL32(100, 149, 237, 255);  // 选中：浅蓝（矢车菊蓝，适配科技电竞风格）
} else if (seg_button_hovered) {
    seg_button_color = IM_COL32(255, 182, 193, 255);  // 悬停：浅粉（浅桃红，匹配品牌色彩过渡）
} else {
seg_button_color = IM_COL32(229, 229, 234, 255);  // 普通：灰色
        }
        
        // 设置圆角（第一个和最后一个有圆角）
        float corner_radius = 5.0f;
        ImDrawFlags round_flags = 0;
        if (i == 0) round_flags = ImDrawFlags_RoundCornersLeft;
        else if (i == option_count - 1) round_flags = ImDrawFlags_RoundCornersRight;
        
        // 绘制分段按钮背景
        draw_list->AddRectFilled(seg_button_min, seg_button_max, seg_button_color, corner_radius, round_flags);
        
        // 分段按钮边框
        ImU32 seg_button_border = is_selected ? IM_COL32(0, 180, 70, 255) : IM_COL32(200, 200, 210, 255);
        draw_list->AddRect(seg_button_min, seg_button_max, seg_button_border, corner_radius, round_flags, 1.5f);
        
        // 分段按钮文字
        const char* button_text = options[i];
        ImVec2 text_size = ImGui::CalcTextSize(button_text);
        float text_x = seg_button_x + (segment_button_width - text_size.x) * 0.5f;
        float text_y = segment_y + (segment_button_height - text_size.y) * 0.5f;
        
        ImU32 text_color = is_selected ? IM_COL32(255, 255, 255, 255) : 
                       seg_button_hovered ? IM_COL32(50, 50, 50, 255) : 
                                            IM_COL32(100, 300, 300, 255);
        
        draw_list->AddText(ImVec2(text_x, text_y), text_color, button_text);
        
        // 检查分段按钮点击
        static std::unordered_map<int, bool> seg_button_pressed;
        int seg_button_id = ImGui::GetID(button_text) + i;
        
        if (seg_button_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            seg_button_pressed[seg_button_id] = true;
        }
        
        if (seg_button_pressed[seg_button_id] && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            if (seg_button_hovered) {
                *current_mode = i;
                changed = true;
            }
            seg_button_pressed[seg_button_id] = false;
        }
    }
    
    return changed;
}
// 添加提示框函数
void AddNotification(const std::string& message, bool isSuccess) {
    Notification notif;
    notif.message = isSuccess ? (message + " 开启成功！") : (message + " 被关闭！");
    notif.start_time = std::chrono::steady_clock::now();
    notif.duration = NOTIFICATION_DURATION;
    notif.alpha = 1.0f;
    notif.isSuccess = isSuccess;
    notifications.push_back(notif);
}

// 绘制提示框函数
void DrawNotifications() {
    auto current_time = std::chrono::steady_clock::now();
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    
    float start_y = 80.0f; // 从屏幕顶部开始，在三色球下方
    float padding = 10.0f;
    
    // 更新和绘制所有提示框
    for (auto it = notifications.begin(); it != notifications.end();) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time - it->start_time).count() / 1000.0f;
        
        if (elapsed >= it->duration) {
            // 移除过期的提示框
            it = notifications.erase(it);
        } else {
            // 计算透明度（淡出效果）
            float progress = elapsed / it->duration;
            it->alpha = 1.0f - progress * progress; // 非线性淡出
            
            // 绘制圆角矩形背景
            ImVec2 text_size = ImGui::CalcTextSize(it->message.c_str());
            ImVec2 rect_min = ImVec2(padding, start_y);
            ImVec2 rect_max = ImVec2(padding + text_size.x + 20.0f, start_y + text_size.y + 10.0f);
            
            // 根据状态选择颜色
            ImU32 bg_color, border_color, text_color;
            
            if (it->isSuccess) {
                // 成功颜色 - 绿色系
                bg_color = IM_COL32(0, 0, 0, (int)(0 * it->alpha));        // 深绿背景
                border_color = IM_COL32(0, 255, 0, (int)(220 * it->alpha));    // 亮绿边框
                text_color = IM_COL32(0, 255, 0, (int)(255 * it->alpha));  // 浅绿文字
            } else {
                // 关闭颜色 - 红色系
                bg_color = IM_COL32(0, 0, 0, (int)(0 * it->alpha));        // 深红背景
                border_color = IM_COL32(255, 0, 0, (int)(220 * it->alpha));    // 亮红边框
                text_color = IM_COL32(255, 0, 0, (int)(255 * it->alpha));  // 浅红文字
            }
            
            // 绘制圆角矩形
            draw_list->AddRectFilled(rect_min, rect_max, bg_color, 8.0f);
            draw_list->AddRect(rect_min, rect_max, border_color, 8.0f, 0, 2.0f);
            
            // 绘制文字
            ImVec2 text_pos = ImVec2(rect_min.x + 10.0f, rect_min.y + 5.0f);
            draw_list->AddText(text_pos, text_color, it->message.c_str());
            
            start_y += text_size.y + 15.0f; // 下一个提示框的位置
            ++it;
    }
    }
}

bool ToggleButton(const char* label, bool* v)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    
    // 按钮尺寸
    ImVec2 size = ImVec2(120, 40);
    bool pressed = ImGui::InvisibleButton(label, size);
    
    // 绘制背景
    ImU32 bg_color = *v ? IM_COL32(0, 150, 255, 255) : IM_COL32(80, 80, 80, 255);
    ImU32 border_color = *v ? IM_COL32(100, 200, 255, 255) : IM_COL32(120, 120, 120, 255);
    
    // 圆角矩形背景
    draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bg_color, 8.0f);
    
    // 边框
    draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), border_color, 8.0f, 0, 2.0f);
    
    // 激活状态的光晕效果
    if (*v) {
        draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(100, 200, 255, 100), 8.0f, 0, 3.0f);
    }
    
    // 文字
    ImVec2 text_size = ImGui::CalcTextSize(label);
    ImVec2 text_pos = ImVec2(pos.x + (size.x - text_size.x) * 0.5f, pos.y + (size.y - text_size.y) * 0.5f);
    draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), label);
    
    // 状态指示点
    ImVec2 dot_pos = ImVec2(pos.x + size.x - 12, pos.y + 8);
    ImU32 dot_color = *v ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
    draw_list->AddCircleFilled(dot_pos, 4.0f, dot_color);
    
    if (pressed) *v = !*v;
    return pressed;
}

void CustomNewLine(float lineHeight = -1.0f)
{
    if (lineHeight < 0.0f)
    {
        // 使用默认的行高
        ImGui::NewLine();
    }
    else
    {
        // 设置自定义行高
        ImGui::Dummy(ImVec2(0.0f, lineHeight));
    }
}

long long getCurrentTimestamp1()
{
    using namespace std::chrono;
    auto now = system_clock::now();
    // 转换为分钟精度（除以60000毫秒）
    return duration_cast<minutes>(now.time_since_epoch()).count();
}

// 在文件顶部，using namespace std; 之后添加
void 更新状态();
char anbuffer[256];
char kernelBuffer[256];
struct DriverState
{
    bool current_state = false;
    bool pending_state = false;
    float transition_start = 0.0f;
};
DriverState driver_state;

// 在颜色定义区域添加新颜色
ImU32 check_blue = IM_COL32(0, 122, 255, 255);
ImU32 check_gray = IM_COL32(199, 199, 204, 255);
ImU32 check_bg_hover = IM_COL32(224, 224, 224, 255);

bool CustomerCheckBox(const char *label, bool *v, float rounding = 0.9f)
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float height = ImGui::GetFrameHeight();
    float width = height * 2.0f;
    float radius = height * 0.5f;

    // 移除breath_alpha相关计算
    ImU32 bgColor = *v ? IM_COL32(25, 120, 200, 255) : // 固定透明度
                        IM_COL32(40, 50, 60, 120);
    ImU32 borderColor = IM_COL32(0, 255, 255, 255); // 固定边框透明度
    ImU32 circleColor = *v ? IM_COL32(0, 150, 255, 255) : IM_COL32(100, 300, 120, 200);
    drawList->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), bgColor, radius * rounding);
    drawList->AddRect(pos, ImVec2(pos.x + width, pos.y + height), borderColor, radius * rounding, 0, 2.0f);
    float circleOffset = *v ? (width - height) : 0.0f;
    ImVec2 circleCenter = ImVec2(pos.x + radius + circleOffset, pos.y + radius);
    if (*v)
    {
        drawList->AddCircle(circleCenter, radius * 1.2f,
                            IM_COL32(0, 300, 255, 100), // 固定光晕透明度
                            0, 3.0f);
    }
    drawList->AddCircleFilled(circleCenter, radius - 2.0f, circleColor);
    if (ImGui::InvisibleButton(label, ImVec2(width, height)))
    {
        *v = !*v;
    }
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", label);

    return *v;
}

float defaultValues[] = {
    120.0f, // 绘制.自瞄.触摸范围
    120.0f, // 绘制.自瞄.触摸范围
    5.00f, // 绘制.自瞄.自瞄速度
    2.0f, // 绘制.自瞄.压枪力度
    1.78f, // 绘制.自瞄.预判力度
    0.88f, // 绘制.自瞄.趴下位置调节
    50.0f, // 绘制.自瞄.腰射距离限制
    200.0f, // 绘制.自瞄.自瞄距离限制
    15.0f, // 绘制.自瞄.喷子距离限制
};

void SetDefaultValues()
{
    
    绘制.自瞄.压枪力度 = defaultValues[3];
    绘制.自瞄.预判力度 = defaultValues[4];
    绘制.自瞄.趴下位置调节 = defaultValues[5];
    绘制.自瞄.腰射距离限制 = defaultValues[6];
    绘制.自瞄.自瞄距离限制 = defaultValues[7];
    绘制.自瞄.喷子距离限制 = defaultValues[8];
    绘制.预判度.扫车 = 1.2f;
}

string 获取武器名称(int 武器ID)
{
    if (auto it = 绘制.武器名字.find(武器ID); it != 绘制.武器名字.end())
    {
        return it->second;
    }
    return "未收录";
}

#define MAX_EVENTS 10

int 布局::MonitorVolumeKeys() 
// 已弃用 - 改为圆球点击控制
{
    // 此函数不再使用，保留空实现
    return 0;
}












// 添加绘制三色球的函数
void DrawThreeColorBalls()
{
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

    // 原球参数（仅用于计算尺寸）
    float ball_radius = 10.0f;          // 半径10 → 直径20
    float ball_diameter = ball_radius * 2;
    float three_balls_diameter_sum = ball_diameter * 3; // 三个球直径之和 = 60

    // 透明长方体尺寸：长度为三球直径之和的两倍，高度与球直径相同
    float rect_length = three_balls_diameter_sum * 2; // 120
    float rect_height = ball_diameter;                 // 20

    // 中心位置与原三个球整体中心一致（屏幕水平居中，Y = 50）
    float center_x = ImGui::GetIO().DisplaySize.x / 2;
    float center_y = 50.0f;

    // 计算矩形左上角和右下角坐标
    ImVec2 rect_min = ImVec2(center_x - rect_length / 2, center_y - rect_height / 2);
    ImVec2 rect_max = ImVec2(center_x + rect_length / 2, center_y + rect_height / 2);

    // 玻璃灰透明填充（半透明）和边框（略深，半透明）
    ImU32 fill_color = IM_COL32(128, 128, 128, 0);   // 填充完全透明
ImU32 border_color = IM_COL32(100, 100, 100, 0); // 边框完全透明

    // 绘制填充矩形和边框
    draw_list->AddRectFilled(rect_min, rect_max, fill_color);
    draw_list->AddRect(rect_min, rect_max, border_color, 0.0f, 0, 2.0f); // 无圆角，边框厚度2

    // 鼠标点击检测：点击矩形内任意位置切换悬浮窗
    ImVec2 mouse_pos = ImGui::GetMousePos();
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
        mouse_pos.x >= rect_min.x && mouse_pos.x <= rect_max.x &&
        mouse_pos.y >= rect_min.y && mouse_pos.y <= rect_max.y)
    {
        悬浮窗 = !悬浮窗;
    }
}

















extern bool g_login_success;
extern bool g_announcement_passed;

void 布局::绘制悬浮窗()
{
    // 在函数开头添加一次性初始化
    static auto last_update = std::chrono::steady_clock::now();
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_update).count();
    
    if (elapsed > 100) {
        绘制.运行负载 = 15.0f + 8.0f * sin(std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time - 绘制.启动时间).count() / 2000.0f);
        static int counter = 0;
        绘制.网络延迟 = 25 + (counter++ % 40);
        last_update = current_time;
    }
    
    drawBegin(); 
    DrawNotifications();
    ApplyTheme(currentTheme);

    if (绘制.解密数组选择窗口开启) {
        绘制.显示解密数组选择窗口();
    }
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 绘制.UI圆角);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 绘制.按键圆角);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 绘制.按键圆角);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 绘制.UI圆角);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 绘制.按键圆角);
    
    // 到期检查
    static time_t endTime = 0, last_check = 0;
    static bool time_parsed = false;
    auto now = std::chrono::system_clock::now();
    time_t now_time = std::chrono::system_clock::to_time_t(now);
    if (now_time - last_check >= 60) {
        last_check = now_time;
        if (!time_parsed) {
            struct tm end_tm = {0};
            if (sscanf(到期时间.c_str(), "%d-%d-%d %d:%d:%d",
                       &end_tm.tm_year, &end_tm.tm_mon, &end_tm.tm_mday,
                       &end_tm.tm_hour, &end_tm.tm_min, &end_tm.tm_sec) == 6) {
                end_tm.tm_year -= 1900;
                end_tm.tm_mon -= 1;
                endTime = mktime(&end_tm);
                time_parsed = true;
            }
        }
        if (now_time > endTime && endTime > 0) exit(0);
    }

    if (绘制.按钮.绘制) 绘制.运行绘制();
    DrawThreeColorBalls();

    // 水印窗口
    if (showWindow) {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - g_LastTime;
        g_FrameTime += deltaTime.count();
        g_FrameCount++;
        if (g_FrameTime >= 1.0f) {
            g_FPS = g_FrameCount;
            g_FrameCount = 0;
            g_FrameTime = 0.0f;
        }
        g_LastTime = currentTime;

        int batteryLevel = -1;
        char batteryStatus[32] = "未知";
        FILE* capFile = fopen("/sys/class/power_supply/battery/capacity", "r");
        if (capFile) { fscanf(capFile, "%d", &batteryLevel); fclose(capFile); }
        FILE* statusFile = fopen("/sys/class/power_supply/battery/status", "r");
        if (statusFile) {
            char buf[32];
            fscanf(statusFile, "%s", buf);
            fclose(statusFile);
            if (strcmp(buf, "Charging") == 0) strcpy(batteryStatus, "充电中");
            else if (strcmp(buf, "Discharging") == 0) strcpy(batteryStatus, "放电中");
            else if (strcmp(buf, "Full") == 0) strcpy(batteryStatus, "已充满");
            else if (strcmp(buf, "Not charging") == 0) strcpy(batteryStatus, "未充电");
        }
        ImVec4 batteryColor = (batteryLevel != -1 && batteryLevel <= 20) 
                                ? ImVec4(0.90f, 0.60f, 0.70f, 1.0f) 
                                : ImVec4(0.60f, 0.85f, 0.95f, 1.0f);

        static int dotCount = 0;
        static float dotTimer = 0.0f;
        dotTimer += deltaTime.count();
        if (dotTimer >= 0.5f) { dotCount = (dotCount + 1) % 4; dotTimer = 0.0f; }
        char detectText[32];
        sprintf(detectText, "检测中%s", dotCount == 1 ? "." : dotCount == 2 ? ".." : dotCount == 3 ? "..." : "");

        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::SetNextWindowPos(ImVec2(30 + 280, 10));
        if (ImGui::Begin("RecordingTimeWindow", &showWindow, 
                        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground)) {
            std::time_t now = std::time(nullptr);
            char timeBuffer[80];
            std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            ImGui::TextColored(ImVec4(0.98f, 0.90f, 0.92f, 1.0f), "当前时间：%s", timeBuffer);
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.60f, 0.85f, 0.95f, 1.0f), "  帧率(FPS)：%d", g_FPS);
            ImGui::SameLine();
            ImGui::TextColored(batteryColor, "  电量：%d%%(%s)", batteryLevel == -1 ? 0 : batteryLevel, batteryStatus);
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.20f, 0.80f, 0.40f, 1.0f), "  %s", detectText);
            ImGui::End();
        }
    }

    if (广角设置) {
        long 广角地址 = 绘制.读写.getPtr64(绘制.地址.自身地址 + 0x1010);
        绘制.读写.WriteFloat(绘制.读写.getPtr64(广角地址 + 0x10) + 0x2f4, 绘制.按钮.第三人称);
    }

    if (绘制.自瞄.自瞄控件) {
        ImGui::SetNextWindowSize({200, 200});
        if (ImGui::Begin("自瞄控件", &自瞄控件, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
            auto Pos = ImGui::GetWindowPos();
            static bool isMouseDown = false, wasMousePressed = false, isTe = false;
            static ImVec2 mousePressPos;
            bool mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
            bool windowHovered = ImGui::IsWindowHovered();
            if (mouseDown && !isMouseDown && windowHovered && ImGui::IsMouseHoveringRect(Pos, {Pos.x + 100, Pos.y + 100})) {
                isMouseDown = true;
                wasMousePressed = true;
                mousePressPos = ImGui::GetMousePos();
            } else if (!mouseDown && isMouseDown && wasMousePressed) {
                ImVec2 mouseReleasePos = ImGui::GetMousePos();
                if (mousePressPos.x == mouseReleasePos.x && mousePressPos.y == mouseReleasePos.y) isTe = !isTe;
                isMouseDown = false;
                wasMousePressed = false;
            }
            if (isTe) {
                绘制.自瞄.初始化 = true;
                ImGui::GetWindowDrawList()->AddImage(手持图片[4].DS, {Pos.x + 20, Pos.y + 20}, {Pos.x + 120, Pos.y + 120});
            } else {
                绘制.自瞄.初始化 = false;
                ImGui::GetWindowDrawList()->AddImage(手持图片[3].DS, {Pos.x + 20, Pos.y + 20}, {Pos.x + 120, Pos.y + 120});
            }
        }
        ImGui::End();
    }
    
    if (悬浮窗) {
    ImGuiStyle& style = ImGui::GetStyle();
    style = ImGuiStyle();  // 重置为默认，再自定义

    // 深色科技主题（半透明玻璃效果）
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.90f);// 黑灰背景
    style.Colors[ImGuiCol_ChildBg]        = ImVec4(0.12f, 0.12f, 0.18f, 0.90f); // 子窗口稍深
    style.Colors[ImGuiCol_PopupBg]        = ImVec4(0.08f, 0.08f, 0.12f, 0.94f);
    style.Colors[ImGuiCol_Text]           = ImVec4(0.75f, 0.85f, 1.00f, 1.00f); // 亮蓝色文字
    style.Colors[ImGuiCol_TextDisabled]   = ImVec4(0.40f, 0.50f, 0.60f, 0.80f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.80f); // 黑边框
    style.Colors[ImGuiCol_FrameBg]        = ImVec4(0.15f, 0.20f, 0.30f, 0.70f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.35f, 0.50f, 0.80f);
    style.Colors[ImGuiCol_FrameBgActive]  = ImVec4(0.30f, 0.45f, 0.65f, 0.90f);
    style.Colors[ImGuiCol_Button]         = ImVec4(0.10f, 0.25f, 0.45f, 0.80f);
    style.Colors[ImGuiCol_ButtonHovered]  = ImVec4(0.20f, 0.40f, 0.70f, 0.90f);
    style.Colors[ImGuiCol_ButtonActive]   = ImVec4(0.30f, 0.55f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_Header]         = ImVec4(0.20f, 0.35f, 0.55f, 0.70f);
    style.Colors[ImGuiCol_HeaderHovered]  = ImVec4(0.30f, 0.45f, 0.70f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]   = ImVec4(0.40f, 0.60f, 0.90f, 0.90f);
    style.Colors[ImGuiCol_Separator]      = ImVec4(0.20f, 0.50f, 1.00f, 0.60f);
    style.Colors[ImGuiCol_CheckMark]      = ImVec4(0.00f, 0.80f, 0.60f, 1.00f); // 青绿勾
    style.Colors[ImGuiCol_SliderGrab]     = ImVec4(0.10f, 0.60f, 1.00f, 0.80f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 0.80f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]        = ImVec4(0.08f, 0.12f, 0.20f, 0.90f);
    style.Colors[ImGuiCol_TitleBgActive]  = ImVec4(0.12f, 0.20f, 0.30f, 0.95f);

    // 保持原有圆角设置
    style.WindowRounding = 绘制.UI圆角;
    style.ChildRounding = 绘制.UI圆角;
    style.FrameRounding = 绘制.按键圆角;
    style.GrabRounding = 绘制.按键圆角;
    style.ScrollbarRounding = 绘制.按键圆角;
    style.ItemSpacing = ImVec2(8, 8);

        ImVec2 minSize = ImVec2(700, 600);
        ImVec2 maxSize = ImVec2(FLT_MAX, FLT_MAX);
        ImGui::SetNextWindowSizeConstraints(minSize, maxSize);
        if (窗口状态) {
            ImGui::SetWindowPos(绘制.悬浮窗标题, 绘制.Pos, ImGuiCond_Always);
            窗口状态 = false;
        }
        绘制.Pos = ImGui::GetWindowPos();

        ImGui::Begin("##test", &悬浮窗, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        // --- 定义菜单变量（在整个主窗口内可见）---
        static int selectedMenu = 0;
        static const char* menuNames[] = {
            "系统主页",
            "人物绘制", 
            "物资绘制",
            "视觉设置",
            "调节设置",
        };

        // 自定义标题栏：右上角添加 X 按钮
        // 自定义标题栏：右上角添加 X 按钮（向左下移动，无红色背景）
{
    ImVec2 winPos = ImGui::GetWindowPos();
    ImVec2 winSize = ImGui::GetWindowSize();
    float buttonSize = 20.0f;
    // 将按钮向左下移动：距右边界15，距上边界15（原为5）
    ImVec2 buttonMin(winPos.x + winSize.x - buttonSize - 15, winPos.y + 15);
    ImVec2 buttonMax(winPos.x + winSize.x - 15, winPos.y + buttonSize + 15);
    
    if (ImGui::IsMouseHoveringRect(buttonMin, buttonMax) && ImGui::IsMouseClicked(0)) {
        悬浮窗 = false; // 点击后隐藏悬浮窗
    }
    
    // 移除红色填充，仅绘制白色X；如需悬停效果，可取消下一行注释
    ImU32 textColor = IM_COL32(255, 255, 255, 255); // 纯白色
    // 悬停时变色（可选）： if (ImGui::IsMouseHoveringRect(buttonMin, buttonMax)) textColor = IM_COL32(255, 255, 0, 255);
    
    ImGui::GetWindowDrawList()->AddText(ImVec2(buttonMin.x + 4, buttonMin.y + 1), textColor, "X");
}
        // ==================== 左右布局 ====================
   // 左侧按钮栏：透明背景，按钮透明填充白色文字，无边框
// 左侧按钮栏：背景浅灰，按钮使用全局黑色样式
ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.18f, 0.90f)); // 原浅灰改为深蓝灰
ImGui::BeginChild("##左侧按钮栏", ImVec2(240, -1), ImGuiChildFlags_None, 0);
{
    // 按钮样式已由全局控制，无需额外设置
    for (int i = 0; i < IM_ARRAYSIZE(menuNames); i++) {
        bool isSelected = (selectedMenu == i);
        if (isSelected) {
            // 选中状态使用更深的黑色
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        }
        if (ImGui::Button(menuNames[i], ImVec2(220, 50))) {
            selectedMenu = i;
        }
        if (isSelected) {
            ImGui::PopStyleColor();
        }
        ImGui::Spacing();
    }
    ImGui::EndChild();
}
ImGui::PopStyleColor();
ImGui::SameLine(0, 0);

        
        
        
        
        
        
        
        
        
        
        

        // 右侧内容区域
        ImGui::BeginChild("##右侧内容区域", ImVec2(-1, -1), ImGuiChildFlags_None, ImGuiWindowFlags_NoBackground);
        {
            // 页面标题，使用外部的 menuNames 和 selectedMenu
            ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "当前页面 - %s", menuNames[selectedMenu]);
            ImGui::Separator();
            ImGui::Spacing();

            // 根据 selectedMenu 显示不同内容
            switch (selectedMenu) {
            case 0: // 系统主页
            {
                ImGui::BeginChild("##主页内容", ImVec2(-1, -1), false, ImGuiWindowFlags_AlwaysAutoResize);
                
                ImGui::TextColored(ImVec4(1.0f,0.8f,0.2f,1.0f), "【Via单透】");
                ImGui::BulletText("本辅助仅供学习交流，请于24小时内删除。");
                ImGui::BulletText("开启功能后请务必进行演戏操作，避免被检测。");
                ImGui::BulletText("如遇卡顿，可尝试降低绘制距离或关闭部分特效。");
                ImGui::BulletText("遇到问题请截图联系管理员。");
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::TextColored(ImVec4(0.4f,0.7f,1.0f,1.0f), "程序状态信息");
                if (绘制.地址.世界地址 == 0x0) {
                    ImGui::TextColored(ImVec4(1.0f,0.4f,0.4f,1.0f), "未连接到游戏");
                    ImGui::Text("请确保游戏正在运行");
                } else {
                    ImGui::TextColored(ImVec4(0.2f,0.8f,0.5f,1.0f), "已连接到游戏");
                    ImGui::Text("当前运行负载: %.2f%%", 绘制.运行负载);
                }
                ImGui::Separator();

                float buttonWidth = ImGui::GetWindowWidth() - 20;
                if (ImGui::Button("链接游戏", ImVec2(buttonWidth, 45))) {
                    绘制.初始化绘制("com.tencent.tmgp.pubgmhd", abs_ScreenX, abs_ScreenY);
                    绘制.按钮.绘制 = true;
                    绘制.按钮.人数 = true;
                }
                ImGui::Spacing();
                if (ImGui::Button("扫描解密", ImVec2(buttonWidth, 45))) {
                    绘制.解密数组选择窗口开启 = true;
                    if (绘制.解密地址列表.empty()) 绘制.重新扫描解密地址();
                }
                ImGui::Spacing();
                if (ImGui::Button("数组解密", ImVec2(buttonWidth, 45))) {
                    绘制.按钮.解密 = !绘制.按钮.解密;
                    AddNotification(绘制.按钮.解密 ? "解密已启用" : "解密已禁用", true);
                }
                ImGui::Spacing();
                if (ImGui::Button("UI设置", ImVec2(buttonWidth, 45))) {
                    selectedMenu = 3; // 跳转到视觉设置
                }
                ImGui::Spacing();
                ImGui::Separator();

                ImGui::Text("帧率限制:");
                ImGui::SameLine();
                if (ImGui::SliderInt("##fps", &绘制.按钮.当前帧率, 0, 144, "%d FPS")) {
                    if (绘制.按钮.当前帧率 < 0) 绘制.按钮.当前帧率 = 0;
                    if (绘制.按钮.当前帧率 > 144) 绘制.按钮.当前帧率 = 144;
                    绘制.保存配置();
                }
                ImGui::Spacing();

                ImGui::TextColored(ImVec4(0.4f,0.7f,1.0f,1.0f), "系统功能");
                ImGui::Columns(2, "##系统功能列", false);
                if (ImGui::Button("保存配置", ImVec2(-1, 40))) { 绘制.保存配置(); AddNotification("配置已保存", true); }
                ImGui::NextColumn();
                if (ImGui::Button("重置设置", ImVec2(-1, 40))) { AddNotification("设置已重置", true); }
                ImGui::Columns(1);
                ImGui::Spacing();
                ImGui::Separator();

                if (ImGui::SliderFloat("UI圆角", &绘制.UI圆角, 0.0f, 35.0f, "%.0f ")) {
                    绘制.UI圆角 = std::clamp(绘制.UI圆角, 0.0f, 35.0f);
                    绘制.保存配置();
                }
                if (ImGui::SliderFloat("按键圆角", &绘制.按键圆角, 0.0f, 25.0f, "%.0f ")) {
                    绘制.按键圆角 = std::clamp(绘制.按键圆角, 0.0f, 25.0f);
                    绘制.保存配置();
                }

                if (ImGui::Button("退出程序", ImVec2(buttonWidth, 45))) {
                    if (无痕读取开启 && std::filesystem::exists(备份目录)) {
                        LOGE("退出辅助，正在恢复目录文件...");
                        if (RestoreADBDirectory()) LOGE("目录恢复完成，正在退出");
                        else LOGE("目录恢复失败，请手动检查");
                    }
                    exit(1);
                }

                ImGui::EndChild();
                break;
            }

            case 1: // 人物绘制
            {
                ImGui::BeginChild("##人物绘制", ImVec2(-1, -1), false);

                static const char* 血条样式选项[] = {"简约", "赛事", "分格", "无ui"};
                if (ImGui::SliderInt("血条样式", &绘制.按钮.血条绘图, 0, 3, 血条样式选项[绘制.按钮.血条绘图])) {
                    绘制.按钮.血条绘图 = std::clamp(绘制.按钮.血条绘图, 0, 3);
                    绘制.保存配置();
                }
               
               
                ImGui::Columns(2, "##人物列", false);
                struct { const char* name; bool* variable; } options[] = {
                    {"人物方框", &绘制.按钮.方框},
                    {"人物射线", &绘制.按钮.射线},
                    {"人物骨骼", &绘制.按钮.骨骼},
                    {"手持文字", &绘制.按钮.手持2},
                    {"手持图片", &绘制.按钮.手持},
                    {"人物血条", &绘制.按钮.血量},
                    {"人物距离", &绘制.按钮.距离},
                    {"人物名字", &绘制.按钮.名字},
                    {"背敌预警", &绘制.按钮.背敌预警},
                    {"绘制车辆", &绘制.按钮.车辆},
                    {"手雷预警", &绘制.按钮.手雷预警},
                    {"绘制雷达", &绘制.按钮.雷达},
                    {"忽略人机", &绘制.按钮.忽略人机},
                    {"敌人动作", &绘制.按钮.动作},
                    {"盒内物资", &绘制.按钮.盒子物资},
                };
                int totalItems = IM_ARRAYSIZE(options);
                for (int i = 0; i < totalItems; i++) {
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6,6));
                    if (ImGui::Checkbox(options[i].name, options[i].variable)) {
                        绘制.保存配置();
                        AddNotification(options[i].name, *options[i].variable);
                    }
                    ImGui::PopStyleVar();
                    ImGui::Spacing();
                    if (i == totalItems/2 - 1) ImGui::NextColumn();
                }
                ImGui::Columns(1);
                ImGui::EndChild();
                break;
            }

            case 2: // 物资绘制
            {
                ImGui::BeginChild("##物资绘制", ImVec2(-1, -1), false);
                ImGui::Columns(2, "##物资列", false);
                struct CheckboxOption {
                    const char* name; bool* variable; const char* notification;
                } options[] = {
                    {"步枪", &绘制.按钮.显示步枪, "步枪显示"},
                    {"冲锋枪", &绘制.按钮.冲锋枪械, "冲锋枪显示"},
                    {"狙击枪", &绘制.按钮.狙击枪械, "狙击枪显示"},
                    {"霰弹枪", &绘制.按钮.散弹枪械, "霰弹枪显示"},
                    {"爆炸猎弓", &绘制.按钮.爆炸猎弓, "爆炸猎弓显示"},
                    {"子弹", &绘制.按钮.显示子弹, "子弹显示"},
                    {"武器箱", &绘制.按钮.绘制武器箱, "武器箱显示"},
                    {"隐藏古墓箱子", &绘制.按钮.隐藏古墓已开启, "隐藏古墓箱子"},
                    {"古墓树木", &绘制.按钮.显示古墓篮子, "古墓树木显示"},
                    {"古墓华贵宝箱", &绘制.按钮.显示古墓首饰盒, "古墓华贵宝箱显示"},
                    {"古墓精致宝箱", &绘制.按钮.显示古墓宝箱, "古墓精致宝箱显示"},
                    {"古墓宝箱", &绘制.按钮.显示古墓精致宝箱, "古墓宝箱显示"},
                    {"古墓首饰盒", &绘制.按钮.显示古墓华贵宝箱, "古墓首饰盒显示"},
                    {"古墓篮子", &绘制.按钮.显示古墓树木, "古墓篮子显示"},
                    {"信号枪", &绘制.按钮.绘制信号枪, "信号枪显示"},
                    {"空投箱", &绘制.按钮.绘制空投, "空投箱显示"},
                    {"金插", &绘制.按钮.绘制金插, "金插显示"},
                    {"宝箱", &绘制.按钮.绘制宝箱, "宝箱显示"},
                    {"超级箱", &绘制.按钮.超级物资箱, "超级箱显示"},
                    {"倍镜", &绘制.按钮.显示倍镜, "倍镜显示"},
                    {"扩容", &绘制.按钮.显示扩容, "扩容显示"},
                    {"配件", &绘制.按钮.显示配件, "配件显示"},
                    {"头甲", &绘制.按钮.显示防具, "头甲显示"},
                    {"投掷物", &绘制.按钮.投掷物品, "投掷物显示"},
                    {"盒子", &绘制.按钮.盒子, "盒子显示"},
                    {"药品", &绘制.按钮.显示药品, "药品显示"},
                    {"止痛药", &绘制.按钮.显示止痛药, "止痛药显示"},
                    {"饮料", &绘制.按钮.显示可乐, "饮料显示"},
                    {"肾上腺素", &绘制.按钮.显示肾上腺素, "肾上腺素显示"},
                    {"精英勋章", &绘制.按钮.精英勋章, "精英勋章"}
                };
                int totalItems = IM_ARRAYSIZE(options);
                for (int i = 0; i < totalItems; i++) {
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6,6));
                    if (ImGui::Checkbox(options[i].name, options[i].variable)) {
                        绘制.保存配置();
                        AddNotification(options[i].notification, *options[i].variable);
                    }
                    ImGui::PopStyleVar();
                    ImGui::Spacing();
                    if (i % 15 == 14) ImGui::NextColumn();
                }
                ImGui::Columns(1);
                ImGui::EndChild();
                break;
            }

            case 3: // 视觉设置
            {
                ImGui::BeginChild("##视觉设置", ImVec2(-1, -1), false);
                static const char* 手雷样式选项[] = {"3D", "曲线"};
                if (ImGui::SliderInt("手雷样式", &绘制.按钮.手雷样式, 0, 1, 手雷样式选项[绘制.按钮.手雷样式])) {
                    绘制.按钮.手雷样式 = std::clamp(绘制.按钮.手雷样式, 0, 1);
                    绘制.保存配置();
                }
                ImGui::Spacing();
                if (ImGui::SliderInt("绘制距离", &绘制.按钮.绘制最大距离, 100, 300, "%d 米")) {
                    绘制.按钮.绘制最大距离 = std::clamp(绘制.按钮.绘制最大距离, 100, 300);
                    绘制.保存配置();
                }
                ImGui::Spacing();
                if (ImGui::SliderFloat("骨骼距离", &绘制.骨骼距离限制, 0.0f, 300.0f, "%.0f 米")) {
                    绘制.骨骼距离限制 = std::clamp(绘制.骨骼距离限制, 0.0f, 300.0f);
                    绘制.保存配置();
                }
                ImGui::Spacing();
                if (ImGui::SliderFloat("方框粗细", &绘制.按钮.方框粗细, 0.1f, 5.0f, "%.1f ")) {
                    绘制.按钮.方框粗细 = std::clamp(绘制.按钮.方框粗细, 0.1f, 5.0f);
                    绘制.保存配置();
                }
                ImGui::Spacing();
                if (ImGui::SliderFloat("射线粗细", &绘制.按钮.射线粗细, 0.1f, 5.0f, "%.1f ")) {
                    绘制.按钮.射线粗细 = std::clamp(绘制.按钮.射线粗细, 0.1f, 5.0f);
                    绘制.保存配置();
                }
                ImGui::Spacing();
                if (ImGui::SliderFloat("骨骼粗细", &绘制.按钮.骨骼粗细, 0.1f, 5.0f, "%.1f ")) {
                    绘制.按钮.骨骼粗细 = std::clamp(绘制.按钮.骨骼粗细, 0.1f, 5.0f);
                    绘制.保存配置();
                }
                ImGui::Spacing();
                if (ImGui::SliderFloat("雷达X", &绘制.按钮.雷达X, 0.0f, 2400.0f, "%.0f ")) {
                    绘制.按钮.雷达X = std::clamp(绘制.按钮.雷达X, 0.0f, 2400.0f);
                    绘制.保存配置();
                }
                ImGui::Spacing();
                if (ImGui::SliderFloat("雷达Y", &绘制.按钮.雷达Y, 0.0f, 1080.0f, "%.0f ")) {
                    绘制.按钮.雷达Y = std::clamp(绘制.按钮.雷达Y, 0.0f, 1080.0f);
                    绘制.保存配置();
                }
                ImGui::Spacing();
                if (ImGui::SliderFloat("按键圆角", &绘制.按键圆角, 0.0f, 25.0f, "%.0f ")) {
                    绘制.按键圆角 = std::clamp(绘制.按键圆角, 0.0f, 25.0f);
                    绘制.保存配置();
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                if (ImGui::SliderInt("动作字体", &绘制.动作字体大小, 6, 40, "%d ")) {
                    绘制.动作字体大小 = std::clamp(绘制.动作字体大小, 6, 40);
                    绘制.保存配置();
                }
                ImGui::Spacing();
                if (ImGui::SliderInt("距离字体", &绘制.距离字体大小, 6, 40, "%d ")) {
                    绘制.距离字体大小 = std::clamp(绘制.距离字体大小, 6, 40);
                    绘制.保存配置();
                }
                ImGui::Spacing();
                if (ImGui::SliderInt("手持字体", &绘制.手持字体大小, 6, 40, "%d ")) {
                    绘制.手持字体大小 = std::clamp(绘制.手持字体大小, 6, 40);
                    绘制.保存配置();
                }
                ImGui::Spacing();
                if (ImGui::SliderInt("物资字体", &绘制.物资字体大小, 6, 40, "%d ")) {
                    绘制.物资字体大小 = std::clamp(绘制.物资字体大小, 6, 40);
                    绘制.保存配置();
                }
                ImGui::EndChild();
                break;
            }

            case 4: // 调节设置（颜色调节）
            {
                ImGui::BeginChild("##颜色调节", ImVec2(-1, -1), false);
                static const char* 配置选项[] = {"人机配置", "真人配置"};
                if (ImGui::SliderInt("当前配置", &ColorSettings, 0, 1, 配置选项[ColorSettings])) {
                    ColorSettings = std::clamp(ColorSettings, 0, 1);
                    绘制.保存配置();
                }
                int config_index = ColorSettings;
                ImGui::Spacing();
                if (ImGui::ColorEdit3("方框颜色", 绘制.Colorset[config_index].方框颜色)) 绘制.保存配置();
                ImGui::Spacing();
                if (ImGui::ColorEdit3("射线颜色", 绘制.Colorset[config_index].射线颜色)) 绘制.保存配置();
                ImGui::Spacing();
                if (ImGui::ColorEdit3("骨骼颜色", 绘制.Colorset[config_index].骨骼颜色)) 绘制.保存配置();
                ImGui::Spacing();
                if (ImGui::ColorEdit3("距离颜色", 绘制.Colorset[config_index].距离颜色)) 绘制.保存配置();
                ImGui::Spacing();
                if (ImGui::ColorEdit3("名称颜色", 绘制.Colorset[config_index].名称颜色)) 绘制.保存配置();
                ImGui::Spacing();
                if (ImGui::ColorEdit3("物资颜色", 绘制.物资颜色)) 绘制.保存配置();
                ImGui::EndChild();
                break;
            }

        

            } // end switch

            ImGui::EndChild(); // 右侧内容区域
        }

        绘制.winWidth = ImGui::GetWindowWidth();
        绘制.winHeith = ImGui::GetWindowHeight();
        g_window = ImGui::GetCurrentWindow();
        ImGui::End(); // 主窗口结束
    }

    drawEnd();
}

void 布局::开启悬浮窗()
{
    timer WindowDrawing;
    WindowDrawing.SetFps(120);
    WindowDrawing.AotuFPS_init();
    WindowDrawing.setAffinity();
    
    // 初始化启动时间
    绘制.启动时间 = std::chrono::steady_clock::now();
    
    while (true)
    {
        // 更新状态信息
        更新状态();
        
        绘制悬浮窗();
        WindowDrawing.SetFps(绘制.按钮.当前帧率);
        WindowDrawing.AotuFPS();
        std::this_thread::sleep_for(1ms);
    }
}

// 在 悬浮窗.cpp 中简化更新状态函数
void 更新状态()
{
    // 更新运行负载（这里用模拟值，你可以替换为实际计算）
    static auto last_time = std::chrono::steady_clock::now();
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_time).count();
    
    // 模拟运行负载变化
    绘制.运行负载 = 10.0f + 5.0f * sin(elapsed / 1000.0f);
    
    // 模拟网络延迟变化
    绘制.网络延迟 = 30 + rand() % 50;
    
    last_time = current_time;
}
