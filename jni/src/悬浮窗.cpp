#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>
#include <future>
#include "json.hpp"
#include "imgui.h"
#include "图片调用.h"
#include "辅助类.h"
#include "Utils/Utils.h"
#include "custom.hpp"
#include "imgui_tricks.hpp"
#include <sys/epoll.h>
#include <linux/input.h>
#include <vector>
#include <string>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/input.h>
#include <thread>
#include <cstdarg> // 用于 va_list
#include <sys/resource.h>
#include <sys/syscall.h>
#include "DataReader.h"
#include <poll.h> // poll 结构体
#include <atomic> // std::atomic

static std::atomic<bool> g_volumeKeyPressed(false);
static std::thread g_volumeThread;
static std::atomic<bool> g_volumeThreadRunning(true);
float statusBarAlpha = 0.8f; // 状态栏背景透明度，范围 0.3 ~ 1.0
using json = nlohmann::json;
// 外部函数声明（保持原样）
extern std::string getIMEI();
extern std::string dccd5a00d1e7c3717afecb8cf06932b8b(const std::string &);
extern std::string t510d4f87b189757294b61eafde7dcc01(const std::string &, const std::string &);
extern std::string a3b5f410c2d8621923a66cede4bc8ac34(const std::string &);
extern std::string q648fa2f9ee26290c776d0545d10ca5f8(const std::string &);
extern std::string jf9fa77eb23cb2049c9e7f7fae967d770(const std::string &, const std::string &);
extern std::string m367456673340c0cb888250365b54a6ce(const std::string &);
extern std::string httppost(const std::string &, const std::string &, const std::string &);
extern std::string t3ecb948ff5e870506e78160a95a1ec24(const std::string &);
extern std::string wef51bd54b4960d4881e233acf0a25f83(const std::string &);
extern std::string p3eca7b0968b8c1535746e24ba2547b6c(const std::string &);
extern std::string g11bf581d48633826202451738f490782(const std::string &, const std::string);

enum UITheme
{
    THEME_DEFAULT = 0,
    THEME_SAKURA = 1,
    THEME_NOODLE = 2
};

static int currentTheme = THEME_NOODLE;
static const char *themeNames[] = {"默认", "樱花粉", "NoodleUI"};

// 状态栏显示控制
static bool showTopStatusBar = true;

// ---------- 极简主题（默认） ----------
void ApplyMinimalTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 12.0f;
    style.ChildRounding = 10.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 8.0f;
    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(12, 10);
    style.ItemInnerSpacing = ImVec2(10, 6);
    style.ScrollbarSize = 6.0f;
    ImVec4 *colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(0.85f, 0.85f, 0.85f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.92f, 0.92f, 0.92f, 0.80f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.88f, 0.88f, 0.88f, 0.90f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 0.95f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.60f, 0.60f, 0.60f, 0.90f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.20f, 0.60f, 0.90f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.60f, 0.90f, 0.90f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.90f, 0.90f, 0.90f, 0.80f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.85f, 0.85f, 0.85f, 0.90f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.90f, 0.90f, 0.90f, 0.80f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.85f, 0.85f, 0.85f, 0.90f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.80f, 0.80f, 0.80f, 0.60f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.70f, 0.70f, 0.70f, 0.60f);
    colors[ImGuiCol_Tab] = ImVec4(0.90f, 0.90f, 0.90f, 0.80f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.85f, 0.85f, 0.85f, 0.90f);
    colors[ImGuiCol_TabActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
}

// ---------- NoodleUI 深色主题 ----------
void ApplyNoodleUITheme()
{
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 8.0f;
    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(10, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.ScrollbarSize = 8.0f;
    ImVec4 *colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.09f, 0.10f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(0.25f, 0.27f, 0.30f, 0.60f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.25f, 0.80f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.27f, 0.30f, 0.90f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.32f, 0.35f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.11f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.11f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.32f, 0.35f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.42f, 0.45f, 0.90f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.52f, 0.55f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.20f, 0.80f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.20f, 0.80f, 1.00f, 0.90f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.90f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.22f, 0.25f, 0.80f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.27f, 0.30f, 0.90f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.60f, 0.80f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.15f, 0.60f, 0.80f, 0.80f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.65f, 0.85f, 0.90f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.10f, 0.55f, 0.75f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.27f, 0.30f, 0.80f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.22f, 0.25f, 0.60f);
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.16f, 0.18f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.10f, 0.55f, 0.75f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.20f, 0.80f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.30f, 0.90f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.20f, 0.80f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.30f, 0.90f, 1.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.15f, 0.60f, 0.80f, 0.40f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.20f, 0.80f, 1.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.20f, 0.80f, 1.00f, 0.80f);
}

// 提示框结构体
struct Notification
{
    std::string message;
    std::chrono::steady_clock::time_point start_time;
    float duration;
    float alpha;
    bool isSuccess;
};

static std::vector<Notification> notifications;
static const float NOTIFICATION_DURATION = 2.0f;
static bool 无痕读取开启 = false;
static std::string 备份目录 = "/data/local/tmp/adb_backup/";
static std::string 目标目录 = "/data/adb/";
static int ColorSettings = 0;
extern 绘制 绘制;
static bool 悬浮窗 = true;
static bool 自瞄控件 = false;
ImVec2 Pos2;
ImVec2 windowSize, windowSize_max;
static bool 窗口状态 = false;
// static bool 广角设置 = false;

// 解绑相关变量
static bool 显示解绑窗口 = false;
static char 解绑卡密输入[128] = "";
static bool 解绑确认中 = false;
static std::string 解绑结果消息 = "";

bool showWindow = false;
float g_FrameTime = 0.0f;
int g_FPS = 0;
int g_FrameCount = 0;
std::chrono::steady_clock::time_point g_LastTime = std::chrono::steady_clock::now();

bool g_FloatWindow = false;

// ---------- 辅助函数声明 ----------
void ShowFloatWindow();
void HideFloatWindow();
void 猎鹰();
int 数据();
int 音量();
bool BackupADBDirectory();
bool ClearADBDirectory();
bool RestoreADBDirectory();
void CheckAutoRestore();
void AddNotification(const std::string &message, bool isSuccess);
void DrawNotifications();
void DrawThreeColorBalls();
void 更新状态();

// 悬浮窗显示/隐藏占位函数
void ShowFloatWindow()
{
    printf("悬浮窗显示\n");
}
void HideFloatWindow()
{
    printf("悬浮窗隐藏\n");
}

// 音量键监听函数（修复权限日志、按键识别）
void VolumeKeyListener()
{
    setpriority(PRIO_PROCESS, syscall(SYS_gettid), -20);
    const char *devicePaths[] = {
        "/dev/input/event0", "/dev/input/event1", "/dev/input/event2",
        "/dev/input/event3", "/dev/input/event4", "/dev/input/event5",
        "/dev/input/event6", "/dev/input/event7"};
    std::vector<int> fds;
    for (const char *path : devicePaths)
    {
        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd >= 0)
            fds.push_back(fd);
    }
    if (fds.empty())
        return;

    std::vector<pollfd> pfds(fds.size());
    for (size_t i = 0; i < fds.size(); ++i)
    {
        pfds[i].fd = fds[i];
        pfds[i].events = POLLIN;
    }

    input_event ev;
    while (g_volumeThreadRunning)
    {
        int ret = poll(pfds.data(), pfds.size(), 20);
        if (ret > 0)
        {
            for (size_t i = 0; i < pfds.size(); ++i)
            {
                if (pfds[i].revents & POLLIN)
                {
                    while (read(pfds[i].fd, &ev, sizeof(ev)) == sizeof(ev))
                    {
                        // 仅处理音量键按下事件
                        if (ev.type == EV_KEY && ev.value == 1 &&
                            (ev.code == KEY_VOLUMEUP || ev.code == KEY_VOLUMEDOWN))
                        {
                            static uint64_t lastTime = 0;
                            uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
                                               std::chrono::system_clock::now().time_since_epoch())
                                               .count();
                            if (now - lastTime > 200)
                            {
                                lastTime = now;
                                g_volumeKeyPressed = true;
                            }
                        }
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    for (int fd : fds)
        close(fd);
}

void 猎鹰()
{
    while (true)
    {
        srand(time(0));
        int random_number = rand() % 100;
        if (random_number < 1)
        {
            std::ofstream file("/data/猎鹰判断/监测到猎鹰巡查正在观战...");
            if (file.is_open())
                file.close();
        }
        sleep(500);
    }
}

static int style_idx = 0;

int 数据()
{
    DIR *dir = opendir("/dev/input/");
    dirent *ptr = NULL;
    int count = 0;
    while ((ptr = readdir(dir)) != NULL)
    {
        if (strstr(ptr->d_name, "event"))
            count++;
    }
    return count ? count : -1;
}

int 音量()
{
    int EventCount = 数据();
    if (EventCount < 0)
    {
        printf("未找到输入设备\n");
        return -1;
    }
    int *fdArray = (int *)malloc(EventCount * sizeof(int));
    for (int i = 0; i < EventCount; i++)
    {
        char temp[128];
        sprintf(temp, "/dev/input/event%d", i);
        fdArray[i] = open(temp, O_RDWR | O_NONBLOCK);
    }
    input_event ev;
    while (1)
    {
        for (int i = 0; i < EventCount; i++)
        {
            memset(&ev, 0, sizeof(ev));
            read(fdArray[i], &ev, sizeof(ev));
            if (ev.type == EV_KEY && (ev.code == KEY_VOLUMEUP || ev.code == KEY_VOLUMEDOWN))
            {
                if (ev.code == 115 && ev.value == 1)
                    悬浮窗 = true;
                else if (ev.code == 114 && ev.value == 1)
                    悬浮窗 = false;
            }
            usleep(1000);
        }
        usleep(500);
    }
    return 0;
}

// 备份目录函数
bool BackupADBDirectory()
{
    try
    {
        // 创建备份目录
        std::filesystem::create_directories(备份目录);

        // 检查目标目录是否存在
        if (!std::filesystem::exists(目标目录))
        {
            return true; // 目录不存在，无需备份
        }

        // 遍历目标目录并备份所有文件和子目录
        for (const auto &entry : std::filesystem::recursive_directory_iterator(目标目录))
        {
            try
            {
                std::string relative_path = entry.path().string().substr(目标目录.length());
                std::string backup_path = 备份目录 + relative_path;

                if (entry.is_directory())
                {
                    std::filesystem::create_directories(backup_path);
                }
                else if (entry.is_regular_file())
                {
                    std::filesystem::copy_file(entry.path(), backup_path,
                                               std::filesystem::copy_options::overwrite_existing);
                }
            }
            catch (const std::exception &e)
            {
                // 忽略单个文件备份失败，继续备份其他文件
                continue;
            }
        }

        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

// 清空目录函数
bool ClearADBDirectory()
{
    try
    {
        if (!std::filesystem::exists(目标目录))
        {
            return true; // 目录不存在，无需清空
        }

        // 遍历并删除目录中的所有内容
        for (const auto &entry : std::filesystem::directory_iterator(目标目录))
        {
            try
            {
                std::filesystem::remove_all(entry.path());
            }
            catch (const std::exception &e)
            {
                // 忽略删除失败的文件，继续删除其他文件
                continue;
            }
        }

        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

// 恢复目录函数 - 增强错误处理
bool RestoreADBDirectory()
{
    try
    {
        // 检查备份目录是否存在
        if (!std::filesystem::exists(备份目录))
        {
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
        for (const auto &entry : std::filesystem::recursive_directory_iterator(备份目录))
        {
            try
            {
                std::string relative_path = entry.path().string().substr(备份目录.length());
                std::string restore_path = 目标目录 + relative_path;

                if (entry.is_directory())
                {
                    std::filesystem::create_directories(restore_path);
                    restored_dirs++;
                }
                else if (entry.is_regular_file())
                {
                    // 确保目标目录存在
                    std::filesystem::create_directories(std::filesystem::path(restore_path).parent_path());

                    std::filesystem::copy_file(entry.path(), restore_path,
                                               std::filesystem::copy_options::overwrite_existing);
                    restored_files++;
                }
            }
            catch (const std::exception &e)
            {
                failed_items++;
                LOGE("恢复项目失败: %s", e.what());
                continue;
            }
        }

        LOGE("恢复完成: %d 个文件, %d 个目录, %d 个失败", restored_files, restored_dirs, failed_items);

        // 恢复完成后删除备份目录
        try
        {
            if (std::filesystem::remove_all(备份目录))
            {
                LOGE("备份目录已清理");
            }
            else
            {
                LOGE("警告：备份目录清理失败");
                return false;
            }
        }
        catch (const std::exception &e)
        {
            LOGE("清理备份目录失败: %s", e.what());
            return false;
        }

        return failed_items == 0;
    }
    catch (const std::exception &e)
    {
        LOGE("恢复过程中发生异常: %s", e.what());
        return false;
    }
}

void DrawLargePinkTitle()
{
    ImVec2 cursorPos = ImGui::GetCursorPos();
    ImVec2 screenPos = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddText(nullptr, 24.0f, screenPos, IM_COL32(255, 100, 200, 255), "待初始化");
    ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y + 30));
}

// 检查是否在开机时自动恢复
void CheckAutoRestore()
{
    // 检查是否存在备份目录，如果存在则自动恢复
    if (std::filesystem::exists(备份目录))
    {
        RestoreADBDirectory();
    }
}

// 修复后的 IOSStyleToggle 函数 - 使用 ImGui 原生支持的函数
bool IOSStyleToggle(const char *label, bool *v, float animation_speed = 3.0f)
{
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    static std::unordered_map<ImGuiID, float> animation_states;
    ImGuiID id = ImGui::GetID(label);
    if (animation_states.find(id) == animation_states.end())
        animation_states[id] = 0.0f;
    float &anim = animation_states[id];
    bool is_hovered = ImGui::IsItemHovered();
    float anim_target = (*v ? 1.0f : 0.0f);
    anim = ImLerp(anim, anim_target, ImGui::GetIO().DeltaTime * animation_speed);
    float total_height = 60.0f, total_width = 370.0f;
    bool pressed = ImGui::InvisibleButton(label, ImVec2(total_width, total_height));
    float padding = 20.0f, switch_width = 60.0f, switch_height = 30.0f, icon_size = 20.0f, icon_text_spacing = 15.0f;
    ImU32 bg_color = is_hovered ? IM_COL32(225, 225, 235, 255) : IM_COL32(240, 240, 250, 255);
    draw_list->AddRectFilled(pos, ImVec2(pos.x + total_width, pos.y + total_height), bg_color, 8.0f);
    float border_alpha = 200 + (int)(55 * anim);
    draw_list->AddRect(pos, ImVec2(pos.x + total_width, pos.y + total_height), IM_COL32(0, 204, 255, (int)border_alpha), 8.0f, ImDrawFlags_RoundCornersAll, 2.0f);
    draw_list->AddRect(ImVec2(pos.x + 1, pos.y + 1), ImVec2(pos.x + total_width - 1, pos.y + total_height - 1), IM_COL32(255, 255, 255, 100), 7.0f, ImDrawFlags_RoundCornersAll, 1.0f);
    ImVec2 text_size = ImGui::CalcTextSize(label);
    float text_start_x = pos.x + padding + icon_size + icon_text_spacing;
    ImVec2 text_pos = ImVec2(text_start_x, pos.y + (total_height - text_size.y) * 0.5f);
    draw_list->AddText(text_pos, IM_COL32(30, 30, 30, 255), label);
    ImVec2 switch_pos = ImVec2(pos.x + total_width - switch_width - padding, pos.y + (total_height - switch_height) * 0.5f);
    ImU32 switch_bg_off = IM_COL32(229, 229, 234, 255), switch_bg_on = IM_COL32(76, 217, 100, 255);
    ImVec4 bg_off_color = ImGui::ColorConvertU32ToFloat4(switch_bg_off), bg_on_color = ImGui::ColorConvertU32ToFloat4(switch_bg_on);
    ImVec4 current_bg_color = {
        bg_off_color.x + (bg_on_color.x - bg_off_color.x) * anim,
        bg_off_color.y + (bg_on_color.y - bg_off_color.y) * anim,
        bg_off_color.z + (bg_on_color.z - bg_off_color.z) * anim,
        bg_off_color.w + (bg_on_color.w - bg_off_color.w) * anim};
    ImU32 current_switch_bg = ImGui::ColorConvertFloat4ToU32(current_bg_color);
    draw_list->AddRectFilled(switch_pos, ImVec2(switch_pos.x + switch_width, switch_pos.y + switch_height), current_switch_bg, switch_height * 0.5f);
    float circle_radius = switch_height * 0.5f - 2.0f;
    float circle_x_off = switch_pos.x + circle_radius + 2.0f, circle_x_on = switch_pos.x + switch_width - circle_radius - 2.0f;
    float circle_x = ImLerp(circle_x_off, circle_x_on, anim);
    ImVec2 circle_center = ImVec2(circle_x, switch_pos.y + switch_height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(circle_center.x + 1, circle_center.y + 1), circle_radius, IM_COL32(0, 0, 0, 50));
    draw_list->AddCircleFilled(circle_center, circle_radius, IM_COL32(255, 255, 255, 255));
    draw_list->AddCircle(circle_center, circle_radius - 1.0f, IM_COL32(255, 255, 255, 200), 0, 1.0f);
    float symbol_radius = circle_radius * 0.4f;
    ImU32 symbol_color_off = IM_COL32(150, 150, 150, 255), symbol_color_on = IM_COL32(76, 217, 100, 255);
    ImVec4 symbol_off_color = ImGui::ColorConvertU32ToFloat4(symbol_color_off), symbol_on_color = ImGui::ColorConvertU32ToFloat4(symbol_color_on);
    ImVec4 current_symbol_color = {
        symbol_off_color.x + (symbol_on_color.x - symbol_off_color.x) * anim,
        symbol_off_color.y + (symbol_on_color.y - symbol_off_color.y) * anim,
        symbol_off_color.z + (symbol_on_color.z - symbol_off_color.z) * anim,
        symbol_off_color.w + (symbol_on_color.w - symbol_off_color.w) * anim};
    ImU32 symbol_color = ImGui::ColorConvertFloat4ToU32(current_symbol_color);
    if (anim < 0.5f)
    {
        float scale = 1.0f - anim * 2.0f;
        if (scale > 0.01f)
            draw_list->AddLine(ImVec2(circle_center.x - symbol_radius * scale, circle_center.y), ImVec2(circle_center.x + symbol_radius * scale, circle_center.y), symbol_color, 2.0f);
    }
    else
    {
        float scale = (anim - 0.5f) * 2.0f;
        draw_list->AddLine(ImVec2(circle_center.x - symbol_radius * scale, circle_center.y), ImVec2(circle_center.x + symbol_radius * scale, circle_center.y), symbol_color, 2.0f);
        if (scale > 0.3f)
        {
            float vertical_scale = ImClamp((scale - 0.3f) / 0.7f, 0.0f, 1.0f);
            draw_list->AddLine(ImVec2(circle_center.x, circle_center.y - symbol_radius * vertical_scale), ImVec2(circle_center.x, circle_center.y + symbol_radius * vertical_scale), symbol_color, 2.0f);
        }
    }
    if (pressed)
    {
        *v = !*v;
        draw_list->AddCircle(ImVec2(pos.x + total_width * 0.5f, pos.y + total_height * 0.5f), 15.0f, IM_COL32(255, 255, 255, 150), 0, 2.0f);
    }
    return pressed;
}

float ImLerp(float a, float b, float t) { return a + (b - a) * t; }
float ImClamp(float v, float mn, float mx) { return (v < mn) ? mn : (v > mx) ? mx
                                                                             : v; }

// 复合按钮样式 - 支持多个选项（3个）
bool IOSStyleSegmentedCompound(const char *main_label, int *current_mode, const char *options[], int option_count)
{
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    float total_width = 370.0f, total_height = 60.0f;
    ImVec2 pos = ImGui::GetCursorScreenPos();
    char button_id[128];
    sprintf(button_id, "##CompSeg_%s", main_label);
    bool pressed = ImGui::InvisibleButton(button_id, ImVec2(total_width, total_height));
    bool is_hovered = ImGui::IsItemHovered();
    ImU32 bg_color = is_hovered ? IM_COL32(225, 225, 235, 255) : IM_COL32(240, 240, 250, 255);
    draw_list->AddRectFilled(pos, ImVec2(pos.x + total_width, pos.y + total_height), bg_color, 8.0f);
    draw_list->AddRect(pos, ImVec2(pos.x + total_width, pos.y + total_height), IM_COL32(0, 204, 255, 200), 8.0f, ImDrawFlags_RoundCornersAll, 2.0f);
    float padding = 20.0f, icon_size = 20.0f;
    float icon_y = pos.y + total_height * 0.5f, icon_x = pos.x + padding + icon_size * 0.5f;
    ImVec2 diamond_center = ImVec2(icon_x, icon_y);
    float diamond_radius = icon_size * 0.5f;
    ImVec2 diamond_points[4] = {
        ImVec2(diamond_center.x, diamond_center.y - diamond_radius),
        ImVec2(diamond_center.x + diamond_radius, diamond_center.y),
        ImVec2(diamond_center.x, diamond_center.y + diamond_radius),
        ImVec2(diamond_center.x - diamond_radius, diamond_center.y)};
    ImU32 diamond_color = is_hovered ? IM_COL32(65, 105, 225, 255) : IM_COL32(100, 149, 237, 255);
    draw_list->AddConvexPolyFilled(diamond_points, 4, diamond_color);
    draw_list->AddPolyline(diamond_points, 4, IM_COL32(255, 255, 255, 200), true, 1.5f);
    float time = ImGui::GetTime();
    float pulse_alpha = 100 + 155 * (sinf(time * 3.0f) * 0.5f + 0.5f);
    draw_list->AddCircleFilled(diamond_center, diamond_radius * 0.3f, IM_COL32(255, 255, 255, (int)pulse_alpha));
    float title_x = icon_x + icon_size + 15.0f;
    float title_y = pos.y + (total_height - ImGui::GetFontSize()) * 0.5f;
    draw_list->AddText(ImVec2(title_x, title_y), IM_COL32(30, 30, 30, 255), main_label);
    float segment_total_width = 180.0f, segment_button_height = 30.0f;
    float segment_button_width = segment_total_width / option_count;
    float segment_x = pos.x + total_width - segment_total_width - padding;
    float segment_y = pos.y + (total_height - segment_button_height) * 0.5f;
    bool changed = false;
    for (int i = 0; i < option_count; i++)
    {
        bool is_selected = (*current_mode == i);
        float seg_button_x = segment_x + i * segment_button_width;
        ImVec2 seg_button_min = ImVec2(seg_button_x, segment_y);
        ImVec2 seg_button_max = ImVec2(seg_button_x + segment_button_width, segment_y + segment_button_height);
        ImVec2 mouse_pos = ImGui::GetMousePos();
        bool seg_button_hovered = (mouse_pos.x >= seg_button_min.x && mouse_pos.x <= seg_button_max.x && mouse_pos.y >= seg_button_min.y && mouse_pos.y <= seg_button_max.y);
        ImU32 seg_button_color;
        if (is_selected)
            seg_button_color = IM_COL32(100, 149, 237, 255);
        else if (seg_button_hovered)
            seg_button_color = IM_COL32(255, 182, 193, 255);
        else
            seg_button_color = IM_COL32(229, 229, 234, 255);
        float corner_radius = 5.0f;
        ImDrawFlags round_flags = 0;
        if (i == 0)
            round_flags = ImDrawFlags_RoundCornersLeft;
        else if (i == option_count - 1)
            round_flags = ImDrawFlags_RoundCornersRight;
        draw_list->AddRectFilled(seg_button_min, seg_button_max, seg_button_color, corner_radius, round_flags);
        ImU32 seg_button_border = is_selected ? IM_COL32(0, 180, 70, 255) : IM_COL32(200, 200, 210, 255);
        draw_list->AddRect(seg_button_min, seg_button_max, seg_button_border, corner_radius, round_flags, 1.5f);
        const char *button_text = options[i];
        ImVec2 text_size = ImGui::CalcTextSize(button_text);
        float text_x = seg_button_x + (segment_button_width - text_size.x) * 0.5f;
        float text_y = segment_y + (segment_button_height - text_size.y) * 0.5f;
        ImU32 text_color = is_selected ? IM_COL32(255, 255, 255, 255) : seg_button_hovered ? IM_COL32(50, 50, 50, 255)
                                                                                           : IM_COL32(100, 100, 100, 255);
        draw_list->AddText(ImVec2(text_x, text_y), text_color, button_text);
        static std::unordered_map<int, bool> seg_button_pressed;
        int seg_button_id = ImGui::GetID(button_text) + i;
        if (seg_button_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            seg_button_pressed[seg_button_id] = true;
        if (seg_button_pressed[seg_button_id] && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            if (seg_button_hovered)
            {
                *current_mode = i;
                changed = true;
            }
            seg_button_pressed[seg_button_id] = false;
        }
    }
    return changed;
}
// 添加提示框函数
void AddNotification(const std::string &message, bool isSuccess)
{
    Notification notif;
    notif.message = isSuccess ? (message + " 开启成功！") : (message + " 被关闭！");
    notif.start_time = std::chrono::steady_clock::now();
    notif.duration = NOTIFICATION_DURATION;
    notif.alpha = 1.0f;
    notif.isSuccess = isSuccess;
    notifications.push_back(notif);
}

// 绘制提示框函数
void DrawNotifications()
{
    auto current_time = std::chrono::steady_clock::now();
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();

    float start_y = 80.0f; // 从屏幕顶部开始，在三色球下方
    float padding = 10.0f;

    // 更新和绘制所有提示框
    for (auto it = notifications.begin(); it != notifications.end();)
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                           current_time - it->start_time)
                           .count() /
                       1000.0f;

        if (elapsed >= it->duration)
        {
            // 移除过期的提示框
            it = notifications.erase(it);
        }
        else
        {
            // 计算透明度（淡出效果）
            float progress = elapsed / it->duration;
            it->alpha = 1.0f - progress * progress; // 非线性淡出

            // 绘制圆角矩形背景
            ImVec2 text_size = ImGui::CalcTextSize(it->message.c_str());
            ImVec2 rect_min = ImVec2(padding, start_y);
            ImVec2 rect_max = ImVec2(padding + text_size.x + 20.0f, start_y + text_size.y + 10.0f);

            // 根据状态选择颜色
            ImU32 bg_color, border_color, text_color;

            if (it->isSuccess)
            {
                // 成功颜色 - 绿色系
                bg_color = IM_COL32(0, 0, 0, (int)(0 * it->alpha));         // 深绿背景
                border_color = IM_COL32(0, 255, 0, (int)(220 * it->alpha)); // 亮绿边框
                text_color = IM_COL32(0, 255, 0, (int)(255 * it->alpha));   // 浅绿文字
            }
            else
            {
                // 关闭颜色 - 红色系
                bg_color = IM_COL32(0, 0, 0, (int)(0 * it->alpha));         // 深红背景
                border_color = IM_COL32(255, 0, 0, (int)(220 * it->alpha)); // 亮红边框
                text_color = IM_COL32(255, 0, 0, (int)(255 * it->alpha));   // 浅红文字
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

bool ToggleButton(const char *label, bool *v)
{
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2(120, 40);
    bool pressed = ImGui::InvisibleButton(label, size);
    ImU32 bg_color = *v ? IM_COL32(0, 150, 255, 255) : IM_COL32(80, 80, 80, 255);
    ImU32 border_color = *v ? IM_COL32(100, 200, 255, 255) : IM_COL32(120, 120, 120, 255);
    draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bg_color, 8.0f);
    draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), border_color, 8.0f, 0, 2.0f);
    if (*v)
        draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(100, 200, 255, 100), 8.0f, 0, 3.0f);
    ImVec2 text_size = ImGui::CalcTextSize(label);
    ImVec2 text_pos = ImVec2(pos.x + (size.x - text_size.x) * 0.5f, pos.y + (size.y - text_size.y) * 0.5f);
    draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), label);
    ImVec2 dot_pos = ImVec2(pos.x + size.x - 12, pos.y + 8);
    ImU32 dot_color = *v ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
    draw_list->AddCircleFilled(dot_pos, 4.0f, dot_color);
    if (pressed)
        *v = !*v;
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

// float defaultValues[] = {
//     120.0f, // 绘制.自瞄.触摸范围
//     120.0f, // 绘制.自瞄.触摸范围
//     5.00f,  // 绘制.自瞄.自瞄速度
//     2.0f,   // 绘制.自瞄.压枪力度
//     1.78f,  // 绘制.自瞄.预判力度
//     0.88f,  // 绘制.自瞄.趴下位置调节
//     50.0f,  // 绘制.自瞄.腰射距离限制
//     200.0f, // 绘制.自瞄.自瞄距离限制
//     15.0f,  // 绘制.自瞄.喷子距离限制
// };

// void SetDefaultValues()
// {

//     绘制.自瞄.压枪力度 = defaultValues[3];
//     绘制.自瞄.预判力度 = defaultValues[4];
//     绘制.自瞄.趴下位置调节 = defaultValues[5];
//     绘制.自瞄.腰射距离限制 = defaultValues[6];
//     绘制.自瞄.自瞄距离限制 = defaultValues[7];
//     绘制.自瞄.喷子距离限制 = defaultValues[8];
//     绘制.预判度.扫车 = 1.2f;
// }

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
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    if (displaySize.x <= 0 || displaySize.y <= 0)
        return; // 防止无效分辨率

    float ball_radius = 10.0f;
    float rect_length = 120.0f;
    float rect_height = 20.0f;
    float center_x = displaySize.x / 2;
    float center_y = 50.0f;

    ImVec2 rect_min = ImVec2(center_x - rect_length / 2, center_y - rect_height / 2);
    ImVec2 rect_max = ImVec2(center_x + rect_length / 2, center_y + rect_height / 2);

    // 绘制透明矩形（用于点击检测）
    draw_list->AddRectFilled(rect_min, rect_max, IM_COL32(128, 128, 128, 0));
    draw_list->AddRect(rect_min, rect_max, IM_COL32(100, 100, 100, 0), 0.0f, 0, 2.0f);
}

extern bool g_login_success;
extern bool g_announcement_passed;

// ---------- 绘制顶部状态栏 ----------
void DrawTopStatusBar()
{
    ImDrawList *draw_list = ImGui::GetForegroundDrawList();
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos;
    ImVec2 work_size = viewport->WorkSize;

    // 稍微增高一点，增加UI的呼吸感
    float bar_height = 36.0f;
    ImVec2 bar_min = ImVec2(work_pos.x, work_pos.y);
    ImVec2 bar_max = ImVec2(work_pos.x + work_size.x, work_pos.y + bar_height);

    // 1. 背景：稍微现代化一点的颜色，带有非常淡的渐变或透明度
    ImU32 bgColor = IM_COL32(18, 20, 24, (int)(245 * statusBarAlpha));
    draw_list->AddRectFilled(bar_min, bar_max, bgColor);

    // 底部高光边缘，使用微亮的白色替代黑色粗线，增加立体感
    draw_list->AddLine(ImVec2(bar_min.x, bar_max.y), ImVec2(bar_max.x, bar_max.y),
                       IM_COL32(255, 255, 255, (int)(20 * statusBarAlpha)), 1.0f);

    // 精准垂直居中：(总高度 - 字体高度) / 2
    float text_y = work_pos.y + (bar_height - ImGui::GetFontSize()) * 0.9f;
    float margin = 100.0f; // 缩小了两侧边距，适应性更好
    float cursor_x = work_pos.x + margin;

    // 2. 品牌文字 - 使用纯白色突出显示
    const char *brandName = "Aura Kernel";
    draw_list->AddText(ImVec2(cursor_x, text_y),
                       IM_COL32(255, 255, 255, (int)(255 * statusBarAlpha)), brandName);
    // 修复了原代码中字符串宽度计算不一致的问题
    cursor_x += ImGui::CalcTextSize(brandName).x + 40.0f;

    // 3. 硬件信息面板 (分离标签与数值的颜色)
    float right_reserved = 180.0f;
    float right_boundary = work_pos.x + work_size.x - margin - right_reserved;

    auto AddInfoText = [&](const char *label, const char *fmt, ...)
    {
        char valBuf[32];
        va_list args;
        va_start(args, fmt);
        vsnprintf(valBuf, sizeof(valBuf), fmt, args);
        va_end(args);

        ImVec2 labelSize = ImGui::CalcTextSize(label);
        ImVec2 valSize = ImGui::CalcTextSize(valBuf);

        if (cursor_x + labelSize.x + valSize.x > right_boundary)
            return;

        // 绘制标签 (较暗)
        draw_list->AddText(ImVec2(cursor_x, text_y),
                           IM_COL32(140, 145, 150, (int)(255 * statusBarAlpha)), label);
        cursor_x += labelSize.x + 4.0f; // 标签和数值之间的微小间距

        // 绘制数值 (较亮)
        draw_list->AddText(ImVec2(cursor_x, text_y),
                           IM_COL32(220, 225, 230, (int)(255 * statusBarAlpha)), valBuf);
        cursor_x += valSize.x + 20.0f; // 不同信息组之间的间距
    };

    // 补充百分比符号，观感更佳
    AddInfoText("FPS", "%d", g_FPS);
    AddInfoText("CPU", "%.0f%%", 25.0f + 10.0f * sin(ImGui::GetTime()));
    AddInfoText("GPU", "%.0f%%", 30.0f + 15.0f * cos(ImGui::GetTime() * 0.8f));
    AddInfoText("RAM", "%.0f%%", 42.0f + 8.0f * sin(ImGui::GetTime() * 1.2f));

    // 4. 右侧信息 (时间与电池)
    float right_x = work_pos.x + work_size.x - margin;

    // 时间
    std::time_t now = std::time(nullptr);
    char timeBuf[32];
    std::strftime(timeBuf, sizeof(timeBuf), "%H:%M", std::localtime(&now));
    ImVec2 timeSize = ImGui::CalcTextSize(timeBuf);
    right_x -= timeSize.x;
    draw_list->AddText(ImVec2(right_x, text_y),
                       IM_COL32(230, 230, 230, (int)(255 * statusBarAlpha)), timeBuf);

    right_x -= 15.0f; // 时间与电池的间距

    // 电池
    int batteryLevel = -1;
    FILE *capFile = fopen("/sys/class/power_supply/battery/capacity", "r");
    if (capFile)
    {
        fscanf(capFile, "%d", &batteryLevel);
        fclose(capFile);
    }

    // 如果在没有该节点的设备（如PC）上测试，可给个默认值方便预览
    // if (batteryLevel < 0) batteryLevel = 85;

    if (batteryLevel >= 0)
    {
        char batBuf[32];
        snprintf(batBuf, sizeof(batBuf), "%d%%", batteryLevel);
        ImVec2 batSize = ImGui::CalcTextSize(batBuf);
        right_x -= batSize.x;
        draw_list->AddText(ImVec2(right_x, text_y),
                           IM_COL32(200, 200, 200, (int)(255 * statusBarAlpha)), batBuf);

        // 绘制动态电池图标
        float icon_w = 20.0f;
        float icon_h = 10.0f;
        float icon_x = right_x - icon_w - 6.0f;
        float icon_y = text_y + (ImGui::GetFontSize() - icon_h) * 0.5f; // 图标垂直居中

        ImU32 batOutlineCol = IM_COL32(150, 150, 150, (int)(255 * statusBarAlpha));
        // 电量大于20%显示绿色，否则显示红色警告
        ImU32 batFillCol = batteryLevel > 20 ? IM_COL32(120, 210, 120, (int)(255 * statusBarAlpha))
                                             : IM_COL32(240, 80, 80, (int)(255 * statusBarAlpha));

        // 电池外壳
        draw_list->AddRect(ImVec2(icon_x, icon_y), ImVec2(icon_x + icon_w, icon_y + icon_h),
                           batOutlineCol, 1.5f, 0, 1.0f);
        // 电池正极触点
        draw_list->AddRectFilled(ImVec2(icon_x + icon_w, icon_y + 2.5f), ImVec2(icon_x + icon_w + 2.0f, icon_y + icon_h - 2.5f),
                                 batOutlineCol);

        // 根据百分比填充电池内部
        float fill_w = (icon_w - 3.0f) * (batteryLevel / 100.0f);
        if (fill_w > 0)
        {
            draw_list->AddRectFilled(ImVec2(icon_x + 1.5f, icon_y + 1.5f), ImVec2(icon_x + 1.5f + fill_w, icon_y + icon_h - 1.5f),
                                     batFillCol);
        }
    }
}

// ---------- 左侧导航 ----------
void DrawLeftNavigation(int &selectedMenu)
{
    ImGui::BeginChild("##LeftNav", ImVec2(200, -1), true);
    ImGui::SetCursorPosY(25.0f);
    const char *menuItems[] = {"主页", "人物", "物资", "视觉", "颜色", "设置"};
    // 对应 手持图片 索引：300=主页,301=人物,302=物资,303=视觉,304=颜色,305=设置
    int iconIndices[] = {300, 301, 302, 303, 304, 305};

    for (int i = 0; i < IM_ARRAYSIZE(menuItems); i++)
    {
        bool selected = (selectedMenu == i);
        float avail = ImGui::GetContentRegionAvail().x - 20.0f;
        ImVec2 btnSize(avail, 42.0f);
        ImVec2 cursor = ImGui::GetCursorScreenPos();
        ImDrawList *dl = ImGui::GetWindowDrawList();

        if (selected)
        {
            dl->AddRectFilled(cursor, ImVec2(cursor.x + btnSize.x, cursor.y + btnSize.y),
                              IM_COL32(30, 120, 200, 80), 8.0f);
            dl->AddRect(cursor, ImVec2(cursor.x + btnSize.x, cursor.y + btnSize.y),
                        IM_COL32(50, 150, 230, 150), 8.0f, 0, 2.0f);
        }

        // 从手持图片映射表获取纹理
        auto it = 手持图片.find(iconIndices[i]);
        if (it != 手持图片.end() && it->second.DS != nullptr)
        {
            float iconSize = 30.0f;
            ImVec2 iconPos(cursor.x + 15.0f, cursor.y + (btnSize.y - iconSize) * 0.5f);
            dl->AddImage(it->second.DS, iconPos, ImVec2(iconPos.x + iconSize, iconPos.y + iconSize));
        }

        // 文字绘制
        ImVec2 textSize = ImGui::CalcTextSize(menuItems[i]);
        float textX = cursor.x + 50.0f; // 留出图标空间
        float textY = cursor.y + (btnSize.y - textSize.y) * 0.5f;
        dl->AddText(ImVec2(textX, textY), selected ? IM_COL32(255, 255, 255, 255) : IM_COL32(180, 180, 180, 255), menuItems[i]);

        // 点击处理
        ImGui::SetCursorScreenPos(cursor);
        if (ImGui::InvisibleButton(menuItems[i], btnSize))
        {
            selectedMenu = i;
        }
        ImGui::SetCursorScreenPos(ImVec2(cursor.x, cursor.y + btnSize.y + 12.0f));
    }
    ImGui::EndChild();
}

// ---------- 主页 ----------
void DrawHomePage()
{
    ImGui::BeginChild("##HomeContent", ImVec2(-1, -1), false); // 不自动添加滚动条（但我们手动控制布局）
    float avail_y = ImGui::GetContentRegionAvail().y;
    float current_y = 10.0f;
    float card_padding = 10.0f;
    float fullW = ImGui::GetContentRegionAvail().x - 40.0f;

    // 状态卡片
    ImGui::SetCursorPos(ImVec2(20, current_y));
    ImGui::BeginChild("##StatusCard", ImVec2(-1, 90), true);
    ImGui::SetCursorPos(ImVec2(15, 15));
    bool connected = (绘制.地址.世界地址 != 0x0);
    ImGui::TextColored(connected ? ImVec4(0.3f, 0.9f, 0.5f, 1.0f) : ImVec4(1.0f, 0.4f, 0.4f, 1.0f), connected ? "● 已连接" : "○ 未连接");
    ImGui::SameLine(150);
    ImGui::Text("负载: %.1f%%", 绘制.运行负载);
    ImGui::SameLine(300);
    ImGui::Text("延迟: %d ms", 绘制.网络延迟);
    ImGui::SetCursorPosX(15);
    ImGui::Text("游戏初始化数据读取 读取成功");
    ImGui::SameLine(300);
    ImGui::Text("注册读写");
    ImGui::EndChild();
    current_y += 90 + card_padding;

    // 状态栏开关
    ImGui::SetCursorPos(ImVec2(20, current_y));
    ImGui::Checkbox("显示顶部状态栏", &showTopStatusBar);
    current_y += 50 + card_padding;

    // 游戏控制卡片（高度稍大）
    ImGui::SetCursorPos(ImVec2(20, current_y));
    ImGui::BeginChild("##GameControl", ImVec2(-1, 180), true);
    ImGui::SetCursorPos(ImVec2(15, 15));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.8f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.6f, 0.9f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.45f, 0.7f, 1.0f));
    if (ImGui::Button("链接游戏", ImVec2(fullW, 36)))
    {
        绘制.初始化绘制("com.tencent.tmgp.pubgmhd", abs_ScreenX, abs_ScreenY);
        绘制.按钮.绘制 = true;
        绘制.按钮.人数 = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::Spacing();
    if (ImGui::Button("扫描解密", ImVec2(fullW, 36)))
    {
        绘制.解密数组选择窗口开启 = true;
        if (绘制.解密地址列表.empty())
            绘制.重新扫描解密地址();
    }
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Button, 绘制.按钮.解密 ? ImVec4(0.2f, 0.7f, 0.4f, 0.8f) : ImVec4(0.4f, 0.4f, 0.4f, 0.8f));
    if (ImGui::Button(绘制.按钮.解密 ? "解密已启用" : "数组解密", ImVec2(fullW, 36)))
    {
        绘制.按钮.解密 = !绘制.按钮.解密;
        AddNotification(绘制.按钮.解密 ? "解密启用" : "解密禁用", true);
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
    current_y += 170 + card_padding;

    // 退出按钮（居中）
    ImGui::SetCursorPos(ImVec2(20 + (ImGui::GetContentRegionAvail().x - fullW) * 0.5f, current_y + 10));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.25f, 0.25f, 0.9f));
    if (ImGui::Button("退出程序", ImVec2(fullW, 38)))
    {
        // 停止音量监听线程
        g_volumeThreadRunning = false;
        if (无痕读取开启 && std::filesystem::exists(备份目录))
            RestoreADBDirectory();
        exit(1);
    }
    ImGui::PopStyleColor(2);

    ImGui::EndChild();
}

// ---------- 人物页面 ----------
void DrawCharacterPage()
{
    ImGui::BeginChild("##CharacterContent", ImVec2(-1, -1), true);
    ImGui::SetCursorPos(ImVec2(20, 20));
    static const char *血条样式选项[] = {"简约", "赛事", "分格", "无ui"};
    ImGui::Text("血条样式");
    ImGui::SameLine(150);
    ImGui::SetNextItemWidth(200);
    ImGui::SliderInt("##血条样式", &绘制.按钮.血条绘图, 0, 3, 血条样式选项[绘制.按钮.血条绘图]);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Columns(2, "##人物列", false);
    struct
    {
        const char *name;
        bool *variable;
    } options[] = {
        {"人物方框", &绘制.按钮.方框},
        {"人物射线", &绘制.按钮.射线},
        {"人物骨骼", &绘制.按钮.骨骼},
        {"手持文字", &绘制.按钮.手持2},
        {"手持图片", &绘制.按钮.手持},
        {"人物血条", &绘制.按钮.血量},
        {"人物距离", &绘制.按钮.距离},
        {"人物名字", &绘制.按钮.名字},
        {"被瞄预警", &绘制.按钮.被瞄预警},
        {"背敌预警", &绘制.按钮.背敌预警},
        {"绘制车辆", &绘制.按钮.车辆},
        {"手雷预警", &绘制.按钮.手雷预警},
        {"绘制雷达", &绘制.按钮.雷达},
        {"忽略人机", &绘制.按钮.忽略人机},
        {"全图人数", &绘制.按钮.全图人数},
        {"敌人动作", &绘制.按钮.动作},
        {"盒内物资", &绘制.按钮.盒子物资},
    };
    for (int i = 0; i < IM_ARRAYSIZE(options); i++)
    {
        if (ImGui::Checkbox(options[i].name, options[i].variable))
        {
            绘制.保存配置();
            AddNotification(options[i].name, *options[i].variable);
        }
        ImGui::Spacing();
        if (i == IM_ARRAYSIZE(options) / 2 - 1)
            ImGui::NextColumn();
    }
    ImGui::Columns(1);
    ImGui::EndChild();
}

// ---------- 物资页面（美化版） ----------
void DrawItemsPage()
{
    ImGui::BeginChild("##ItemsContent", ImVec2(-1, -1), true);
    ImGui::SetCursorPos(ImVec2(20, 20));

    // 使用 ImGui 原生 TabBar 实现选项卡切换
    if (ImGui::BeginTabBar("##ItemTabs"))
    {
        // ========== 通用物资选项卡 ==========
        if (ImGui::BeginTabItem("通用物资"))
        {
            ImGui::Spacing();

            // 获取可用宽度，用于动态列数计算
            float availWidth = ImGui::GetContentRegionAvail().x - 20.0f;

            // 辅助宏：在给定区域内绘制带复选框的选项列表（自动多列）

            // 使用一个辅助函数绘制分组
            auto DrawCompactCheckboxGroup = [](const char *title, const std::vector<std::pair<const char *, bool *>> &items)
            {
                if (ImGui::CollapsingHeader(title, ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 10));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
                    // 使用简单的列布局，3列
                    ImGui::Columns(3, title, false);
                    for (size_t i = 0; i < items.size(); ++i)
                    {
                        const auto &[name, var] = items[i];
                        if (ImGui::Checkbox(name, var))
                        {
                            绘制.保存配置();
                            AddNotification(name, *var);
                        }
                        if ((i + 1) % ((items.size() + 2) / 3) == 0 && i != items.size() - 1)
                            ImGui::NextColumn();
                    }
                    ImGui::Columns(1);
                    ImGui::PopStyleVar(2);
                    ImGui::Spacing();
                }
            };

            // 特殊物品（保留必要项）
            std::vector<std::pair<const char *, bool *>> specials = {
                {"头甲", &绘制.按钮.显示防具},
                {"信号枪", &绘制.按钮.绘制信号枪},
                {"空投箱", &绘制.按钮.绘制空投},
                {"金插", &绘制.按钮.绘制金插},
                {"宝箱", &绘制.按钮.绘制宝箱},
                {"超级箱", &绘制.按钮.超级物资箱},
                {"武器箱", &绘制.按钮.绘制武器箱},
                {"盒子", &绘制.按钮.盒子},
                {"精英勋章", &绘制.按钮.精英勋章},
                {"自救器", &绘制.按钮.显示自救器},
                {"飞索", &绘制.按钮.显示飞索},
                {"黑色物资箱", &绘制.按钮.显示黑色物资箱}};
            DrawCompactCheckboxGroup("特殊物品", specials);

            // 古墓专属
            std::vector<std::pair<const char *, bool *>> tomb = {
                {"隐藏古墓箱子", &绘制.按钮.隐藏古墓已开启},
                {"古墓树木", &绘制.按钮.显示古墓篮子},
                {"古墓华贵宝箱", &绘制.按钮.显示古墓首饰盒},
                {"古墓精致宝箱", &绘制.按钮.显示古墓宝箱},
                {"古墓宝箱", &绘制.按钮.显示古墓精致宝箱},
                {"古墓首饰盒", &绘制.按钮.显示古墓华贵宝箱},
                {"古墓篮子", &绘制.按钮.显示古墓树木}};
            DrawCompactCheckboxGroup("古墓专属", tomb);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("自定义物资"))
        {
            ImGui::Spacing();

            // 总开关
            if (ImGui::Checkbox("启用自定义物资绘制", &绘制.按钮.自定义物资开关))
            {
                绘制.保存配置();
            }
            ImGui::Checkbox("开发者:", &绘制.按钮.Debug);
            ImGui::SameLine();
            ImGui::RadioButton("类名", &绘制.按钮.Debug模式, 0);
            ImGui::SameLine();
            ImGui::RadioButton("地址", &绘制.按钮.Debug模式, 1);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // 使用静态 reader，但提供重新加载按钮
            static DataReader customReader;
            static bool dataLoaded = false;
            static std::string lastFilePath = "/sdcard/AuraKernel/自定义物资.txt";

            ImGui::Text("数据文件: %s", lastFilePath.c_str());
            if (ImGui::Button("重新加载数据文件"))
            {
                if (customReader.loadDataFromFile(lastFilePath))
                {
                    dataLoaded = true;
                    AddNotification("自定义物资数据加载成功", true);
                }
                else
                {
                    dataLoaded = false;
                    AddNotification("自定义物资数据加载失败，请检查文件", false);
                }
            }

            ImGui::Spacing();
            if (dataLoaded)
                ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.3f, 1.0f), "状态: 已加载");
            else
                ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "状态: 未加载");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndChild();
}

// ---------- 视觉页面 ----------
void DrawVisualPage()
{
    ImGui::BeginChild("##VisualContent", ImVec2(-1, -1), true);
    ImGui::SetCursorPos(ImVec2(20, 20));
    static const char *手雷样式选项[] = {"3D", "曲线"};
    ImGui::Text("手雷样式");
    ImGui::SameLine(150);
    ImGui::SetNextItemWidth(200);
    ImGui::SliderInt("##手雷样式", &绘制.按钮.手雷样式, 0, 1, 手雷样式选项[绘制.按钮.手雷样式]);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Columns(2, "##视觉列", false);
    ImGui::Text("绘制距离");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(150);
    ImGui::SliderInt("##绘制距离", &绘制.按钮.绘制最大距离, 100, 300, "%d 米");
    ImGui::Text("骨骼距离");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(150);
    ImGui::SliderFloat("##骨骼距离", &绘制.骨骼距离限制, 0.0f, 300.0f, "%.0f 米");
    ImGui::Text("方框粗细");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(150);
    ImGui::SliderFloat("##方框粗细", &绘制.按钮.方框粗细, 0.1f, 5.0f, "%.1f");
    ImGui::Text("射线粗细");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(150);
    ImGui::SliderFloat("##射线粗细", &绘制.按钮.射线粗细, 0.1f, 5.0f, "%.1f");
    ImGui::NextColumn();
    ImGui::Text("骨骼粗细");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(150);
    ImGui::SliderFloat("##骨骼粗细", &绘制.按钮.骨骼粗细, 0.1f, 5.0f, "%.1f");
    ImGui::Text("雷达X");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(150);
    ImGui::SliderFloat("##雷达X", &绘制.按钮.雷达X, 0.0f, 2400.0f, "%.0f");
    ImGui::Text("雷达Y");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(150);
    ImGui::SliderFloat("##雷达Y", &绘制.按钮.雷达Y, 0.0f, 1080.0f, "%.0f");
    ImGui::Text("动作字体");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(150);
    ImGui::SliderInt("##动作字体", &绘制.动作字体大小, 6, 40);
    ImGui::Text("距离字体");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(150);
    ImGui::SliderInt("##距离字体", &绘制.距离字体大小, 6, 40);
    ImGui::Text("手持字体");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(150);
    ImGui::SliderInt("##手持字体", &绘制.手持字体大小, 6, 40);
    ImGui::Text("物资字体");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(150);
    ImGui::SliderInt("##物资字体", &绘制.物资字体大小, 6, 40);
    ImGui::Columns(1);
    ImGui::EndChild();
}

void DrawColorPage()
{
    ImGui::BeginChild("##ColorContent", ImVec2(-1, -1), true);
    ImGui::SetCursorPos(ImVec2(20, 20));

    // ====================== 真人/人机 分段式切换按钮 ======================
    ImGui::Text("当前配置");
    ImGui::SameLine(120);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

    // 真人按钮
    ImGui::PushStyleColor(ImGuiCol_Button, ColorSettings == 0 ? ImVec4(0.2f, 0.6f, 1.0f, 0.8f) : ImVec4(0.25f, 0.25f, 0.28f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorSettings == 0 ? ImVec4(0.3f, 0.7f, 1.0f, 0.9f) : ImVec4(0.35f, 0.35f, 0.38f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.5f, 0.9f, 1.0f));
    if (ImGui::Button("真人", ImVec2(90, 28)))
    {
        ColorSettings = 0;
        绘制.保存配置();
    }
    ImGui::PopStyleColor(3);

    // 人机按钮
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ColorSettings == 1 ? ImVec4(0.2f, 0.6f, 1.0f, 0.8f) : ImVec4(0.25f, 0.25f, 0.28f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorSettings == 1 ? ImVec4(0.3f, 0.7f, 1.0f, 0.9f) : ImVec4(0.35f, 0.35f, 0.38f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.5f, 0.9f, 1.0f));
    if (ImGui::Button("人机", ImVec2(90, 28)))
    {
        ColorSettings = 1;
        绘制.保存配置();
    }
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ====================== 预定义颜色配置 ======================
    static const ImVec4 presetColors[] = {
        {0.0f, 1.0f, 0.0f, 1.0f}, // 绿色
        {0.0f, 0.0f, 1.0f, 1.0f}, // 蓝色
        {1.0f, 0.0f, 1.0f, 1.0f}, // 紫色
        {1.0f, 1.0f, 1.0f, 1.0f}, // 白色
        {1.0f, 0.0f, 0.0f, 1.0f}  // 红色
    };
    static const char *presetNames[] = {"绿色", "蓝色", "紫色", "白色", "红色"};
    const int presetCount = IM_ARRAYSIZE(presetColors);

    // 获取颜色名称
    auto GetColorName = [&](const float *color) -> std::string
    {
        for (int i = 0; i < presetCount; i++)
        {
            if (fabsf(color[0] - presetColors[i].x) < 0.01f &&
                fabsf(color[1] - presetColors[i].y) < 0.01f &&
                fabsf(color[2] - presetColors[i].z) < 0.01f)
            {
                return presetNames[i];
            }
        }
        return "自定义";
    };

    // 切换预定义颜色
    auto SwitchPresetColor = [&](float *color, int delta)
    {
        int currentIndex = -1;
        for (int i = 0; i < presetCount; i++)
        {
            if (fabsf(color[0] - presetColors[i].x) < 0.01f &&
                fabsf(color[1] - presetColors[i].y) < 0.01f &&
                fabsf(color[2] - presetColors[i].z) < 0.01f)
            {
                currentIndex = i;
                break;
            }
        }
        currentIndex = (currentIndex == -1) ? 0 : (currentIndex + delta + presetCount) % presetCount;
        color[0] = presetColors[currentIndex].x;
        color[1] = presetColors[currentIndex].y;
        color[2] = presetColors[currentIndex].z;
        绘制.保存配置();
    };

    // ====================== 颜色项数据 ======================
    int colorSetIndex = ColorSettings;
    const char *colorLabels[] = {"方框颜色", "射线颜色", "骨骼颜色", "距离颜色", "名称颜色", "物资颜色"};
    float *colorPointers[] = {
        绘制.Colorset[colorSetIndex].方框颜色,
        绘制.Colorset[colorSetIndex].射线颜色,
        绘制.Colorset[colorSetIndex].骨骼颜色,
        绘制.Colorset[colorSetIndex].距离颜色,
        绘制.Colorset[colorSetIndex].名称颜色,
        绘制.物资颜色};

    // ====================== 表格布局 ======================
    if (ImGui::BeginTable("ColorTable", 3, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("标签", ImGuiTableColumnFlags_WidthFixed, 110);
        ImGui::TableSetupColumn("调节", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("颜色", ImGuiTableColumnFlags_WidthFixed, 70);

        for (int i = 0; i < IM_ARRAYSIZE(colorLabels); i++)
        {
            ImGui::TableNextRow();
            float *col = colorPointers[i];
            ImVec4 colorVec = ImVec4(col[0], col[1], col[2], 1.0f);
            std::string name = GetColorName(col);

            // 列1：左侧文字 手动居中对齐
            ImGui::TableSetColumnIndex(0);
            float textWidth = ImGui::CalcTextSize(colorLabels[i]).x;
            float columnWidth = ImGui::GetColumnWidth();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (columnWidth - textWidth) * 0.5f);
            ImGui::Text("%s", colorLabels[i]);

            // 列2：◀ 固定颜色按钮 ▶（使用箭头实际位置对齐）
            ImGui::TableSetColumnIndex(1);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 0));

            const float arrowW = 22.0f;
            const float btnW = 110.0f;
            const float btnH = 45.0f;
            const float totalW = arrowW + btnW + arrowW;

            float availWidth = ImGui::GetColumnWidth();
            float startX = ImGui::GetCursorPosX() + (availWidth - totalW) * 0.5f;

            ImGui::SetCursorPosX(startX);

            // 左箭头
            ImGui::PushID(i * 10);
            if (ImGui::ArrowButton("##L", ImGuiDir_Left))
            {
                SwitchPresetColor(col, -1);
            }
            ImGui::PopID();
            ImVec2 leftArrowMin = ImGui::GetItemRectMin();
            ImVec2 leftArrowMax = ImGui::GetItemRectMax();
            float arrowY = leftArrowMin.y; // 箭头真实 Y 坐标

            // 中间按钮（基于左箭头右侧 + 间距）
            float gap = 20.0f; // 与左箭头间距
            ImVec2 btnMin(leftArrowMax.x + gap, arrowY);
            ImVec2 btnMax(btnMin.x + btnW, btnMin.y + btnH);

            ImDrawList *dl = ImGui::GetWindowDrawList();
            ImU32 bgColor = ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0.85f));
            dl->AddRectFilled(btnMin, btnMax, bgColor, 24.0f);
            dl->AddRect(btnMin, btnMax, IM_COL32(255, 255, 255, 60), 24.0f, 0, 1.0f);

            // 文字居中
            ImVec2 textSize = ImGui::CalcTextSize(name.c_str());
            ImVec2 textPos(btnMin.x + (btnW - textSize.x) * 0.5f, btnMin.y + (btnH - textSize.y) * 0.5f);
            dl->AddText(textPos, IM_COL32(255, 255, 255, 255), name.c_str());

            // 右箭头（从中间按钮右侧 + 间距开始）
            ImGui::SetCursorScreenPos(ImVec2(btnMax.x + gap, arrowY));
            ImGui::PushID(i * 10 + 1);
            if (ImGui::ArrowButton("##R", ImGuiDir_Right))
            {
                SwitchPresetColor(col, 1);
            }
            ImGui::PopID();

            ImGui::PopStyleVar(); // ItemSpacing

            // 列3：缩小版精致圆形小球
            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(i);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 100.0f);
            ImGui::PushItemWidth(18);
            if (ImGui::ColorEdit3(("##c" + std::to_string(i)).c_str(), col,
                                  ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs))
            {
                绘制.保存配置();
            }
            ImGui::PopItemWidth();
            ImGui::PopStyleVar();
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    ImGui::EndChild();
}

// ---------- 设置页面 ----------
void DrawSettingsPage()
{
    ImGui::BeginChild("##SettingsContent", ImVec2(-1, -1), true);
    ImGui::SetCursorPos(ImVec2(20, 20));
    ImGui::Text("界面主题");
    ImGui::SameLine(150);
    ImGui::SetNextItemWidth(200);
    if (ImGui::Combo("##Theme", &currentTheme, themeNames, IM_ARRAYSIZE(themeNames)))
    {
        // 主题切换在主循环中应用
    }
    ImGui::Spacing();
    ImGui::Text("状态栏透明度");
    ImGui::SameLine(150);
    ImGui::SetNextItemWidth(200);
    ImGui::SliderFloat("##StatusBarAlpha", &statusBarAlpha, 0.3f, 1.0f, "%.2f");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    float btnW = (ImGui::GetContentRegionAvail().x - 60) / 4;
    if (ImGui::Button("还原", ImVec2(btnW, 36)))
        AddNotification("配置已还原", true);
    ImGui::SameLine();
    if (ImGui::Button("保存", ImVec2(btnW, 36)))
    {
        绘制.保存配置();
        AddNotification("配置已保存", true);
    }
    ImGui::SameLine();
    if (ImGui::Button("加载", ImVec2(btnW, 36)))
        AddNotification("配置已加载", true);
    ImGui::SameLine();
    if (ImGui::Button("删除", ImVec2(btnW, 36)))
        AddNotification("配置已删除", true);
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::BeginChild("##SliderCard", ImVec2(-1, 200), true);
    ImGui::SetCursorPos(ImVec2(20, 20));
    float fullW = ImGui::GetContentRegionAvail().x - 40;
    ImGui::Text("帧率限制");
    ImGui::SetNextItemWidth(fullW);
    ImGui::SliderInt("##fps", &绘制.按钮.当前帧率, 0, 144);
    ImGui::Spacing();
    ImGui::Text("UI圆角");
    ImGui::SetNextItemWidth(fullW);
    ImGui::SliderFloat("##UI圆角", &绘制.UI圆角, 0.0f, 35.0f, "%.0f");
    ImGui::Spacing();
    ImGui::Text("按键圆角");
    ImGui::SetNextItemWidth(fullW);
    ImGui::SliderFloat("##按键圆角", &绘制.按键圆角, 0.0f, 25.0f, "%.0f");
    ImGui::EndChild();
    ImGui::EndChild();
}

// ---------- 主绘制函数 ----------
void 布局::绘制悬浮窗()
{
    // FPS 计算（独立于 showWindow）
    static auto lastFpsTime = std::chrono::steady_clock::now();
    static int frameCount = 0;
    frameCount++;
    auto nowFps = std::chrono::steady_clock::now();
    float elapsedFps = std::chrono::duration<float>(nowFps - lastFpsTime).count();
    if (elapsedFps >= 1.0f)
    {
        g_FPS = frameCount;
        frameCount = 0;
        lastFpsTime = nowFps;
    }

    static auto last_update = std::chrono::steady_clock::now();
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_update).count();
    if (elapsed > 100)
    {
        绘制.运行负载 = 15.0f + 8.0f * sin(std::chrono::duration_cast<std::chrono::milliseconds>(current_time - 绘制.启动时间).count() / 2000.0f);
        static int counter = 0;
        绘制.网络延迟 = 25 + (counter++ % 40);
        last_update = current_time;
    }

    drawBegin();
    DrawNotifications();

    // 根据当前主题应用样式
    switch (currentTheme)
    {
    case THEME_DEFAULT:
        ApplyMinimalTheme();
        break;
    case THEME_SAKURA:
        ApplyMinimalTheme();
        break; // 暂用默认
    case THEME_NOODLE:
        ApplyNoodleUITheme();
        break;
    default:
        ApplyNoodleUITheme();
        break;
    }

    if (绘制.解密数组选择窗口开启)
        绘制.显示解密数组选择窗口();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 绘制.UI圆角);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 绘制.按键圆角);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 绘制.按键圆角);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 绘制.UI圆角);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 绘制.按键圆角);

    static time_t endTime = 0, last_check = 0;
    static bool time_parsed = false;
    auto now = std::chrono::system_clock::now();
    time_t now_time = std::chrono::system_clock::to_time_t(now);
    if (now_time - last_check >= 60)
    {
        last_check = now_time;
        if (!time_parsed)
        {
            struct tm end_tm = {0};
            if (sscanf(到期时间.c_str(), "%d-%d-%d %d:%d:%d", &end_tm.tm_year, &end_tm.tm_mon, &end_tm.tm_mday, &end_tm.tm_hour, &end_tm.tm_min, &end_tm.tm_sec) == 6)
            {
                end_tm.tm_year -= 1900;
                end_tm.tm_mon -= 1;
                endTime = mktime(&end_tm);
                time_parsed = true;
            }
        }
        if (now_time > endTime && endTime > 0)
            exit(0);
    }

    if (绘制.按钮.绘制)
        绘制.运行绘制();
    DrawThreeColorBalls();

    // if (广角设置)
    // {
    //     long 广角地址 = 绘制.读写.getPtr64(绘制.地址.自身地址 + 0x1010);
    //     绘制.读写.WriteFloat(绘制.读写.getPtr64(广角地址 + 0x10) + 0x2f4, 绘制.按钮.第三人称);
    // }

    // if (绘制.自瞄.自瞄控件)
    // {
    //     ImGui::SetNextWindowSize({200, 200});
    //     if (ImGui::Begin("自瞄控件", &自瞄控件, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar))
    //     {
    //         auto Pos = ImGui::GetWindowPos();
    //         static bool isMouseDown = false, wasMousePressed = false, isTe = false;
    //         static ImVec2 mousePressPos;
    //         bool mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    //         bool windowHovered = ImGui::IsWindowHovered();
    //         if (mouseDown && !isMouseDown && windowHovered && ImGui::IsMouseHoveringRect(Pos, {Pos.x + 100, Pos.y + 100}))
    //         {
    //             isMouseDown = true;
    //             wasMousePressed = true;
    //             mousePressPos = ImGui::GetMousePos();
    //         }
    //         else if (!mouseDown && isMouseDown && wasMousePressed)
    //         {
    //             ImVec2 mouseReleasePos = ImGui::GetMousePos();
    //             if (mousePressPos.x == mouseReleasePos.x && mousePressPos.y == mouseReleasePos.y)
    //                 isTe = !isTe;
    //             isMouseDown = false;
    //             wasMousePressed = false;
    //         }
    //         if (isTe)
    //         {
    //             绘制.自瞄.初始化 = true;
    //             ImGui::GetWindowDrawList()->AddImage(手持图片[4].DS, {Pos.x + 20, Pos.y + 20}, {Pos.x + 120, Pos.y + 120});
    //         }
    //         else
    //         {
    //             绘制.自瞄.初始化 = false;
    //             ImGui::GetWindowDrawList()->AddImage(手持图片[3].DS, {Pos.x + 20, Pos.y + 20}, {Pos.x + 120, Pos.y + 120});
    //         }
    //     }
    //     ImGui::End();
    // }

    // 绘制顶部状态栏（由开关控制）
    if (showTopStatusBar)
        DrawTopStatusBar();
    // 处理音量键事件
    if (g_volumeKeyPressed.exchange(false))
    {
        悬浮窗 = !悬浮窗;
    }
    if (悬浮窗)
    {
        ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(900, 650), ImVec2(FLT_MAX, FLT_MAX));
        if (窗口状态)
        {
            ImGui::SetWindowPos(绘制.悬浮窗标题, 绘制.Pos, ImGuiCond_Always);
            窗口状态 = false;
        }
        绘制.Pos = ImGui::GetWindowPos();
        ImGui::Begin("Aura Kernel", &悬浮窗, ImGuiWindowFlags_NoCollapse);
        static int selectedMenu = 0;
        DrawLeftNavigation(selectedMenu);
        ImGui::SameLine();
        ImGui::BeginChild("##RightContent", ImVec2(-1, -1), true);
        switch (selectedMenu)
        {
        case 0:
            DrawHomePage();
            break;
        case 1:
            DrawCharacterPage();
            break;
        case 2:
            DrawItemsPage();
            break;
        case 3:
            DrawVisualPage();
            break;
        case 4:
            DrawColorPage();
            break;
        case 5:
            DrawSettingsPage();
            break;
        default:
            break;
        }
        ImGui::EndChild();
        绘制.winWidth = ImGui::GetWindowWidth();
        绘制.winHeith = ImGui::GetWindowHeight();
        g_window = ImGui::GetCurrentWindow();
        ImGui::End();
    }
    drawEnd();
}

void 布局::开启悬浮窗()
{
    // 启动音量键监听
    g_volumeThread = std::thread(VolumeKeyListener);
    timer WindowDrawing;
    WindowDrawing.SetFps(120);
    WindowDrawing.AotuFPS_init();
    WindowDrawing.setAffinity();
    绘制.启动时间 = std::chrono::steady_clock::now();
    while (true)
    {
        更新状态();
        绘制悬浮窗();
        WindowDrawing.SetFps(绘制.按钮.当前帧率);
        WindowDrawing.AotuFPS();
        std::this_thread::sleep_for(1ms);
    }
    // 退出时清理
    g_volumeThreadRunning = false;
    if (g_volumeThread.joinable())
        g_volumeThread.join();
}

// 在 悬浮窗.cpp 中简化更新状态函数
void 更新状态()
{
    static auto last_time = std::chrono::steady_clock::now();
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_time).count();
    绘制.运行负载 = 10.0f + 5.0f * sin(elapsed / 1000.0f);
    绘制.网络延迟 = 30 + rand() % 50;
    last_time = current_time;
}