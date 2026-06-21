#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <map>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <memory>
#include <mutex>
#include <random>
#include "json.hpp"
#include "图片调用.h"
#include "物资ID.h"
#include "辅助类.h"
#include "DataReader.h"
#include "ItemNameMap.h"
#include "HookRW.h" // 添加HookRW头文件
#include "Offsets.h"
#include <unistd.h>
// 添加ptrace过检测功能
#include <sys/ptrace.h>
#include "json.hpp"

// ========== 自定义物资全局变量 ==========
DataReader *g_CustomReader = nullptr;
std::atomic<bool> g_CustomDataLoaded{false}; // 定义并初始化

using json = nlohmann::json;
extern float statusBarAlpha;
int decrypt_zero_x()
{
    return 0x10; // 默认偏移值，根据你的游戏修改
}

int decrypt_zero_y()
{
    return 0x20; // 默认偏移值，根据你的游戏修改
}

extern 绘制 绘制;
std::map<std::string, std::chrono::steady_clock::time_point> 自救Timers;
bool 线程开启状态 = false;
extern bool showTopStatusBar;
// 掩体函数

void 更新自救倒计时()
{
    auto it = 自救Timers.begin();
    while (it != 自救Timers.end())
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                           std::chrono::steady_clock::now() - it->second)
                           .count();
        if (elapsed >= 10)
            it = 自救Timers.erase(it);
        else
            ++it;
    }
}

int GetSmartAimBone(int enemyState)
{
    // 趴下和探头状态打头
    if (enemyState == 64 || enemyState == 4112 || enemyState == 4128)
    {
        return 0; // 头部部位
    }
    // 其他所有状态都打脖子
    return 1; // 脖子部位
}

// 修复函数声明语法
std::vector<HookRW::MemSeg> 获取内存段(pid_t pid)
{
    std::vector<HookRW::MemSeg> segments;
    char path[128];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);

    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        printf("错误: 无法打开 '%s'\n", path);
        return segments;
    }

    char *line = nullptr;
    size_t length = 0;
    while (getline(&line, &length, fp) > 0)
    {
        uintptr_t start, end;
        char flags[8] = {0};
        if (sscanf(line, "%lx-%lx %7s", &start, &end, flags) == 3)
        {
            if (strstr(flags, "rw-p"))
            {
                segments.push_back({start, end});
            }
        }
    }

    free(line);
    fclose(fp);
    return segments;
}

// 修复：正确的参数声明语法
int 验证内存(class 绘制 *绘制实例, uintptr_t addr)
{
    int count = 0;
    for (uintptr_t p = addr - 4;; p -= 8)
    {
        int value = 绘制实例->读写.getDword(p);
        if (value < 100 || value > 200)
            break;
        count++;
    }
    return count;
}

// 修复：正确的参数声明语法
std::vector<uintptr_t> 扫描内存段(const HookRW::MemSeg &seg, uintptr_t target, class 绘制 *绘制实例)
{
    std::vector<uintptr_t> results;

    // 按4KB页边界对齐
    uintptr_t start = (seg.start + 0xFFF) & ~0xFFF;
    uintptr_t end = seg.end & ~0xFFF;
    if (start >= end)
        return results;

    const size_t pageSize = 0x1000;
    const size_t ptrSize = 8;
    uint8_t *buffer = new uint8_t[pageSize];
    if (!buffer)
        return results;

    // 分页读取并扫描
    for (uintptr_t offset = 0; offset + pageSize <= end - start; offset += pageSize)
    {
        if (!绘制实例->读写.readv(start + offset, buffer, pageSize))
        {
            continue; // 读取失败则跳过
        }

        for (size_t i = 0; i + ptrSize <= pageSize; i += ptrSize)
        {
            uintptr_t value = *reinterpret_cast<uintptr_t *>(buffer + i);
            if (value == target)
            {
                results.push_back(start + offset + i);
            }
        }
    }

    delete[] buffer;
    return results;
}

void 绘制::查找解密地址()
{
    if (!this->已启用解密)
    {
        return;
    }

    try
    {
        // 步骤1: 获取目标进程PID
        printf("开始查找解密地址 (PID: %d)\n", this->pid);

        // 步骤2: 获取模块基址
        if (地址.libue4 == 0)
        {
            printf("错误: 未找到模块基址\n");
            return;
        }
        printf("模块基址: 0x%lX\n", 地址.libue4);

        // 步骤3: 计算目标特征地址
        // 使用链式读取
        uintptr_t 临时地址1 = 读写.getPtr64(地址.libue4 + 0x14EC2468);
        if (临时地址1 == 0)
        {
            printf("错误: 第一步读取失败\n");
            return;
        }

        uintptr_t 临时地址2 = 读写.getPtr64(临时地址1 + 0xF8);
        if (临时地址2 == 0)
        {
            printf("错误: 第二步读取失败\n");
            return;
        }
        uintptr_t 临时地址3 = 读写.getPtr64(临时地址2 + 0x340);
        if (临时地址3 == 0)
        {
            printf("错误: 第三步读取失败\n");
            return;
        }

        this->特征地址 = 读写.getPtr64(临时地址3 + 0xF0);
        if (this->特征地址 == 0)
        {
            printf("错误: 无法计算特征地址\n");
            return;
        }
        printf("特征地址: 0x%lX\n", this->特征地址);

        // 步骤4: 获取进程可读写内存段
        auto segments = 获取内存段(this->pid); // 调用静态函数
        if (segments.empty())
        {
            printf("错误: 未找到有效的内存段\n");
            return;
        }
        printf("扫描 %zu 个可读写内存段...\n", segments.size());

        // 步骤5: 扫描所有内存段
        std::vector<uintptr_t> 指针地址列表;
        for (const auto &seg : segments)
        {
            auto found = 扫描内存段(seg, this->特征地址, this); // 调用静态函数
            指针地址列表.insert(指针地址列表.end(), found.begin(), found.end());
        }
        printf("共找到 %zu 个指向特征地址的指针\n", 指针地址列表.size());

        // 步骤6: 验证并提取解密地址
        this->解密地址列表.clear();
        for (uintptr_t ptrAddr : 指针地址列表)
        {
            int validCount = 验证内存(this, ptrAddr); // 调用静态函数
            if (validCount > 5)
            {
                uintptr_t decryptAddr = ptrAddr - 8 * validCount;
                this->解密地址列表.push_back(decryptAddr);
                printf("发现解密地址: 0x%lX (验证计数: %d)\n", decryptAddr, validCount);
            }
        }
        if (!this->解密地址列表.empty())
        {
            // 使用第一个找到的解密地址
            this->解密数组 = this->解密地址列表[0];
            地址.数组地址 = this->解密数组;
            世界数量 = 读写.getDword(地址.世界地址 + 0xB8);
            printf("成功启用解密，使用地址: 0x%lX\n", this->解密数组);
        }
        else
        {
            printf("未找到有效的解密地址\n");
        }

        printf("总计找到 %zu 个有效解密地址\n", this->解密地址列表.size());
    }
    catch (...)
    {
        printf("解密地址查找过程中发生异常\n");
    }
}

void 绘制::设置解密功能(bool 启用)
{
    this->已启用解密 = 启用;
    if (启用)
    {
        // 在新线程中查找解密地址
        std::thread 解密线程([&]()
                             { this->查找解密地址(); });
        解密线程.detach();
    }
    else
    {
        this->解密数组 = 0;
        this->解密地址列表.clear();
        printf("解密功能已禁用\n");
    }
}

bool 绘制::获取解密状态()
{
    return this->已启用解密 && this->解密数组 != 0;
}

void 绘制::重新扫描解密地址()
{
    if (this->已启用解密)
    {
        printf("重新扫描解密地址...\n");
        this->查找解密地址();
    }
}

void 绘制::显示解密数组选择窗口()
{
    if (!this->解密数组选择窗口开启)
    {
        return;
    }

    // 设置窗口大小和位置
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("解密数组选择", &this->解密数组选择窗口开启, ImGuiWindowFlags_NoCollapse))
    {

        ImGui::TextColored(ImVec4(0, 1, 1, 1), "当前解密状态: %s",
                           this->获取解密状态() ? "已启用" : "已禁用");

        ImGui::TextColored(ImVec4(0, 1, 1, 1), "当前使用地址: 0x%lX", this->解密数组);

        ImGui::Separator();

        // 显示找到的所有解密地址
        if (this->解密地址列表.empty())
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "未找到任何解密地址");
            //        if (ImGui::Button("重新扫描")) {
            //         this->重新扫描解密地址();
            //        }
        }
        else
        {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "找到 %zu 个解密地址:", this->解密地址列表.size());

            // 创建滚动区域
            ImGui::BeginChild("##解密地址列表", ImVec2(0, 250), true);

            for (size_t i = 0; i < this->解密地址列表.size(); i++)
            { // 修复：解密地址_list -> 解密地址列表
                uintptr_t addr = this->解密地址列表[i];

                // 创建按钮标签
                char label[64];
                snprintf(label, sizeof(label), "地址 %zu: 0x%lX", i + 1, addr);

                // 如果是当前使用的地址，高亮显示
                if (addr == this->解密数组)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 0.3f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));

                    if (ImGui::Button(label, ImVec2(ImGui::GetContentRegionAvail().x - 120, 0)))
                    {
                        this->选择解密数组(addr);
                    }

                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "当前使用");

                    ImGui::PopStyleColor(2);
                }
                else
                {
                    if (ImGui::Button(label, ImVec2(ImGui::GetContentRegionAvail().x - 120, 0)))
                    {
                        this->选择解密数组(addr);
                    }

                    ImGui::SameLine();
                    if (ImGui::SmallButton("选择"))
                    {
                        this->选择解密数组(addr);
                    }
                }
            }

            ImGui::EndChild();
        }

        ImGui::Separator();

        // 控制按钮
        ImGui::BeginGroup();

        float button_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 3.0f;

        if (ImGui::Button("重新扫描", ImVec2(button_width, 60)))
        {
            this->重新扫描解密地址();
        }

        ImGui::SameLine();

        if (this->获取解密状态())
        {
            if (ImGui::Button("禁用解密", ImVec2(button_width, 60)))
            {
                this->设置解密功能(false);
            }
        }
        else
        {
            if (ImGui::Button("启用解密", ImVec2(button_width, 60)))
            {
                this->设置解密功能(true);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("关闭窗口", ImVec2(button_width, 60)))
        {
            this->解密数组选择窗口开启 = false;
        }

        ImGui::EndGroup();

        // 状态信息
        ImGui::Separator();
        if (this->特征地址 != 0)
        {
            ImGui::Text("特征地址: 0x%lX", this->特征地址);
        }

        ImGui::End();
    }
}
void 绘制::选择解密数组(uintptr_t 数组地址)
{
    this->解密数组 = 数组地址;

    // 更新绘制系统中的数组地址
    if (地址.数组地址 != 0)
    {
        地址.数组地址 = this->解密数组;
        世界数量 = 读写.getDword(地址.世界地址 + 0xB8);
        printf("已选择解密地址: 0x%lX\n", this->解密数组);
    }
}

float calculateDistance(float x1, float y1, float x2, float y2)
{
    return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
// std::unordered_map<int, 武器触发条件> 绘制::武器触发配置;
// std::unordered_map<int, 武器参数> 绘制::武器参数配置;
std::unordered_map<uintptr_t, int> 绘制::lastBoneIndices;
std::unordered_map<uintptr_t, std::vector<int>> 绘制::recentBoneIndices;

bool clickRegion(bool isClick, Rect clickRect);

char filename[64];
const char *MEMReadandwrite = "/storage/emulated/0/Download/Memory";
int MountFile;

std::vector<smokeObject> smokeObjects;
std::vector<smokeObject> aimSmokeObjects;

std::mutex mtx;
unordered_map<int, ImColor> colorMap;
std::mutex printMutex;
std::map<std::string, int> Antitankgrenade;     // 给手雷一个机会,不然让你飞起来
std::map<std::string, int> SelfAntitankgrenade; // 给手雷一个机会,不然让你飞起来
// 定义一个全局计时器字典，用于存储每个手雷的开始时间
std::map<std::string, std::chrono::steady_clock::time_point> grenadeTimers;
static bool 烟雾中 = false;

double normalizeAngle180(double angle) // 角度转换
{
    // 将角度转换到 [0, 360) 范围内
    angle = fmod(angle, 360.0f);
    // 如果角度小于 0，则加上 360 使其变为正数
    if (angle < 0.0f)
    {
        angle += 360.0f;
    }
    // 将角度转换到 [-180, 180) 范围内
    if (angle > 180.0f)
    {
        angle -= 360.0f;
    }
    return angle;
}

void 绘制::保存配置()
{
    // 保存基础配置
    nlohmann::json base_config;

    // 按钮配置
    base_config["按钮"] = {
        {"雷达X", 按钮.雷达X},
        {"雷达Y", 按钮.雷达Y},
        {"方框粗细", 按钮.方框粗细},
        {"射线粗细", 按钮.射线粗细},
        {"骨骼粗细", 按钮.骨骼粗细},
        {"当前帧率", 按钮.当前帧率},
        {"帧率选项", 按钮.帧率选项},
        {"血条绘图", 按钮.血条绘图},
        {"动作", 按钮.动作},
        {"烟雾倒计时", 按钮.烟雾倒计时},
        {"绘制", 按钮.绘制},
        {"忽略人机", 按钮.忽略人机},
        {"手持2", 按钮.手持2},
        {"被瞄预警", 按钮.被瞄预警},
        {"物资总开关", 按钮.物资总开关},
        {"绘制信号枪", 按钮.绘制信号枪},
        {"绘制金插", 按钮.绘制金插},
        {"绘制宝箱", 按钮.绘制宝箱},
        {"绘制药箱", 按钮.绘制药箱},
        {"绘制武器箱", 按钮.绘制武器箱},
        {"头甲包显示", 按钮.头甲包显示},
        {"头甲包显示2", 按钮.头甲包显示2},
        {"盒子物资", 按钮.盒子物资},
        {"隐藏已开启", 按钮.隐藏已开启},
        {"超级物资箱", 按钮.超级物资箱},
        {"密钥", 按钮.显示密钥},
        {"绘制空投", 按钮.绘制空投},
        {"人数", 按钮.人数},
        {"方框", 按钮.方框},
        {"血量", 按钮.血量},
        {"手持", 按钮.手持},
        {"盒子", 按钮.盒子},
        {"背敌预警", 按钮.背敌预警},
        {"距离", 按钮.距离},
        {"射线", 按钮.射线},
        {"名字", 按钮.名字},
        {"骨骼", 按钮.骨骼},
        {"车辆", 按钮.车辆},
        {"雷达", 按钮.雷达},
        {"手雷预警", 按钮.手雷预警},
        {"其他物资", 按钮.其他物资},
        {"冲锋枪械", 按钮.冲锋枪械},
        {"狙击枪械", 按钮.狙击枪械},
        {"散弹枪械", 按钮.散弹枪械},
        {"显示对局信息", 按钮.显示对局信息},
        {"瞬爆雷预测", 按钮.瞬爆雷预测},
        {"自救倒计时", 按钮.自救倒计时},
        {"载具血量", 按钮.载具血量},
        {"载具油量", 按钮.载具油量},
        {"步枪", 按钮.显示步枪},
        {"子弹", 按钮.显示子弹},
        {"药品", 按钮.显示药品},
        {"防具", 按钮.显示防具},
        {"倍镜", 按钮.显示倍镜},
        {"扩容", 按钮.显示扩容},
        {"配件", 按钮.显示配件},
        {"饮料", 按钮.显示可乐},
        {"止痛药", 按钮.显示止痛药},
        {"投掷物", 按钮.投掷物品},
        {"肾上腺素", 按钮.显示肾上腺素},
        {"爆炸猎弓", 按钮.爆炸猎弓},
        {"超体职业", 按钮.超体职业},
        {"自救器", 按钮.显示自救器},
        {"飞索", 按钮.显示飞索},
        {"密室钥匙", 按钮.密室钥匙},
        {"黑色物资箱", 按钮.显示黑色物资箱},
        {"绘制最大距离", 按钮.绘制最大距离},
    };

    // 其他配置
    base_config["其他"] = {
        {"骨骼距离限制", 骨骼距离限制},
        {"mk20", mk20},
        {"m417", m417},
        {"动作字体大小", 动作字体大小},
        {"距离字体大小", 距离字体大小},
        {"手持字体大小", 手持字体大小},
        {"物资字体大小", 物资字体大小},
        {"名称字体大小", 名称字体大小},
        {"showTopStatusBar", showTopStatusBar},
    };
    // 在基础配置中添加圆角配置
    base_config["其他"]["UI圆角"] = UI圆角;
    base_config["其他"]["按键圆角"] = 按键圆角;

    // 颜色配置
    base_config["颜色"] = nlohmann::json::object();
    base_config["颜色"]["Colorset"] = nlohmann::json::array();
    for (int i = 0; i < 2; i++)
    {
        base_config["颜色"]["Colorset"].push_back({
            {"方框颜色", {Colorset[i].方框颜色[0], Colorset[i].方框颜色[1], Colorset[i].方框颜色[2], Colorset[i].方框颜色[3]}},
            {"方框掩体颜色", {Colorset[i].方框掩体颜色[0], Colorset[i].方框掩体颜色[1], Colorset[i].方框掩体颜色[2], Colorset[i].方框掩体颜色[3]}},
            {"射线掩体颜色", {Colorset[i].射线掩体颜色[0], Colorset[i].射线掩体颜色[1], Colorset[i].射线掩体颜色[2], Colorset[i].射线掩体颜色[3]}},
            {"射线颜色", {Colorset[i].射线颜色[0], Colorset[i].射线颜色[1], Colorset[i].射线颜色[2], Colorset[i].射线颜色[3]}},
            {"骨骼掩体颜色", {Colorset[i].骨骼掩体颜色[0], Colorset[i].骨骼掩体颜色[1], Colorset[i].骨骼掩体颜色[2], Colorset[i].骨骼掩体颜色[3]}},
            {"骨骼颜色", {Colorset[i].骨骼颜色[0], Colorset[i].骨骼颜色[1], Colorset[i].骨骼颜色[2], Colorset[i].骨骼颜色[3]}},
            {"阵营颜色", {Colorset[i].阵营颜色[0], Colorset[i].阵营颜色[1], Colorset[i].阵营颜色[2], Colorset[i].阵营颜色[3]}},
            {"距离颜色", {Colorset[i].距离颜色[0], Colorset[i].距离颜色[1], Colorset[i].距离颜色[2], Colorset[i].距离颜色[3]}},
            {"名称颜色", {Colorset[i].名称颜色[0], Colorset[i].名称颜色[1], Colorset[i].名称颜色[2], Colorset[i].名称颜色[3]}},
        });
    }
    base_config["颜色"]["物资颜色"] = {
        物资颜色[0],
        物资颜色[1],
        物资颜色[2],
        物资颜色[3]};
    base_config["颜色"]["车辆颜色"] = {
        车辆颜色[0],
        车辆颜色[1],
        车辆颜色[2],
        车辆颜色[3]};

    // 保存基础配置到文件
    std::ofstream base_file("/sdcard/AuraKernel/Aura选择配置.json");
    base_file << base_config.dump(4);
    base_file.close();
}

void 绘制::重置配置()
{
    remove("/sdcard/AuraKernel/Aura配置.json");
    remove("/sdcard/AuraKernel/Aura选择配置.json");
}

void 绘制::读取配置()
{
    // ===== 首次运行标记检测 =====
    const char *markFile = "/sdcard/AuraKernel/.initialized";
    const char *configFile = "/sdcard/AuraKernel/Aura选择配置.json";
    if (access(markFile, F_OK) != 0)

        if (access(markFile, F_OK) != 0)
        {
            remove(configFile);
            FILE *f = fopen(markFile, "w");
            if (f)
            {
                fputs("1", f);
                fclose(f);
            }
        }

    // 读取基础配置文件
    std::ifstream base_file("/sdcard/AuraKernel/Aura选择配置.json");
    if (base_file.is_open())
    {
        nlohmann::json base_config = nlohmann::json::parse(base_file);

        // 读取按钮配置
        if (base_config.contains("按钮"))
        {
            const auto &button = base_config["按钮"];
            按钮.雷达X = button.value("雷达X", 按钮.雷达X);
            按钮.雷达Y = button.value("雷达Y", 按钮.雷达Y);
            按钮.方框粗细 = button.value("方框粗细", 按钮.方框粗细);
            按钮.射线粗细 = button.value("射线粗细", 按钮.射线粗细);
            按钮.骨骼粗细 = button.value("骨骼粗细", 按钮.骨骼粗细);
            按钮.当前帧率 = button.value("当前帧率", 按钮.当前帧率);
            按钮.帧率选项 = button.value("帧率选项", 按钮.帧率选项);
            按钮.血条绘图 = button.value("血条绘图", 按钮.血条绘图);
            按钮.动作 = button.value("动作", 按钮.动作);
            按钮.烟雾倒计时 = button.value("烟雾倒计时", 按钮.烟雾倒计时);
            按钮.绘制 = button.value("绘制", 按钮.绘制);
            按钮.忽略人机 = button.value("忽略人机", 按钮.忽略人机);
            按钮.手持2 = button.value("手持2", 按钮.手持2);
            按钮.被瞄预警 = button.value("被瞄预警", 按钮.被瞄预警);
            按钮.物资总开关 = button.value("物资总开关", 按钮.物资总开关);
            按钮.绘制信号枪 = button.value("绘制信号枪", 按钮.绘制信号枪);
            按钮.绘制金插 = button.value("绘制金插", 按钮.绘制金插);
            按钮.绘制宝箱 = button.value("绘制宝箱", 按钮.绘制宝箱);
            按钮.绘制药箱 = button.value("绘制药箱", 按钮.绘制药箱);
            按钮.绘制武器箱 = button.value("绘制武器箱", 按钮.绘制武器箱);
            按钮.头甲包显示 = button.value("头甲包显示", 按钮.头甲包显示);
            按钮.头甲包显示2 = button.value("头甲包显示2", 按钮.头甲包显示2);
            按钮.盒子物资 = button.value("盒子物资", 按钮.盒子物资);
            按钮.隐藏已开启 = button.value("隐藏已开启", 按钮.隐藏已开启);
            按钮.超级物资箱 = button.value("超级物资箱", 按钮.超级物资箱);
            按钮.显示密钥 = button.value("密钥", 按钮.显示密钥);
            按钮.绘制空投 = button.value("绘制空投", 按钮.绘制空投);
            按钮.人数 = button.value("人数", 按钮.人数);
            按钮.方框 = button.value("方框", 按钮.方框);
            按钮.血量 = button.value("血量", 按钮.血量);
            按钮.手持 = button.value("手持", 按钮.手持);
            按钮.盒子 = button.value("盒子", 按钮.盒子);
            按钮.背敌预警 = button.value("背敌预警", 按钮.背敌预警);
            按钮.距离 = button.value("距离", 按钮.距离);
            按钮.射线 = button.value("射线", 按钮.射线);
            按钮.名字 = button.value("名字", 按钮.名字);
            按钮.骨骼 = button.value("骨骼", 按钮.骨骼);
            按钮.车辆 = button.value("车辆", 按钮.车辆);
            按钮.雷达 = button.value("雷达", 按钮.雷达);
            按钮.手雷预警 = button.value("手雷预警", 按钮.手雷预警);
            按钮.其他物资 = button.value("其他物资", 按钮.其他物资);
            按钮.冲锋枪械 = button.value("冲锋枪械", 按钮.冲锋枪械);
            按钮.狙击枪械 = button.value("狙击枪械", 按钮.狙击枪械);
            按钮.散弹枪械 = button.value("散弹枪械", 按钮.散弹枪械);
            按钮.显示对局信息 = button.value("显示对局信息", 按钮.显示对局信息);
            按钮.瞬爆雷预测 = button.value("瞬爆雷预测", 按钮.瞬爆雷预测);
            按钮.自救倒计时 = button.value("自救倒计时", 按钮.自救倒计时);
            按钮.载具血量 = button.value("载具血量", 按钮.载具血量);
            按钮.载具油量 = button.value("载具油量", 按钮.载具油量);
            按钮.显示步枪 = button.value("步枪", 按钮.显示步枪);
            按钮.显示子弹 = button.value("子弹", 按钮.显示子弹);
            按钮.显示药品 = button.value("药品", 按钮.显示药品);
            按钮.显示防具 = button.value("防具", 按钮.显示防具);
            按钮.显示倍镜 = button.value("倍镜", 按钮.显示倍镜);
            按钮.显示扩容 = button.value("扩容", 按钮.显示扩容);
            按钮.显示配件 = button.value("配件", 按钮.显示配件);
            按钮.显示可乐 = button.value("饮料", 按钮.显示可乐);
            按钮.显示止痛药 = button.value("止痛药", 按钮.显示止痛药);
            按钮.投掷物品 = button.value("投掷物", 按钮.投掷物品);
            按钮.显示肾上腺素 = button.value("肾上腺素", 按钮.显示肾上腺素);
            按钮.爆炸猎弓 = button.value("爆炸猎弓", 按钮.爆炸猎弓);
            按钮.超体职业 = button.value("超体职业", 按钮.超体职业);
            按钮.显示自救器 = button.value("自救器", false);
            按钮.显示飞索 = button.value("飞索", false);
            按钮.密室钥匙 = button.value("密室钥匙", false);
            按钮.显示黑色物资箱 = button.value("黑色物资箱", false);
            按钮.绘制最大距离 = button.value("绘制最大距离", 按钮.绘制最大距离);
        }

        // 读取其他配置
        if (base_config.contains("其他"))
        {
            const auto &other = base_config["其他"];
            骨骼距离限制 = other.value("骨骼距离限制", 骨骼距离限制);
            mk20 = other.value("mk20", mk20);
            m417 = other.value("m417", m417);
            UI圆角 = other.value("UI圆角", UI圆角);
            按键圆角 = other.value("按键圆角", 按键圆角);
            动作字体大小 = other.value("动作字体大小", 动作字体大小);
            距离字体大小 = other.value("距离字体大小", 距离字体大小);
            手持字体大小 = other.value("手持字体大小", 手持字体大小);
            物资字体大小 = other.value("物资字体大小", 物资字体大小);
            名称字体大小 = other.value("名称字体大小", 名称字体大小);
            showTopStatusBar = other.value("showTopStatusBar", true);
        }

        // 读取颜色配置
        if (base_config.contains("颜色"))
        {
            const auto &color = base_config["颜色"];

            if (color.contains("Colorset"))
            {
                const auto &colorset = color["Colorset"];
                for (size_t i = 0; i < std::min(size_t(2), colorset.size()); i++)
                {
                    if (colorset[i].contains("方框颜色"))
                    {
                        const auto &arr = colorset[i]["方框颜色"];
                        for (size_t j = 0; j < 4; j++)
                        {
                            Colorset[i].方框颜色[j] = arr[j].get<float>();
                        }
                    }
                    if (colorset[i].contains("方框掩体颜色"))
                    {
                        const auto &arr = colorset[i]["方框掩体颜色"];
                        for (size_t j = 0; j < 4; j++)
                        {
                            Colorset[i].方框掩体颜色[j] = arr[j].get<float>();
                        }
                    }
                    if (colorset[i].contains("射线颜色"))
                    {
                        const auto &arr = colorset[i]["射线颜色"];
                        for (size_t j = 0; j < 4; j++)
                        {
                            Colorset[i].射线颜色[j] = arr[j].get<float>();
                        }
                    }
                    if (colorset[i].contains("射线掩体颜色"))
                    {
                        const auto &arr = colorset[i]["射线掩体颜色"];
                        for (size_t j = 0; j < 4; j++)
                        {
                            Colorset[i].射线掩体颜色[j] = arr[j].get<float>();
                        }
                    }
                    if (colorset[i].contains("骨骼颜色"))
                    {
                        const auto &arr = colorset[i]["骨骼颜色"];
                        for (size_t j = 0; j < 4; j++)
                        {
                            Colorset[i].骨骼颜色[j] = arr[j].get<float>();
                        }
                    }
                    if (colorset[i].contains("骨骼掩体颜色"))
                    {
                        const auto &arr = colorset[i]["骨骼掩体颜色"];
                        for (size_t j = 0; j < 4; j++)
                        {
                            Colorset[i].骨骼掩体颜色[j] = arr[j].get<float>();
                        }
                    }

                    if (colorset[i].contains("阵营颜色"))
                    {
                        const auto &arr = colorset[i]["阵营颜色"];
                        for (size_t j = 0; j < 4; j++)
                        {
                            Colorset[i].阵营颜色[j] = arr[j].get<float>();
                        }
                    }
                    if (colorset[i].contains("距离颜色"))
                    {
                        const auto &arr = colorset[i]["距离颜色"];
                        for (size_t j = 0; j < 4; j++)
                        {
                            Colorset[i].距离颜色[j] = arr[j].get<float>();
                        }
                    }
                    if (colorset[i].contains("名称颜色"))
                    {
                        const auto &arr = colorset[i]["名称颜色"];
                        for (size_t j = 0; j < 4; j++)
                        {
                            Colorset[i].名称颜色[j] = arr[j].get<float>();
                        }
                    }
                }
            }
            if (color.contains("物资颜色"))
            {
                const auto &arr = color["物资颜色"];
                for (size_t i = 0; i < 4; i++)
                {
                    物资颜色[i] = arr[i].get<float>();
                }
            }
            if (color.contains("车辆颜色"))
            {
                const auto &arr = color["车辆颜色"];
                for (size_t i = 0; i < 4; i++)
                {
                    车辆颜色[i] = arr[i].get<float>();
                }
            }
        }
    }
}

uintptr_t 绘制::转十六进制(string value)
{
    try
    {
        return std::stoul(value, nullptr, 16);
    }
    catch (...)
    {
        return 0;
    }
}

ImColor RandomColor()
{
    float hue = static_cast<float>(rand() % 360);                       // 色相 0-360度
    float saturation = 0.7f + static_cast<float>(rand() % 30) / 100.0f; // 饱和度 0.7-1.0
    float value = 0.8f + static_cast<float>(rand() % 20) / 100.0f;      // 明度 0.8-1.0

    float r, g, b;
    ImGui::ColorConvertHSVtoRGB(hue / 360.0f, saturation, value, r, g, b);
    return ImColor(r, g, b, 1.0f); // 保持完全不透明
}

ImColor GetRandomColorById(int id)
{
    if (colorMap.find(id) != colorMap.end())
    {
        return colorMap[id];
    }
    else
    {
        ImColor color = RandomColor();
        colorMap[id] = color;
        return color;
    }
}

void 绘制::OffScreen(ImDrawList *ImDraw, D4DVector Obj, float camear, ImU32 color, float Radius, float 距离)
{
    ImRect screen_rect = {0.0f, 0.0f, 真实PX, 真实PY};
    if (Obj.Z > 0 && screen_rect.Contains({Obj.X, Obj.Y}))
        return;
    auto screen_center = screen_rect.GetCenter();
    auto angle = atan2(screen_center.y - Obj.Y, screen_center.x - Obj.X);
    angle += camear > 0 ? M_PI : 0.0f;
    D2DVector arrow_center{
        screen_center.x + Radius * cosf(angle),
        screen_center.y + Radius * sinf(angle)};
    std::array<ImVec2, 4> points{
        ImVec2(-22.0f, -8.6f),
        ImVec2(0.0f, 0.0f),
        ImVec2(-22.0f, 8.6f),
        ImVec2(-18.0f, 0.0f)};
    D2DVector tpoint;
    for (auto &point : points)
    {
        auto x = point.x * 1.155f;
        auto y = point.y * 1.155f;
        point.x = arrow_center.X + x * cosf(angle) - y * sinf(angle);
        point.y = arrow_center.Y + x * sinf(angle) + y * cosf(angle);
        tpoint.X = point.x;
        tpoint.Y = point.y;
    }
    float alpha = 1.0f;
    if (camear > 0)
    {
        constexpr float nearThreshold = 200 * 200;
        ImVec2 screen_outer_diff = {
            Obj.X < 0 ? abs(Obj.X) : (Obj.X > screen_rect.Max.x ? Obj.X - screen_rect.Max.x : 0.0f),
            Obj.Y < 0 ? abs(Obj.Y) : (Obj.Y > screen_rect.Max.y ? Obj.Y - screen_rect.Max.y : 0.0f),
        };
        float distance = static_cast<float>(pow(screen_outer_diff.x, 2) + pow(screen_outer_diff.y, 2));
        alpha = camear < 0 ? 1.0f : (distance / nearThreshold);
    }
    ImColor arrowColor = color;
    arrowColor.Value.w = (alpha < 1.0f) ? alpha : 1.0f;
    ImDraw->AddTriangleFilled(points[0], points[1], points[3], arrowColor);
    ImDraw->AddTriangleFilled(points[2], points[1], points[3], arrowColor);
    ImDraw->AddQuad(points[0], points[1], points[2], points[3], ImColor(0.0f, 0.0f, 0.0f, alpha), 1.335f);

    float radius = 30.0f;
    ImColor textColor = (arrowColor.Value.x == 0.0f) ? ImColor(0.0f, 1.0f, 0.0f, alpha) : ImColor(1.0f, 0.0f, 0.0f, alpha);
    string tmp = to_string((int)距离);
    auto textSize = ImGui::CalcTextSize(tmp.c_str(), 0, 28);
    ImDraw->AddText(NULL, 28, {tpoint.X - (textSize.x / 2), tpoint.Y}, ImColor(1.0f, 1.0f, 1.0f, alpha), tmp.c_str());
}

void DrawLine2D(FVector2 a1, FVector2 a2)
{
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(a1.X, a1.Y), ImVec2(a2.X, a2.Y), ImColor(255, 255, 255, 255), 1.0f);
}

bool Read(pid_t pid, uint64_t addr, void *buffer, size_t size)
{
    return 绘制.读写.readv(addr, buffer, size);
}

#include "Event.h"
void 绘制::初始化绘制(string 包名, int 真实X, int 真实Y)
{
    真实PX = 真实X;
    int pid1 = 读写.getPID(包名.c_str());
    this->pid = pid1;
    读写.初始化读写(pid1);
    绘图.初始化绘图(真实X, 真实Y);
    this->真实PX = 真实X;
    this->真实PY = 真实Y;
    骨骼 = new class 骨骼(&读写);
    if (真实Y < 真实X)
    {
        this->PX = 真实X / 2;
        this->PY = 真实Y / 2;
    }
    else
    {
        this->PX = 真实Y / 2;
        this->PY = 真实X / 2;
    }
    地址.libue4 = 读写.get_module_base((char *)"libUE4.so");

    // 设置启动时间
    启动时间 = std::chrono::steady_clock::now();

    // 初始化解密相关变量
    解密数组 = 0;
    已启用解密 = false;
    特征地址 = 0;
    解密地址列表.clear();
    解密数组选择窗口开启 = false;

    // 初始化其他状态变量
    运行负载 = 0.0f;
    网络延迟 = 0;

    DebugAimedClassName.clear();
    bDebugAimedValid = false;
}

FVector2D 绘制::WorldToScreen(const FVector_class &WorldLocation)
{
    FVector2D ScreenLocation;

    // 直接使用静态链读出的自身数据.矩阵
    float matrix[16];
    memcpy(matrix, 自身数据.矩阵, sizeof(matrix));

    // 计算裁剪空间中的 w 分量
    float w = matrix[3] * WorldLocation.X + matrix[7] * WorldLocation.Y + matrix[11] * WorldLocation.Z + matrix[15];
    if (w < 0.001f)
    {
        // 在相机后方，返回 INFINITY 让上层跳过绘制
        return FVector2D(INFINITY, INFINITY);
    }

    float screenW = displayInfo.width;
    float screenH = displayInfo.height;
    float halfW = screenW / 2.0f;
    float halfH = screenH / 2.0f;

    // NDC -> 屏幕坐标
    ScreenLocation.X = halfW + (matrix[0] * WorldLocation.X + matrix[4] * WorldLocation.Y + matrix[8] * WorldLocation.Z + matrix[12]) / w * halfW;
    ScreenLocation.Y = halfH - (matrix[1] * WorldLocation.X + matrix[5] * WorldLocation.Y + matrix[9] * WorldLocation.Z + matrix[13]) / w * halfH;

    return ScreenLocation;
}

D2DVector 绘制::WorldToScreen2(const FVector_class &WorldLocation)
{
    D2DVector ScreenLocation;
    float matrix[16];
    memcpy(matrix, 自身数据.矩阵, sizeof(matrix));

    float w = matrix[3] * WorldLocation.X + matrix[7] * WorldLocation.Y + matrix[11] * WorldLocation.Z + matrix[15];
    if (w < 0.001f)
    {
        ScreenLocation.X = INFINITY;
        ScreenLocation.Y = INFINITY;
        return ScreenLocation;
    }

    float halfW = displayInfo.width / 2.0f;
    float halfH = displayInfo.height / 2.0f;

    ScreenLocation.X = halfW + (matrix[0] * WorldLocation.X + matrix[4] * WorldLocation.Y + matrix[8] * WorldLocation.Z + matrix[12]) / w * halfW;
    ScreenLocation.Y = halfH - (matrix[1] * WorldLocation.X + matrix[5] * WorldLocation.Y + matrix[9] * WorldLocation.Z + matrix[13]) / w * halfH;
    return ScreenLocation;
}

// ═══════════════════════════════════════════════════════════
// ★ 新增：批量 WorldToScreen（一次矩阵运算出3个坐标）
// ═══════════════════════════════════════════════════════════
void 绘制::WorldToScreenBatch(const FVector_class &WorldLoc,
                               float &outX, float &outY,
                               float &outFootY, float &outHeadY)
{
    float matrix[16];
    memcpy(matrix, 自身数据.矩阵, sizeof(matrix));

    float w = matrix[3] * WorldLoc.X + matrix[7] * WorldLoc.Y + matrix[11] * WorldLoc.Z + matrix[15];
    if (w < 0.01f)
    {
        outX = outY = outFootY = outHeadY = INFINITY;
        return;
    }

    float halfW = displayInfo.width / 2.0f;
    float halfH = displayInfo.height / 2.0f;
    float invW = 1.0f / w;

    // 屏幕中心X
    float baseX = halfW + (matrix[0] * WorldLoc.X + matrix[4] * WorldLoc.Y + matrix[8] * WorldLoc.Z + matrix[12]) * invW * halfW;
    // 屏幕中心Y（身体）
    float baseY = halfH - (matrix[1] * WorldLoc.X + matrix[5] * WorldLoc.Y + matrix[9] * WorldLoc.Z + matrix[13]) * invW * halfH;

    outX = baseX;
    outY = baseY;
    // 脚底Y（Z-5）
    outFootY = halfH - (matrix[1] * WorldLoc.X + matrix[5] * WorldLoc.Y + matrix[9] * (WorldLoc.Z - 5.0f) + matrix[13]) * invW * halfH;
    // 头顶Y（Z+身高）
    outHeadY = halfH - (matrix[1] * WorldLoc.X + matrix[5] * WorldLoc.Y + matrix[9] * (WorldLoc.Z + Offsets::HumanHeight) + matrix[13]) * invW * halfH;
}


// 更新 数据
void 绘制::更新地址数据()
{
    // ========== 基础地址 (使用新偏移) ==========
    地址.世界地址 = 读写.getPtr64(读写.getPtr64(地址.libue4 + Offsets::GWorld) + Offsets::GWorld_PersistentLevel);
    地址.自身地址 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + Offsets::GWorld) + Offsets::GWorld_ActorsCountDec) + 0x88) + 0x30) + 0x3478);

    // 静态视图矩阵链（不依赖玩家 Actor，死亡后依然有效）
    地址.矩阵地址 = 读写.getPtr64(读写.getPtr64(地址.libue4 + Offsets::MatrixChain1) + 0x20) + Offsets::Matrix_ViewMatrix;
    地址.矩阵地址_Tol = 读写.getPtr64(读写.getPtr64(地址.libue4 + Offsets::MatrixChain2) + Offsets::Matrix_Tol_Offset1) + Offsets::Matrix_Tol_Offset2;

    // 数组地址与数量（未解密时）
    地址.数组地址 = 读写.getPtr64(地址.世界地址 + Offsets::GWorld_ActorsArray);
    世界数量 = 读写.getDword(地址.世界地址 + Offsets::GWorld_ActorsCount);

    // 解密数组优先（如果已启用）
    if (按钮.解密)
    {
        地址.数组地址 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x141BF3F8) + 0xf8) + 0x138) + 0xf0);
        世界数量 = 读写.getDword(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x141BF3F8) + 0xf8) + 0x138) + 0xf8);
    }

    地址.类地址 = 读写.getPtr64(地址.libue4 + Offsets::ClassBase);

    // ========== 自身坐标 (通过 RootComponent + 0x200) ==========
    uintptr_t rootComp = 读写.getPtr64(地址.自身地址 + Offsets::Actor_RootComponent);
    if (rootComp != 0)
    {
        读写.readv(rootComp + 0x200, &自身数据.坐标, sizeof(自身数据.坐标));
    }

    // ========== 自身基础属性 ==========
    自身数据.自身队伍 = 读写.getDword(地址.自身地址 + Offsets::Actor_TeamID);
    自身数据.自身状态 = 读写.getDword(读写.getPtr64(地址.自身地址 + Offsets::Actor_PawnState));
    自身数据.开镜 = 读写.getDword(地址.自身地址 + Offsets::Actor_bIsGunADS);
    自身数据.开火 = 读写.getDword(地址.自身地址 + Offsets::Actor_bIsWeaponFiring);

    // ========== 手持武器信息 ==========
    uintptr_t weaponPtr = 读写.getPtr64(地址.自身地址 + Offsets::Actor_CurrentWeapon);
    if (weaponPtr != 0)
    {
        自身数据.手持id = 读写.getDword(weaponPtr + Offsets::Weapon_RepID);
        自身数据.手持 = heldconversion(自身数据.手持id);
    }

    // 从静态矩阵链直接读取 4x4 视图矩阵（死亡后依然有效）
    // 尝试使用矩阵链1（更通用）
    读写.readv(地址.矩阵地址, &自身数据.矩阵, sizeof(自身数据.矩阵));

    // 如果矩阵链1无效（全零），则回退到矩阵链2
    if (自身数据.矩阵[0] == 0.0f && 自身数据.矩阵[5] == 0.0f && 自身数据.矩阵[10] == 0.0f)
    {
        读写.readv(地址.矩阵地址_Tol, &自身数据.矩阵, sizeof(自身数据.矩阵));
    }

    // FOV 和准星 Yaw 仍然尝试从 PlayerController 读取（自瞄可能需要，读不到也无妨）
    uintptr_t controller = 读写.getPtr64(地址.自身地址 + Offsets::Controller_Offset);
    if (controller != 0)
    {
        uintptr_t camManager = 读写.getPtr64(controller + Offsets::Controller_CameraManager);
        if (camManager != 0)
        {
            自身数据.Fov = 读写.getFloat(camManager + Offsets::CameraManager_FOV);
            // 注：如果 FOV 读不到，自瞄可能受影响，但不影响绘制
        }
        自身数据.准星Y = 读写.getFloat(controller + Offsets::Controller_AimYaw) - 90.0f;
    }

    // ========== 人物高度 ==========
    自身数据.人物高度 = 读写.getFloat(地址.自身地址 + Offsets::Actor_SpeedValue);

    // ========== 手持握把 ==========
    uintptr_t weaponEntity = 读写.getPtr64(地址.自身地址 + Offsets::Actor_WeaponEntity);
    if (weaponEntity != 0)
    {
        uintptr_t weaponComp = 读写.getPtr64(weaponEntity + 0xBB8);
        if (weaponComp != 0)
        {
            自身数据.手持握把 = 读写.getDword(weaponComp + Offsets::Weapon_GripID);
        }
    }

    // ========== 全图人数统计 ==========
    自身数据.全图数量 = 读写.getDword(
        读写.getPtr64(读写.getPtr64(地址.libue4 + Offsets::GWorld) + Offsets::AliveNum) + Offsets::AllivePlayerNum);
    自身数据.真人数量 = 读写.getDword(
        读写.getPtr64(读写.getPtr64(地址.libue4 + Offsets::GWorld) + Offsets::AliveNum) + Offsets::AliveRealPlayerNum);
    自身数据.人机数量 = 自身数据.全图数量 - 自身数据.真人数量;
    自身数据.队伍数量 = 读写.getDword(
        读写.getPtr64(读写.getPtr64(地址.libue4 + Offsets::GWorld) + Offsets::AliveNum) + Offsets::AliveTeamNum);
}

ImColor 绘制::floatArrToImColor(float arr[4])
{
    return ImColor(arr[0] * 255, arr[1] * 255, arr[2] * 255, arr[3] * 255);
}

// ============================================================
// 坐标解密算法 - 在绘制.cpp中 更新对象数据() 函数前面添加
// ============================================================

// 全局解密缓存（也可放在函数内用static）
static std::unordered_map<uintptr_t, FVector_class> g_解密缓存;
static std::unordered_map<uintptr_t, bool> g_解密缓存已初始化;
static std::unordered_map<uintptr_t, std::chrono::steady_clock::time_point> g_缓存时间;
static std::unordered_map<uintptr_t, FVector_class> g_敌人速度缓存;

// 判断坐标是否被加密（Z轴突变检测）
bool 判断坐标是否加密(const FVector_class &坐标, const FVector_class &自身坐标)
{
    // 1. 坐标距离自身超过10000单位 → 明显加密
    float 距离 = sqrt(pow(坐标.X - 自身坐标.X, 2) + pow(坐标.Y - 自身坐标.Y, 2) + pow(坐标.Z - 自身坐标.Z, 2));
    if (距离 > 10000.0f)
        return true;

    // 2. Z轴与自身差距超过500单位 → Z被篡改
    if (fabs(坐标.Z - 自身坐标.Z) > 500.0f)
        return true;

    return false;
}

// 检测多个敌人在自身5米正下方
bool 检测下方敌人集群(const FVector_class &自身坐标)
{
    int 下方计数 = 0;
    float 水平阈值 = 500.0f; // 5米（游戏单位通常是cm）
    float 垂直上限 = 300.0f; // 下方3米内
    float 垂直下限 = 10.0f;  // 至少10cm在下方

    // 注：这里无法直接访问本轮所有敌人坐标，用距离判断替代
    // 检测逻辑在主循环外部也可以用另外的方式
    return false; // 简化版，在主循环中用更精确方式
}

// 核心解密函数 - 在坐标读取后立即调用
void 算法解密坐标(FVector_class &敌人坐标, const FVector_class &自身坐标, uintptr_t 敌人地址)
{
    bool 被加密 = 判断坐标是否加密(敌人坐标, 自身坐标);

    if (!被加密)
    {
        // 坐标正常 → 更新缓存
        g_解密缓存[敌人地址] = 敌人坐标;
        g_解密缓存已初始化[敌人地址] = true;
        return;
    }

    // 坐标被加密！
    if (g_解密缓存已初始化[敌人地址])
    {
        // 有历史缓存 → 用历史坐标 + Z轴修正
        FVector_class &缓存 = g_解密缓存[敌人地址];
        敌人坐标.X = 缓存.X;
        敌人坐标.Y = 缓存.Y;
        // Z轴用自身Z减去170（假设敌人在地面）
        敌人坐标.Z = 自身坐标.Z - 170.0f;
    }
    else
    {
        // 无缓存 → 直接用自身坐标（至少显示在附近）
        敌人坐标.X = 自身坐标.X;
        敌人坐标.Y = 自身坐标.Y;
        敌人坐标.Z = 自身坐标.Z - 170.0f;
        g_解密缓存[敌人地址] = 敌人坐标;
        g_解密缓存已初始化[敌人地址] = true;
    }
}
// 复用的矩阵转换（来自骨骼.hpp）
FMatrix TransformToMatrix(FTransform transform)
{
    FMatrix matrix;
    matrix.M[3][0] = transform.Translation.X;
    matrix.M[3][1] = transform.Translation.Y;
    matrix.M[3][2] = transform.Translation.Z;
    float x2 = transform.Rotation.X + transform.Rotation.X;
    float y2 = transform.Rotation.Y + transform.Rotation.Y;
    float z2 = transform.Rotation.Z + transform.Rotation.Z;
    float xx2 = transform.Rotation.X * x2;
    float yy2 = transform.Rotation.Y * y2;
    float zz2 = transform.Rotation.Z * z2;
    matrix.M[0][0] = (1 - (yy2 + zz2)) * transform.Scale3D.X;
    matrix.M[1][1] = (1 - (xx2 + zz2)) * transform.Scale3D.Y;
    matrix.M[2][2] = (1 - (xx2 + yy2)) * transform.Scale3D.Z;
    float yz2 = transform.Rotation.Y * z2;
    float wx2 = transform.Rotation.W * x2;
    matrix.M[2][1] = (yz2 - wx2) * transform.Scale3D.Z;
    matrix.M[1][2] = (yz2 + wx2) * transform.Scale3D.Y;
    float xy2 = transform.Rotation.X * y2;
    float wz2 = transform.Rotation.W * z2;
    matrix.M[1][0] = (xy2 - wz2) * transform.Scale3D.Y;
    matrix.M[0][1] = (xy2 + wz2) * transform.Scale3D.X;
    float xz2 = transform.Rotation.X * z2;
    float wy2 = transform.Rotation.W * y2;
    matrix.M[2][0] = (xz2 + wy2) * transform.Scale3D.Z;
    matrix.M[0][2] = (xz2 - wy2) * transform.Scale3D.X;
    matrix.M[0][3] = 0;
    matrix.M[1][3] = 0;
    matrix.M[2][3] = 0;
    matrix.M[3][3] = 1;
    return matrix;
}

FMatrix MatrixMulti(const FMatrix &m1, const FMatrix &m2)
{
    FMatrix matrix;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            matrix.M[i][j] = 0;
            for (int k = 0; k < 4; k++)
                matrix.M[i][j] += m1.M[i][k] * m2.M[k][j];
        }
    return matrix;
}

void 绘制::更新对象数据()
{
    if (按钮.雷达)
    {
        绘图.RenderRadarScan(ImGui::GetForegroundDrawList(), ImVec2(按钮.雷达X, 按钮.雷达Y), 150.0f, 100, 按钮.rotationAngle, 150.0f, 自身数据.准星Y);
    }
    int 绘制人机 = 0, 绘制真人 = 0;
    被瞄准对象数量 = 0;
    自瞄.瞄准对象数量 = 0;
    // 开发者模式：记录距离准星最近的物体
    static float minDistDebug = 9999.0f;
    static uintptr_t closestDebugAddr = 0;
    static std::string closestClassName;

    minDistDebug = 9999.0f;
    closestDebugAddr = 0;
    closestClassName.clear();
    bool 存在已开启的超级物资箱 = false; // 必须在 for 循环前声明
    for (int a = 0; a < 世界数量; a++)
    {
        // 主循环
        对象地址.敌人地址 = 读写.getPtr64(地址.数组地址 + a * 8);
        读写.readv(读写.getPtr64(对象地址.敌人地址 + Offsets::Actor_RootComponent) + 0x1B0, &对象信息.敌人信息.坐标, sizeof(对象信息.敌人信息.坐标));
        FVector_class &坐标 = 对象信息.敌人信息.坐标;
        if (按钮.坐标解密)
        {
            // ─── 帧间一致性检测 ───
            if (g_解密缓存已初始化[对象地址.敌人地址])
            {
                FVector_class &上一帧 = g_解密缓存[对象地址.敌人地址];

                // 计算偏移
                float 水平偏移 = sqrt(pow(坐标.X - 上一帧.X, 2) + pow(坐标.Y - 上一帧.Y, 2));
                float 垂直偏移 = fabs(坐标.Z - 上一帧.Z);

                // 加密判断
                bool 水平加密 = (水平偏移 > 800.0f);
                bool 垂直加密 = (垂直偏移 > 300.0f);

                // 替换原来的代码
                if (水平加密 || 垂直加密)
                {
                    FVector_class &速度 = g_敌人速度缓存[对象地址.敌人地址];
                    float dt = 0.05f; // 预测时间因子（根据帧率调整）

                    // XY用上一帧+速度预测
                    坐标.X = 上一帧.X + 速度.X * dt;
                    坐标.Y = 上一帧.Y + 速度.Y * dt;

                    // ★ 关键修复：Z用上一帧的Z + 速度预测，而不是用自身Z！
                    if (!垂直加密)
                    {
                        坐标.Z = 上一帧.Z + 速度.Z * dt;
                    }
                    else
                    {
                        // Z确实加密了，也用上一帧Z，不低于地面即可
                        坐标.Z = 上一帧.Z;
                        if (坐标.Z < -10000)
                            坐标.Z = 上一帧.Z; // 保护
                    }

                    g_缓存时间[对象地址.敌人地址] = std::chrono::steady_clock::now();
                }
                else
                {
                    // 正常更新缓存
                    g_解密缓存[对象地址.敌人地址] = 坐标;
                    g_缓存时间[对象地址.敌人地址] = std::chrono::steady_clock::now();
                    读写.readv(对象地址.敌人地址 + Offsets::Actor_Velocity,
                               &g_敌人速度缓存[对象地址.敌人地址], sizeof(FVector_class));
                }
            }
            else
            {
                // ─── 首次遇到：验证坐标是否合理 ───
                float 距离 = sqrt(pow(坐标.X - 自身数据.坐标.X, 2) + pow(坐标.Y - 自身数据.坐标.Y, 2) + pow(坐标.Z - 自身数据.坐标.Z, 2));

                if (距离 > 10.0f && 距离 < 300000.0f)
                {
                    g_解密缓存[对象地址.敌人地址] = 坐标;
                    g_解密缓存已初始化[对象地址.敌人地址] = true;
                    g_缓存时间[对象地址.敌人地址] = std::chrono::steady_clock::now();
                    读写.readv(对象地址.敌人地址 + Offsets::Actor_Velocity,
                               &g_敌人速度缓存[对象地址.敌人地址], sizeof(FVector_class));
                }
                else
                {
                    // 首次读取就是加密数据 → 用自身坐标作为基准
                    坐标.X = 自身数据.坐标.X;
                    坐标.Y = 自身数据.坐标.Y;
                    坐标.Z = 自身数据.坐标.Z - 170.0f;
                    g_解密缓存[对象地址.敌人地址] = 坐标;
                    g_解密缓存已初始化[对象地址.敌人地址] = true;
                    g_缓存时间[对象地址.敌人地址] = std::chrono::steady_clock::now();
                    memset(&g_敌人速度缓存[对象地址.敌人地址], 0, sizeof(FVector_class));
                    // ← 不再 continue！
                }
            }
        }
        // ★★★ 解密结束 ★★★

        对象信息.敌人信息.距离 = 计算.计算距离(自身数据.坐标, 对象信息.敌人信息.坐标);
        float sx, sy, footY, headY;
        WorldToScreenBatch(对象信息.敌人信息.坐标, sx, sy, footY, headY);
        FVector2D screenPos = {sx, sy};
        FVector2D footPos = {sx, footY};
        FVector2D headPos = {sx, headY};

        struct SmoothPos
        {
            float fx, fy, hz;
        };
        static std::unordered_map<uintptr_t, SmoothPos> smoothCache;

        // 增强屏幕有效性判断：防空框
        float screenW = ImGui::GetIO().DisplaySize.x;
        float screenH = ImGui::GetIO().DisplaySize.y;
        bool screenValid = (footPos.X != INFINITY && footPos.Y != INFINITY && headPos.Y != INFINITY && footPos.X > -screenW && footPos.X < screenW * 2 // 不过度偏离屏幕
                            && footPos.Y > -screenH && footPos.Y < screenH * 2 && headPos.Y > -screenH && headPos.Y < screenH * 2);

        if (!screenValid)
        {
            smoothCache.erase(对象地址.敌人地址); // 清理残留缓存
        }

        constexpr float kSmooth = 0.35f;
        auto &cached = smoothCache[对象地址.敌人地址];
        // 首次出现且有效时直接赋值，无效时填 0 确保后续 t_屏幕坐标.W < 0
        if (cached.fx == 0 && cached.fy == 0 && cached.hz == 0)
        {
            cached = screenValid ? SmoothPos{footPos.X, footPos.Y, headPos.Y} : SmoothPos{0, 0, 0};
        }
        else if (screenValid)
        {
            // 只有屏幕有效时才做平滑更新
            cached.fx += kSmooth * (footPos.X - cached.fx);
            cached.fy += kSmooth * (footPos.Y - cached.fy);
            cached.hz += kSmooth * (headPos.Y - cached.hz);
        }

        float r_x = cached.fx;
        float r_y = cached.fy;
        float r_z = cached.hz;
        float camear_r = (screenPos.X != INFINITY && screenPos.Y != INFINITY) ? 1.0f : -1.0f;

        // 根据屏幕有效状态构造绘制坐标，无效则强制 W < 0 → 不绘制
        D4DVector t_屏幕坐标;
        if (screenValid)
        {
            t_屏幕坐标 = {r_x - (r_y - r_z) / 4, r_y, (r_y - r_z) / 2, r_y - r_z};
        }
        else
        {
            t_屏幕坐标 = {0, 0, 0, -1.0f}; // W < 0 保证后续所有绘制被跳过
        }

        // 距离过滤仍然保留
        if (对象信息.敌人信息.距离 > Offsets::MaxDrawDistance)
        {
            continue;
        }
        // 正常绘制方框...
        char 计算地址[256] = "我是帅哥";
        sprintf(计算地址, "%lx", 对象地址.敌人地址);
        char 自救计算地址[256] = "我是篮子";
        sprintf(自救计算地址, "%lx", 对象地址.敌人地址);
        if (按钮.手雷预警)
        {
            int 手雷ID = 读写.getDword(对象地址.敌人地址 + 0x794);
            const char *投掷物信息 = Getagrenade(手雷ID);
            if (手雷ID == 602004 or 手雷ID == 9825004)
            {
                if (!计时器.hasTimer(计算地址))
                {
                    手雷类.add(计算地址, 对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y);
                }
                else
                {
                    手雷类.remove(计算地址);
                }
            }
            if (投掷物信息 != nullptr)
            {
                std::string name = 投掷物信息;
                if (t_屏幕坐标.W > 0)
                {
                    float 计算_max = 7 - (计时器.getTimerSeconds(计算地址) / 7 * (60 / ImGui::GetIO().Framerate * 0.115));
                    if (计算_max >= 0)
                    {
                        name += "[" + std::to_string((int)对象信息.敌人信息.距离) + "米]";
                        auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 35);
                        if (手雷ID != 602004 && 手雷ID != 9825004)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 35, {r_x - (textSize.x / 2), r_y + 30}, ImColor(255, 0, 0, 255), name.c_str());
                        }
                        if (手雷ID == 602004 or 手雷ID == 9825004)
                        {
                            float 计时 = 计算_max / 7 * 100;
                            float aa = 计时 * 3.6;
                            string 计算 = std::to_string((int)计算_max);
                            计算 += "秒";
                            textSize = ImGui::CalcTextSize(计算.c_str(), 0, 50);
                            ImGui::GetForegroundDrawList()->AddText(NULL, 50, {r_x - (textSize.x / 2), r_y - 18}, ImColor(255, 0, 0, 255), 计算.c_str());

                            string 距离 = std::to_string((int)对象信息.敌人信息.距离);
                            距离 += "米";
                            textSize = ImGui::CalcTextSize(距离.c_str(), 0, 32);
                            ImGui::GetForegroundDrawList()->AddText(NULL, 32, {r_x - (textSize.x / 2), r_y + 20}, ImColor(0, 255, 0, 255), 距离.c_str());
                            if (按钮.手雷样式 == 0)
                            {
                                绘图.ExplosionRange(
                                    D3DVector(对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y, 对象信息.敌人信息.坐标.Z),
                                    ImColor(255, 0, 0, 255), 350, 1.5f, 自身数据.矩阵);
                            }
                            else
                            {
                                绘图.ExplosionRange1(
                                    D3DVector(对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y, 对象信息.敌人信息.坐标.Z),
                                    ImColor(255, 0, 0, 255), 350, 1.5f, 自身数据.矩阵);
                            }
                        }
                    }
                }

                绘图::VecTor3 坐标 = {对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y, 对象信息.敌人信息.坐标.Z};
            }
        }

        char ClassName[64] = "";
        char 对象信息_max[200] = "";
        int ClassID = 读写.getPtr64(对象地址.敌人地址 + Offsets::Actor_ClassID);
        long int FNameEntry;
        if (t_屏幕坐标.W > 0)
        {
            FNameEntry = 读写.getPtr64(读写.getPtr64(地址.类地址 + (ClassID / 0x4000) * 0x8) + (ClassID % 0x4000) * 0x8);
            读写.readv(FNameEntry + 0xC, ClassName, 64);

            ImColor outlineColor = ImColor(0, 0, 0, 255);

            if (按钮.Debug && t_屏幕坐标.W > 0)
            {
                // 计算屏幕坐标到准星(屏幕中心)的距离
                float distToCenter = calculateDistance(PX, PY, r_x, r_y);
                if (distToCenter < minDistDebug)
                {
                    minDistDebug = distToCenter;
                    closestDebugAddr = 对象地址.敌人地址;
                    // 根据 Debug 模式选择类名或地址字符串
                    if (按钮.Debug模式 == 0)
                        closestClassName = std::string(ClassName);
                    else if (按钮.Debug模式 == 1)
                        closestClassName = std::string(计算地址);
                    else                                            // Debug模式 == 2 → 物资ID
                        closestClassName = std::to_string(ClassID); // ClassID 是 int 类型
                }

                // 以下是原有的绘制类名/地址代码，保持不变
                ImColor outlineColor = ImColor(0, 0, 0, 255);
                if (按钮.Debug模式 == 0)
                {
                    auto textSize = ImGui::CalcTextSize(ClassName, 0, 物资字体大小);
                    ImVec2 textPos = {r_x - (textSize.x / 2), r_y};
                    for (int x = -1; x <= 1; x++)
                    {
                        for (int y = -1; y <= 1; y++)
                        {
                            if (x != 0 || y != 0)
                            {
                                ImGui::GetForegroundDrawList()->AddText(NULL, 30, {textPos.x + x, textPos.y + y}, outlineColor, ClassName);
                            }
                        }
                    }
                    ImGui::GetForegroundDrawList()->AddText(NULL, 30, textPos, ImColor(255, 255, 255, 255), ClassName);
                }
                else if (按钮.Debug模式 == 1)
                {
                    auto textSize = ImGui::CalcTextSize(计算地址, 0, 物资字体大小);
                    ImVec2 textPos = {r_x - (textSize.x / 2), r_y};
                    for (int x = -1; x <= 1; x++)
                    {
                        for (int y = -1; y <= 1; y++)
                        {
                            if (x != 0 || y != 0)
                            {
                                ImGui::GetForegroundDrawList()->AddText(NULL, 30, {textPos.x + x, textPos.y + y}, outlineColor, 计算地址);
                            }
                        }
                    }
                    ImGui::GetForegroundDrawList()->AddText(NULL, 30, textPos, ImColor(255, 255, 255, 255), 计算地址);
                }
                else if (按钮.Debug模式 == 2)
                {
                    // ========== 绘制物资ID（新增） ==========
                    char idStr[32];
                    snprintf(idStr, sizeof(idStr), "%d", ClassID); // 十进制
                    // 或者想用十六进制：snprintf(idStr, sizeof(idStr), "0x%X", ClassID);

                    auto textSize = ImGui::CalcTextSize(idStr, 0, 物资字体大小);
                    ImVec2 textPos = {r_x - (textSize.x / 2), r_y};
                    for (int x = -1; x <= 1; x++)
                        for (int y = -1; y <= 1; y++)
                            if (x != 0 || y != 0)
                                ImGui::GetForegroundDrawList()->AddText(NULL, 30, {textPos.x + x, textPos.y + y}, outlineColor, idStr);
                    ImGui::GetForegroundDrawList()->AddText(NULL, 30, textPos, ImColor(255, 255, 255, 255), idStr);
                }
            }
            float Mlline = calculateDistance(PX, PY, t_屏幕坐标.X, t_屏幕坐标.Y);
            if (按钮.车辆)
            {
                long long VehicleData = 读写.getPtr64(对象地址.敌人地址 + Offsets::Vehicle_CommonData);
                float 载具血量 = 读写.getFloat(VehicleData + Offsets::Vehicle_CurrentHP) / 读写.getFloat(VehicleData + Offsets::Vehicle_MaxHP) * 100;
                float 载具油量 = 读写.getFloat(VehicleData + Offsets::Vehicle_CurrentFuel) / 读写.getFloat(VehicleData + Offsets::Vehicle_MaxFuel) * 100;
                if ((int)载具血量 != 0 && (int)载具油量 != 0 && 载具油量 <= 100 && 载具血量 <= 100 && 载具油量 >= 0 && 载具血量 >= 0)
                {
                    std::string name = getMaterialName(ClassName);
                    if (name != "Error" && 读写.getPtr64(地址.自身地址 + Offsets::Actor_Vehicle) != 对象地址.敌人地址 && 对象信息.敌人信息.距离 > 5)
                    {
                        name += std::to_string((int)对象信息.敌人信息.距离) + "米";

                        // 动态字号：远小近大，最大 20
                        float baseFontSize = 22.0f; // 最大字号
                        float minFontSize = 8.0f;   // 最远保留的最小字号
                        float maxDist = 100.0f;     // 开始缩小的距离

                        float scale = (对象信息.敌人信息.距离 > maxDist) ? (maxDist / 对象信息.敌人信息.距离) : 1.0f;
                        float fontSize = baseFontSize * scale;
                        if (fontSize < minFontSize)
                            fontSize = minFontSize;

                        // 准星对准远处的载具时，恢复最大字号（Mlline 为屏幕中心到载具屏幕坐标的距离）
                        const float aimThreshold = 80.0f; // 对准阈值，单位像素
                        if (Mlline < aimThreshold)
                        {
                            fontSize = baseFontSize;
                        }

                        ImColor color = ImColor(
                            static_cast<int>(车辆颜色[0] * 255 + 0.5),
                            static_cast<int>(车辆颜色[1] * 255 + 0.5),
                            static_cast<int>(车辆颜色[2] * 255 + 0.5),
                            static_cast<int>(车辆颜色[3] * 255 + 0.5));

                        // 计算载具名称大小与位置
                        auto textSize = ImGui::CalcTextSize(name.c_str(), 0, fontSize);
                        ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                        // 描边（黑色轮廓）
                        for (int x = -1; x <= 1; x++)
                        {
                            for (int y = -1; y <= 1; y++)
                            {
                                if (x != 0 || y != 0)
                                {
                                    ImGui::GetForegroundDrawList()->AddText(
                                        NULL, fontSize,
                                        {textPos.x + x, textPos.y + y},
                                        outlineColor, name.c_str());
                                }
                            }
                        }
                        // 主体文字
                        ImGui::GetForegroundDrawList()->AddText(NULL, fontSize, textPos, color, name.c_str());

                        // 血量/油量（仅当 Mlline <= 80 时显示，也使用缩放后的字号）
                        if (Mlline <= 80.0f)
                        {
                            char healthText[32];
                            sprintf(healthText, "血量: %.0f", 载具血量);
                            char fuelText[32];
                            sprintf(fuelText, "油量: %.0f", 载具油量);

                            // 用动态字号计算尺寸
                            ImVec2 healthTextSize = ImGui::CalcTextSize(healthText, 0, fontSize);
                            ImVec2 fuelTextSize = ImGui::CalcTextSize(fuelText, 0, fontSize);

                            ImVec2 healthPos = {r_x - (healthTextSize.x + fuelTextSize.x + 10) / 2, r_y + 25};
                            ImVec2 fuelPos = {r_x + (healthTextSize.x + fuelTextSize.x + 10) / 2 - fuelTextSize.x, r_y + 25};

                            // 血量描边
                            for (int x = -1; x <= 1; x++)
                            {
                                for (int y = -1; y <= 1; y++)
                                {
                                    if (x != 0 || y != 0)
                                    {
                                        ImGui::GetForegroundDrawList()->AddText(NULL, fontSize,
                                                                                {healthPos.x + x, healthPos.y + y}, outlineColor, healthText);
                                    }
                                }
                            }
                            ImGui::GetForegroundDrawList()->AddText(NULL, fontSize, healthPos, ImColor(255, 0, 0, 255), healthText);

                            // 油量描边
                            for (int x = -1; x <= 1; x++)
                            {
                                for (int y = -1; y <= 1; y++)
                                {
                                    if (x != 0 || y != 0)
                                    {
                                        ImGui::GetForegroundDrawList()->AddText(NULL, fontSize,
                                                                                {fuelPos.x + x, fuelPos.y + y}, outlineColor, fuelText);
                                    }
                                }
                            }
                            ImGui::GetForegroundDrawList()->AddText(NULL, fontSize, fuelPos, ImColor(0, 255, 0, 255), fuelText);
                        }
                    }
                }
            }

            if (按钮.物资总开关)
            {
                int MaterialID = 读写.getDword(对象地址.敌人地址 + Offsets::Box_Open);
                std::string name = getBoxName(MaterialID);
                if (name != "Error")
                {
                    name += "[";
                    name += std::to_string((int)对象信息.敌人信息.距离);
                    name += "米]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                    ImVec2 textPos = {r_x - (textSize.x / 2), r_y};
                    ImColor textColor = 绘制::floatArrToImColor(绘制::物资颜色);

                    for (int x = -1; x <= 1; x++)
                    {
                        for (int y = -1; y <= 1; y++)
                        {
                            if (x != 0 || y != 0)
                            {
                                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                            }
                        }
                    }
                    ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, textColor, name.c_str());
                }
            }
            if (按钮.显示类名)
            {
                float textY = t_屏幕坐标.Y - 30;
                ImColor textColor = ImColor(255, 255, 255, 255);
                ImColor outlineColor = ImColor(0, 0, 0, 255);
                float fontSize = 20.0f;

                auto textSize = ImGui::CalcTextSize(ClassName, 0, fontSize);

                for (int i = -1; i <= 1; i++)
                {
                    for (int j = -1; j <= 1; j++)
                    {
                        if (i != 0 || j != 0)
                        {
                            ImGui::GetForegroundDrawList()->AddText(
                                NULL,
                                fontSize,
                                {t_屏幕坐标.X - (textSize.x / 2) + i, textY + j},
                                outlineColor,
                                ClassName);
                        }
                    }
                }

                ImGui::GetForegroundDrawList()->AddText(
                    NULL,
                    fontSize,
                    {t_屏幕坐标.X - (textSize.x / 2), textY},
                    textColor,
                    ClassName);
            }
            if (按钮.绘制空投)
            {
                if (
                    strstr(ClassName, "BP_AirDropBox_Helicopter_C") != 0 ||
                    strstr(ClassName, "BP_CG036_AirDropBox_C") != 0 ||
                    strstr(ClassName, "BP_CGO36_AirDropBox_C") != 0 ||
                    strstr(ClassName, "PickUpListWrapperActor") != 0 ||
                    strstr(ClassName, "PlayerDeadInventoryBox_C") != 0 ||
                    strstr(ClassName, "AirDropListWrapperActor") != 0 ||
                    strstr(ClassName, "TreasureBox_Main_C") != 0 ||
                    strstr(ClassName, "BP_AirDropBox_C") != 0)
                {
                    std::string name = "空投[";
                    name += std::to_string((int)对象信息.敌人信息.距离);
                    name += "米]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                    ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                    ImColor textColor = ImColor(255, 0, 0, 255);
                    ImColor outlineColor = ImColor(0, 0, 0, 255);

                    for (int x = -1; x <= 1; x++)
                    {
                        for (int y = -1; y <= 1; y++)
                        {
                            if (x != 0 || y != 0)
                            {
                                ImGui::GetForegroundDrawList()->AddText(
                                    NULL, 30,
                                    {textPos.x + x, textPos.y + y},
                                    outlineColor,
                                    name.c_str());
                            }
                        }
                    }

                    ImGui::GetForegroundDrawList()->AddText(
                        NULL, 30, textPos,
                        textColor,
                        name.c_str());
                }
                else if (strstr(ClassName, "BP_AirDropPlane_Helicopter_C") != 0 ||
                         strstr(ClassName, "BP_AirDropPlane_C") != 0)
                {
                    std::string name = "空投飞机来了[";
                    name += std::to_string((int)对象信息.敌人信息.距离);
                    name += "米]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                    ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                    ImColor textColor = ImColor(255, 255, 0, 255); // 黄色
                    ImColor outlineColor = ImColor(0, 0, 0, 255);

                    for (int x = -1; x <= 1; x++)
                    {
                        for (int y = -1; y <= 1; y++)
                        {
                            if (x != 0 || y != 0)
                            {
                                ImGui::GetForegroundDrawList()->AddText(NULL, 30,
                                                                        {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                            }
                        }
                    }
                    ImGui::GetForegroundDrawList()->AddText(NULL, 30, textPos, textColor, name.c_str());
                }
            }

            if (按钮.显示自救器 && strstr(ClassName, "BP_revivalAED_Pickup_C") != 0)
            {
                std::string name = "自救器[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x != 0 || y != 0)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                        }
                    }
                }
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (按钮.显示飞索 && strstr(ClassName, "BP_Pickup_Finger_C") != 0)
            {
                std::string name = "飞索[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x != 0 || y != 0)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                        }
                    }
                }
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (按钮.密室钥匙 && strstr(ClassName, "BP_PickupWrapper_Key_C") != 0)
            {
                std::string name = "密室钥匙[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x != 0 || y != 0)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                        }
                    }
                }
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (按钮.显示密钥)
            {
                std::string keyName;
                ImColor keyColor;
                ImColor keyOutlineColor = ImColor(0, 0, 0, 255); // 默认描边，可被覆盖

                if (strstr(ClassName, "BP_CommercialWrapper_LV1_C") != 0)
                {
                    keyName = "侦察兵密钥";
                    keyColor = ImColor(208, 138, 71, 255);     // 铜色
                    keyOutlineColor = ImColor(0, 255, 0, 255); // 绿色描边
                }
                else if (strstr(ClassName, "BP_CommercialWrapper_LV2_C") != 0)
                {
                    keyName = "突击兵密钥";
                    keyColor = ImColor(192, 192, 192, 255);      // 银色
                    keyOutlineColor = ImColor(0, 150, 255, 255); // 蓝色描边
                }
                else if (strstr(ClassName, "BP_CommercialWrapper_LV3_C") != 0)
                {
                    keyName = "特种兵密钥";
                    keyColor = ImColor(220, 220, 255, 255);      // 高亮银 + 钻石亮（白偏蓝）
                    keyOutlineColor = ImColor(255, 215, 0, 255); // 金色描边（突出重要性）
                }
                else if (strstr(ClassName, "BP_CommercialWrapper_LV4_C") != 0)
                {
                    keyName = "指挥官密钥";
                    keyColor = ImColor(255, 215, 0, 255);          // 亮金
                    keyOutlineColor = ImColor(185, 242, 255, 255); // 钻石亮色（蓝白）
                }
                if (!keyName.empty())
                {
                    std::string name = keyName + "[" + std::to_string((int)对象信息.敌人信息.距离) + "米]";
                    float fontSize = 25.0f; // 固定字号25
                    ImVec2 textSize = ImGui::CalcTextSize(name.c_str(), 0, fontSize);
                    ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                    // 先绘制描边（使用对应描边颜色）
                    for (int x = -1; x <= 1; x++)
                        for (int y = -1; y <= 1; y++)
                            if (x != 0 || y != 0)
                                ImGui::GetForegroundDrawList()->AddText(NULL, fontSize,
                                                                        {textPos.x + x, textPos.y + y}, keyOutlineColor, name.c_str());

                    // 绘制主文字
                    ImGui::GetForegroundDrawList()->AddText(NULL, fontSize, textPos, keyColor, name.c_str());
                }
            }

            if (按钮.绘制信号枪 &&
                (strstr(ClassName, "Pistol_Flaregun_") != 0 ||
                 strstr(ClassName, "Pistol_RevivalFlaregun_Wrapper") != 0 ||
                 strstr(ClassName, "BP_Pistol_RevivalFlaregun_Wrappe") != 0 ||
                 strstr(ClassName, "BP_Pistol_RevivalFlaregun_Wrapper_C") != 0 ||
                 strstr(ClassName, "BP_Pistol_Flargun_Wrapper_C") != 0 ||
                 strstr(ClassName, "BP_Pistol_Flaregun_10000_C") != 0 ||
                 strstr(ClassName, "BP_Pistol_Flaregun_C") != 0 ||
                 strstr(ClassName, "BP_Pistol_RevivalFlaregun_C") != 0 ||
                 strstr(ClassName, "BP_Ammo_RevivalFlare_Pickup_C") != 0))
            {
                // 判断类型：召回类名包含 "Revival"
                bool isRevival = (strstr(ClassName, "Revival") != nullptr);
                // 判断是信号弹（Ammo_）还是信号枪
                bool isAmmo = (strstr(ClassName, "Ammo_") != nullptr);

                std::string name = isRevival ? "召回信号" : "物资信号";
                name += isAmmo ? "弹[" : "枪[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";

                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 召回绿色，物资红色
                ImColor textColor = isRevival ? ImColor(0, 255, 0, 255) : ImColor(255, 0, 0, 255);

                // 黑色描边
                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x != 0 || y != 0)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小,
                                                                    {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                        }
                    }
                }
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, textColor, name.c_str());
            }

            if (按钮.显示古墓树木 && (strstr(ClassName, "BP_LostTomb_TreeHole_C") != 0))
            {
                std::string name = "树";

                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                ImGui::GetForegroundDrawList()->AddText(NULL, 30, {r_x - (textSize.x / 2), r_y}, ImColor(255, 255, 0, 255), name.c_str());
            }

            if (按钮.显示古墓篮子 && (strstr(ClassName, "BP_LostTomb_Basket_C") != 0))
            {
                std::string name = "篮子";

                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                ImGui::GetForegroundDrawList()->AddText(NULL, 30, {r_x - (textSize.x / 2), r_y}, ImColor(255, 255, 0, 255), name.c_str());
            }

            if (按钮.显示古墓首饰盒 && (strstr(ClassName, "BP_LostTomb_Container1_C") != 0))
            {
                std::string name = "首饰盒";

                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                ImGui::GetForegroundDrawList()->AddText(NULL, 30, {r_x - (textSize.x / 2), r_y}, ImColor(255, 255, 0, 255), name.c_str());
            }

            int 古墓开启状态 = 读写.getDword(对象地址.敌人地址 + 0x260);
            if (按钮.显示古墓宝箱 && (strstr(ClassName, "BP_LostTomb_SupplyBox1_C") != 0))
            {
                if (按钮.隐藏古墓已开启 && 古墓开启状态 == 1)
                {
                }
                else
                {
                    std::string name = "古墓宝箱";
                    name += Level(ClassName);
                    if (古墓开启状态 == 1)
                    {
                        name += "开";
                    }
                    else
                    {
                        name += "未开";
                    }
                    name += "" + std::to_string(static_cast<int>(对象信息.敌人信息.距离)) + "m";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 按钮.物资字体大小);
                    ImGui::GetForegroundDrawList()->AddText(NULL, 按钮.物资字体大小, {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y}, ImColor(255, 255, 0, 255), name.c_str());
                }
            }

            if (按钮.显示古墓精致宝箱 && (strstr(ClassName, "BP_LostTomb_SupplyBox1_2_C") != 0))
            {
                if (按钮.隐藏古墓已开启 && 古墓开启状态 == 1)
                {
                }
                else
                {
                    std::string name = "古墓精致宝箱";
                    name += Level(ClassName);
                    if (古墓开启状态 == 1)
                    {
                        name += "开";
                    }
                    else
                    {
                        name += "未开";
                    }
                    name += "" + std::to_string(static_cast<int>(对象信息.敌人信息.距离)) + "m";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 按钮.物资字体大小);
                    ImGui::GetForegroundDrawList()->AddText(NULL, 按钮.物资字体大小, {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y}, ImColor(255, 255, 0, 255), name.c_str());
                }
            }

            if (按钮.显示古墓华贵宝箱 && (strstr(ClassName, "BP_LostTomb_SupplyBox1_3_C") != 0))
            {
                if (按钮.隐藏古墓已开启 && 古墓开启状态 == 1)
                {
                }
                else
                {
                    std::string name = "古墓华贵宝箱";
                    name += Level(ClassName);
                    if (古墓开启状态 == 1)
                    {
                        name += "开";
                    }
                    else
                    {
                        name += "未开";
                    }
                    name += "" + std::to_string(static_cast<int>(对象信息.敌人信息.距离)) + "m";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 按钮.物资字体大小);
                    ImGui::GetForegroundDrawList()->AddText(NULL, 按钮.物资字体大小, {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y}, ImColor(255, 255, 0, 255), name.c_str());
                }
            }

            if (按钮.解密)
            {
                对象地址.敌人地址 = 读写.getPtr64(读写.getPtr64(地址.数组地址 + a * decrypt_zero_x()) + decrypt_zero_y());
            }
            else
            {
                对象地址.敌人地址 = 读写.getPtr64(地址.数组地址 + a * 8);
            }

            if (按钮.显示黑色物资箱 && (strstr(ClassName, "BP_WAlnnerWrapperList_C") != 0))
            {
                std::string name = "黑色物资箱[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x != 0 || y != 0)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                        }
                    }
                }
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (按钮.爆炸猎弓 && strstr(ClassName, "BP_Other_HuntingBow_Wrapper_C") != 0)
            {
                std::string name = "爆炸猎弓[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x != 0 || y != 0)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                        }
                    }
                }
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            // 🔴 第一阶段：先扫描所有对象，标记是否存在已开启的箱子
            if (按钮.超级物资箱 && strstr(ClassName, "BP_MilitaryInnerWrapperList_C") != 0)
            {
                // 标记：场景里有已开的箱子
                存在已开启的超级物资箱 = true;
            }

            // 🟢 第二阶段：绘制逻辑（严格互斥，只显示一个）
            if (按钮.超级物资箱)
            {
                // ====================== 1. 绘制【已开启】箱子（最高优先级）
                if (strstr(ClassName, "BP_MilitaryInnerWrapperList_C") != 0)
                {
                    std::string name = "超级物资箱[已开]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                    ImVec2 textPos = {r_x - (textSize.x / 2), r_y};
                    ImColor textColor = ImColor(128, 128, 128, 255);
                    ImColor outlineColor = ImColor(0, 0, 0, 255);

                    // 描边
                    for (int x = -1; x <= 1; x++)
                        for (int y = -1; y <= 1; y++)
                            if (x != 0 || y != 0)
                                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                    // 主文字
                    ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, textColor, name.c_str());
                    continue;
                }

                // ====================== 2. 绘制【未开启】箱子（只有【没有已开箱子】时才显示）
                if (!存在已开启的超级物资箱 &&
                    (strstr(ClassName, "EscapeBox_SpeEffect_C") != 0 ||
                     strstr(ClassName, "MilitarySupplyBoxBase_Baltic_Classic_C") != 0 ||
                     strstr(ClassName, "MilitarySupplyBoxBase_Baltic_Theme_C") != 0 ||
                     strstr(ClassName, "EscopeBox_SpeEffect_C") != 0))
                {
                    std::string name = "超级物资箱[未开 " + std::to_string((int)对象信息.敌人信息.距离) + "米]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                    ImVec2 textPos = {r_x - (textSize.x / 2), r_y};
                    ImColor textColor = ImColor(255, 0, 255, 255);
                    ImColor outlineColor = ImColor(0, 0, 0, 255);

                    // 描边
                    for (int x = -1; x <= 1; x++)
                        for (int y = -1; y <= 1; y++)
                            if (x != 0 || y != 0)
                                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                    // 主文字
                    ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, textColor, name.c_str());
                    continue;
                }
            }

            if (按钮.显示防具 && (strstr(ClassName, "ckUp_BP_Helmet_Lv3_C") != 0 || strstr(ClassName, "PickUp_BP_Helmet_Lv3_C") != 0))
            {
                std::string name = "三级头[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x != 0 || y != 0)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                        }
                    }
                }
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 128, 0, 255), name.c_str());
            }
            if (按钮.显示防具 && (strstr(ClassName, "ckUp_BP_Armor_Lv3_C") != 0 || strstr(ClassName, "PickUp_BP_Armor_Lv3_C") != 0))
            {
                std::string name = "三级甲[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x != 0 || y != 0)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                        }
                    }
                }
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(0, 120, 255, 255), name.c_str());
            }

            if (按钮.显示防具 && (strstr(ClassName, "ckUp_BP_Bag_Lv3_C") != 0 || strstr(ClassName, "PickUp_BP_Bag_Lv3_B_C") != 0))
            {
                std::string name = "三级包[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x != 0 || y != 0)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                        }
                    }
                }
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (按钮.绘制信号枪 && strstr(ClassName, "Ammo_Flare") != 0)
            {
                std::string name = "信号枪子弹[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x != 0 || y != 0)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                        }
                    }
                }
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 0, 255), name.c_str());
            }

            if (按钮.绘制金插 && strstr(ClassName, "SuperPeopleSkillReplaceActor1_BP_C") != 0)
            {
                std::string name = "金色插件[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y};

                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x != 0 || y != 0)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                        }
                    }
                }
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (按钮.盒子)
            {
                std::string classNameStr(ClassName);
                if (classNameStr.find("CharacterDeadInventoryBox_C") != std::string::npos ||
                    /*classNameStr.find("PickUpListWrapperActor") != std::string::npos || */
                    classNameStr.find("RollTombBox_") != std::string::npos ||
                    classNameStr.find("EscapePlayerTombBox") != std::string::npos ||
                    classNameStr.find("DeadInventoryBox") != std::string::npos ||
                    classNameStr.find("_TrainingBoxLi") != std::string::npos)
                {
                    std::string lootText = "盒子[" + std::to_string((int)对象信息.敌人信息.距离) + "米]";
                    ImVec2 textPos = {r_x, r_y};
                    const char *text = lootText.c_str();
                    float fontSize = 15.0f; // 可根据需要调整

                    // 使用字体精确计算文本尺寸（避免默认CalcTextSize包含额外间距）
                    ImFont *font = ImGui::GetFont();
                    ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);

                    // 紧凑内边距：水平2px，垂直1px（或0px）
                    float paddingX = 2.0f;
                    float paddingY = 1.0f; // 调小此处可降低矩形高度

                    ImVec2 rectMin = {textPos.x - textSize.x / 2.0f - paddingX, textPos.y - textSize.y / 2.0f - paddingY};
                    ImVec2 rectMax = {textPos.x + textSize.x / 2.0f + paddingX, textPos.y + textSize.y / 2.0f + paddingY};

                    ImColor yellowColor = ImColor(255, 255, 0, 255);
                    ImDrawList *draw = ImGui::GetForegroundDrawList();

                    // 绘制边框（圆角值可调，999表示最大圆角即半圆，实际效果接近圆角矩形）
                    draw->AddRect(rectMin, rectMax, yellowColor, 5.0f, 0, 1.0f); // 圆角半径5px更自然

                    // 绘制文本（居中对齐）
                    ImVec2 textDrawPos = {rectMin.x + paddingX, rectMin.y + paddingY};
                    draw->AddText(font, fontSize, textDrawPos, yellowColor, text);
                }
            }

            int 开启状态 = 读写.getDword(对象地址.敌人地址 + Offsets::Box_OpenState);
            if (按钮.绘制宝箱 && (strstr(ClassName, "EscapeBox_SupplyBox_") != 0 or strstr(ClassName, "EscapeBoxHight_SupplyBox_") != 0))
            {
                if (按钮.隐藏已开启 && 开启状态 == 1)
                {
                }
                else
                {
                    std::string name = "宝箱 ";
                    name += Level(ClassName);
                    if (开启状态 == 1)
                    {
                        name += " 已开启";
                    }
                    else
                    {
                        name += " 未开启";
                    }
                    name += "[" + std::to_string(static_cast<int>(对象信息.敌人信息.距离)) + "M]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                    ImVec2 textPos = {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y};

                    for (int x = -1; x <= 1; x++)
                    {
                        for (int y = -1; y <= 1; y++)
                        {
                            if (x != 0 || y != 0)
                            {
                                ImGui::GetForegroundDrawList()->AddText(NULL, 25, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                            }
                        }
                    }
                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, textPos, ImColor(255, 255, 0, 255), name.c_str());
                }
            }

            if (按钮.绘制药箱 && (strstr(ClassName, "EscapeBox_Medical_") != 0 or strstr(ClassName, "EscapeBoxHight_Medical_") != 0))
            {
                if (按钮.隐藏已开启 && 开启状态 == 1)
                {
                }
                else
                {
                    std::string name = "药箱 ";
                    name += Level(ClassName);
                    if (开启状态 == 1)
                    {
                        name += " 已开启";
                    }
                    else
                    {
                        name += " 未开启";
                    }
                    name += "[" + std::to_string(static_cast<int>(对象信息.敌人信息.距离)) + "M]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                    ImVec2 textPos = {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y};

                    for (int x = -1; x <= 1; x++)
                    {
                        for (int y = -1; y <= 1; y++)
                        {
                            if (x != 0 || y != 0)
                            {
                                ImGui::GetForegroundDrawList()->AddText(NULL, 25, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                            }
                        }
                    }
                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, textPos, ImColor(255, 255, 0, 255), name.c_str());
                }
            }

            // 先判断是否是盒子类
            bool isBoxLike = (
                // 原有盒子
                strstr(ClassName, "CharacterDeadInventoryBox_C") ||
                strstr(ClassName, "PickUpListWrapperActor") ||
                strstr(ClassName, "RollTombBox_") ||
                strstr(ClassName, "EscapePlayerTombBox") ||
                strstr(ClassName, "_TrainingBoxLi") ||
                strstr(ClassName, "BP_PlayerDeadListWrapper_C") ||
                strstr(ClassName, "DeadInventoryBox") ||
                // 空投类（包含各种变体）
                strstr(ClassName, "BP_AirDropBox_Helicopter_C") ||
                strstr(ClassName, "BP_CG036_AirDropBox_C") ||
                strstr(ClassName, "BP_CGO36_AirDropBox_C") ||
                strstr(ClassName, "PickUpListWrapperActor") ||
                strstr(ClassName, "PlayerDeadInventoryBox_C") ||
                strstr(ClassName, "AirDropListWrapperActor") ||
                strstr(ClassName, "TreasureBox_Main_C") ||
                strstr(ClassName, "BP_AirDropBox_C") ||
                strstr(ClassName, "TreasureBox_Main_C") || // 金色宝箱
                // 密室箱子（如果你希望它们也能显示内部物资）
                strstr(ClassName, "EscapeBox_SupplyBox_") ||
                strstr(ClassName, "EscapeBoxHight_SupplyBox_") ||
                strstr(ClassName, "EscapeBox_Weapon_") ||
                strstr(ClassName, "EscapeBoxHight_Weapon_"));

            // 判断是否为空投类（用于调距）
            bool isAirDrop = (strstr(ClassName, "BP_AirDropBox_Helicopter_C") ||
                              strstr(ClassName, "BP_CG036_AirDropBox_C") ||
                              strstr(ClassName, "BP_CGO36_AirDropBox_C") ||
                              strstr(ClassName, "PickUpListWrapperActor") ||
                              strstr(ClassName, "PlayerDeadInventoryBox_C") ||
                              strstr(ClassName, "AirDropListWrapperActor") ||
                              strstr(ClassName, "TreasureBox_Main_C") ||
                              strstr(ClassName, "BP_AirDropBox_C") ||
                              strstr(ClassName, "TreasureBox_Main_C"));

            // 动态距离限制：空投 600 米，普通盒子 100 米
            float boxMaxDist = isAirDrop ? 600.0f : 100.0f;
            float aimThreshold = isAirDrop ? 200.0f : 100.0f; // 准星对准范围也可以适当放宽

            if (按钮.盒子物资 && isBoxLike && 对象信息.敌人信息.距离 <= boxMaxDist)
            {
                float aimDistance = sqrtf(powf(PX - r_x, 2.0f) + powf(PY - r_y, 2.0f));
                if (aimDistance < aimThreshold && 自瞄.瞄准目标 == -1)
                {
                    // 读取物资列表（偏移 0xD98）
                    uintptr_t listBase = 读写.getPtr64(对象地址.敌人地址 + Offsets::BoxPickUpDataList); // 0xD98
                    int 盒内物资数量 = 0;

                    int countField = 读写.getDword(对象地址.敌人地址 + Offsets::BoxPickUpDataList + 0x8);
                    if (countField > 0 && countField < 1000)
                        盒内物资数量 = countField;
                    else
                    {
                        int countHeader = 读写.getDword(listBase);
                        if (countHeader > 0 && countHeader < 1000)
                            盒内物资数量 = countHeader;
                    }

                    if (listBase != 0 && 盒内物资数量 > 0)
                    {
                        uintptr_t 物资数组 = listBase + 0x4;
                        int 文本高度 = 50;
                        for (int i = 0; i < 盒内物资数量; i++)
                        {
                            int 物资地址ID = 读写.getDword(物资数组 + 0x38 * i);
                            int 物资地址数量 = 读写.getDword((物资数组 + 0x38 * i) + 0x14);
                            std::string name = getBoxName1(物资地址ID);
                            if (name != "NULL" && name != "Error")
                            {
                                name += "[" + std::to_string(物资地址数量) + "]";

                                // ====================== 核心修改：密钥突出显示（复用显示密钥颜色） ======================
                                ImColor 文字颜色;
                                ImColor 描边颜色 = ImColor(0, 0, 0, 255); // 默认黑色描边
                                float 文字字号 = 30.0f;                   // 默认普通物资字号

                                // 匹配8个密钥ID，绑定专属颜色+放大字号
                                switch (物资地址ID)
                                {
                                // 侦察兵密钥
                                case 604171:
                                case 9826010:
                                    文字颜色 = ImColor(208, 138, 71, 255); // 铜色
                                    描边颜色 = ImColor(0, 255, 0, 255);    // 绿色描边
                                    文字字号 = 42.0f;                      // 密钥放大字号
                                    break;
                                // 突击兵密钥
                                case 604172:
                                case 9826011:
                                    文字颜色 = ImColor(192, 192, 192, 255); // 银色
                                    描边颜色 = ImColor(0, 150, 255, 255);   // 蓝色描边
                                    文字字号 = 42.0f;
                                    break;
                                // 特种兵密钥
                                case 604173:
                                case 9826012:
                                    文字颜色 = ImColor(220, 220, 255, 255); // 高亮银
                                    描边颜色 = ImColor(255, 215, 0, 255);   // 金色描边
                                    文字字号 = 42.0f;
                                    break;
                                // 指挥官密钥
                                case 604174:
                                case 9826013:
                                    文字颜色 = ImColor(255, 215, 0, 255);   // 亮金色
                                    描边颜色 = ImColor(185, 242, 255, 255); // 钻石蓝描边
                                    文字字号 = 42.0f;
                                    break;
                                // 普通物资：随机颜色+默认字号
                                default:
                                    文字颜色 = GetRandomColorById(物资地址ID);
                                    文字字号 = 30.0f;
                                    break;
                                }

                                // 计算文本尺寸 + 动态行间距（字号大则间距大）
                                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 文字字号);
                                文本高度 += (文字字号 > 30.0f) ? 35 : 25;
                                ImVec2 textPos = {r_x - (textSize.x / 2), r_y - 文本高度};

                                // 绘制描边（密钥用专属描边，普通物资黑色描边）
                                ImGui::GetBackgroundDrawList()->AddText(NULL, 文字字号, {textPos.x - 1, textPos.y - 1}, 描边颜色, name.c_str());
                                ImGui::GetBackgroundDrawList()->AddText(NULL, 文字字号, {textPos.x + 1, textPos.y + 1}, 描边颜色, name.c_str());
                                // 绘制主体文字
                                ImGui::GetForegroundDrawList()->AddText(NULL, 文字字号, textPos, 文字颜色, name.c_str());
                                // ==================================================================================
                            }
                        }
                    }
                }
            }

            // if (按钮.盒子物资 && isBoxLike && 对象信息.敌人信息.距离 <= boxMaxDist)
            // {
            //     float aimDistance = sqrtf(powf(PX - r_x, 2.0f) + powf(PY - r_y, 2.0f));
            //     if (aimDistance < aimThreshold && 自瞄.瞄准目标 == -1)
            //     {
            //         // 读取物资列表（偏移 0xD98）
            //         uintptr_t listBase = 读写.getPtr64(对象地址.敌人地址 + Offsets::BoxPickUpDataList); // 0xD98
            //         int 盒内物资数量 = 0;

            //         int countField = 读写.getDword(对象地址.敌人地址 + Offsets::BoxPickUpDataList + 0x8);
            //         if (countField > 0 && countField < 1000)
            //             盒内物资数量 = countField;
            //         else
            //         {
            //             int countHeader = 读写.getDword(listBase);
            //             if (countHeader > 0 && countHeader < 1000)
            //                 盒内物资数量 = countHeader;
            //         }

            //         if (listBase != 0 && 盒内物资数量 > 0)
            //         {
            //             uintptr_t 物资数组 = listBase + 0x4;
            //             int 文本高度 = 50;
            //             for (int i = 0; i < 盒内物资数量; i++)
            //             {
            //                 int 物资地址ID = 读写.getDword(物资数组 + 0x38 * i);
            //                 int 物资地址数量 = 读写.getDword((物资数组 + 0x38 * i) + 0x14);
            //                 std::string name = getBoxName1(物资地址ID);
            //                 if (name != "NULL" && name != "Error")
            //                 {
            //                     name += "[" + std::to_string(物资地址数量) + "]";
            //                     auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
            //                     文本高度 += 25;
            //                     ImVec2 textPos = {r_x - (textSize.x / 2), r_y - 文本高度};
            //                     ImGui::GetBackgroundDrawList()->AddText(NULL, 30,
            //                                                             {textPos.x - 1, textPos.y - 1}, ImColor(0, 0, 0, 255), name.c_str());
            //                     ImGui::GetBackgroundDrawList()->AddText(NULL, 30,
            //                                                             {textPos.x + 1, textPos.y + 1}, ImColor(0, 0, 0, 255), name.c_str());
            //                     ImGui::GetForegroundDrawList()->AddText(NULL, 30, textPos,
            //                                                             GetRandomColorById(物资地址ID), name.c_str());
            //                 }
            //             }
            //         }
            //     }
            // }

            if (按钮.绘制武器箱 && (strstr(ClassName, "EscapeBox_Weapon_") != 0 or strstr(ClassName, "EscapeBoxHight_Weapon_") != 0))
            {
                if (按钮.隐藏已开启 && 开启状态 == 1)
                {
                }
                else
                {
                    std::string name = "武器箱 ";
                    name += Level(ClassName);
                    if (开启状态 == 1)
                    {
                        name += " 已开启";
                    }
                    else
                    {
                        name += " 未开启";
                    }
                    name += "[" + std::to_string(static_cast<int>(对象信息.敌人信息.距离)) + "M]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                    ImVec2 textPos = {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y};

                    for (int x = -1; x <= 1; x++)
                    {
                        for (int y = -1; y <= 1; y++)
                        {
                            if (x != 0 || y != 0)
                            {
                                ImGui::GetForegroundDrawList()->AddText(NULL, 25, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                            }
                        }
                    }
                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, textPos, ImColor(255, 255, 0, 255), name.c_str());
                }
            }

            if (按钮.自定义物资开关 && g_CustomReader && g_CustomDataLoaded)
            {
                // 确保数据已加载，若未加载则尝试一次
                if (!g_CustomDataLoaded)
                {
                    if (g_CustomReader->loadDataFromFile("/sdcard/AuraKernel/自定义物资.txt"))
                        g_CustomDataLoaded = true;
                }
                if (g_CustomDataLoaded && t_屏幕坐标.W > 0)
                {
                    if (对象信息.敌人信息.距离 < 2000.0f)
                    {
                        const CustomItemInfo *info = g_CustomReader->getItemInfo(ClassName);
                        if (info)
                        {
                            char buffer[128];
                            snprintf(buffer, sizeof(buffer), "%s[%d米]", info->displayName.c_str(), (int)对象信息.敌人信息.距离);

                            float fontSize = info->fontSize;
                            if (fontSize <= 0)
                                fontSize = 物资字体大小;

                            ImVec2 textSize = ImGui::CalcTextSize(buffer, 0, fontSize);
                            ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                            ImColor textColor = info->color;
                            ImColor outlineColor = ImColor(0, 0, 0, 255);

                            for (int x = -1; x <= 1; x++)
                            {
                                for (int y = -1; y <= 1; y++)
                                {
                                    if (x != 0 || y != 0)
                                        ImGui::GetForegroundDrawList()->AddText(NULL, fontSize, {textPos.x + x, textPos.y + y}, outlineColor, buffer);
                                }
                            }
                            ImGui::GetForegroundDrawList()->AddText(NULL, fontSize, textPos, textColor, buffer);
                        }
                    }
                }
            }
        }

        if (strstr(ClassName, "BPPawn_Escape_Raven") != 0 or strstr(ClassName, "BPPawn_Escape_UAV_C") != 0)
        {
            continue;
        }
        if (读写.getDword(地址.自身地址) == 0 && 对象信息.敌人信息.距离 <= 5)
        {
            continue;
        }
        bool isboss = 骨骼->isBoss(*ClassName);
        if (读写.getFloat(对象地址.敌人地址 + Offsets::Actor_HighWalkSpeed) == 479.5 || strstr(ClassName, "BPPawn_Escape_") != 0 || isboss)
        {
            D4DVector 屏外预警坐标(r_x, r_y, r_y - r_z, (r_y - r_z) / 2);
            对象信息.敌人信息.队伍 = 读写.getDword(对象地址.敌人地址 + Offsets::Actor_TeamID);
            对象信息.敌人信息.isboot = (对象信息.敌人信息.队伍 == -1) ? 1 : 读写.getDword(对象地址.敌人地址 + Offsets::Actor_bIsAI);
            对象信息.敌人信息.高级人机 = 读写.getDword(对象地址.敌人地址 + 0xb88);
            if (按钮.忽略人机 && 对象信息.敌人信息.isboot == 1)
            {
                continue;
            }
            对象信息.敌人信息.状态 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + Offsets::Actor_PawnState));
            对象信息.敌人信息.雷达 = 计算.rotateCoord(自身数据.准星Y, (自身数据.坐标.X - 对象信息.敌人信息.坐标.X) / 200, (自身数据.坐标.Y - 对象信息.敌人信息.坐标.Y) / 200);
            读写.readv(对象地址.敌人地址 + Offsets::Actor_Velocity, &对象信息.敌人信息.向量, sizeof(对象信息.敌人信息.向量));
            对象信息.敌人信息.Rotator = 读写.getFloat(对象地址.敌人地址 + Offsets::Actor_Rotator);
            对象信息.敌人信息.当前血量 = 读写.getFloat(对象地址.敌人地址 + Offsets::Actor_Health);
            对象信息.敌人信息.最大血量 = 读写.getFloat(对象地址.敌人地址 + Offsets::Actor_HealthMax);
            对象信息.敌人信息.乘坐载具 = 读写.getDword(对象地址.敌人地址 + Offsets::Actor_Vehicle) != 0;
            对象信息.敌人信息.手持 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + Offsets::Actor_CurrentWeapon) + Offsets::Weapon_RepID);
            对象信息.敌人信息.子弹数量 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + Offsets::Actor_CurrentWeapon) + Offsets::Weapon_ClipAmmo);
            对象信息.敌人信息.子弹最大数量 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + Offsets::Actor_CurrentWeapon) + Offsets::Weapon_ClipMaxAmmo);
            对象信息.敌人信息.角色实体 = 读写.getPtr64(对象地址.敌人地址 + 0x39b0);
            对象信息.敌人信息.实体列表地址 = 读写.getPtr64(对象信息.敌人信息.角色实体 + 0x818) + 0x8;
            对象信息.敌人信息.实体数量 = 读写.getDword(对象信息.敌人信息.角色实体 + 0x818 + 0x8);
            long int MeshOffset = 读写.getPtr64(对象地址.敌人地址 + Offsets::Actor_Mesh);
            int Bonecount = 读写.getPtr64(MeshOffset + Offsets::Mesh_BoneArray + Offsets::Mesh_BoneCountOffset);
            D3DVector tempBones[17];
            骨骼->更新骨骼数据(
                MeshOffset + Offsets::Mesh_ComponentToWorld,
                读写.getPtr64(MeshOffset + Offsets::Mesh_BoneArray) + 0x30,
                tempBones,
                Bonecount,
                对象信息.敌人信息.队伍,
                ClassName);
            for (int i = 0; i < 15; i++)
            {
                对象信息.敌人信息.骨骼坐标[i].X = tempBones[i].X;
                对象信息.敌人信息.骨骼坐标[i].Y = tempBones[i].Y;
                对象信息.敌人信息.骨骼坐标[i].Z = tempBones[i].Z;
            }

            char temp[64];
            读写.getUTF8(temp, 读写.getPtr64(对象地址.敌人地址 + Offsets::Actor_PlayerName));
            对象信息.敌人信息.名字 = temp;
            bool 是否掐雷 = false;
            if (按钮.手雷预警)
            {
                if (对象信息.敌人信息.手持 == 602004 or 对象信息.敌人信息.手持 == 9825004)
                {
                    std::string 状态字符串 = std::to_string(对象信息.敌人信息.状态);
                    if (状态字符串.length() == 5)
                    {
                        计时器.addTimer(计算地址, 0);
                        是否掐雷 = true;
                    }
                    else
                    {
                        if (计时器.hasTimer(计算地址))
                        {
                            string 手雷递送 = 手雷类.calculateKey(对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y);
                            if (手雷递送 == "")
                            {
                                Antitankgrenade[计算地址] = Antitankgrenade[计算地址] + 1;
                                if (Antitankgrenade[计算地址] == 15)
                                {
                                    计时器.removeTimer(计算地址);
                                    Antitankgrenade.erase(计算地址);
                                }
                            }
                            else
                            {
                                计时器.renameTimer(计算地址, 手雷递送);
                                手雷类.remove(手雷递送);
                            }
                        }
                    }
                }
                else
                {
                    if (计时器.hasTimer(计算地址))
                    {
                        string 手雷递送 = 手雷类.calculateKey(对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y);
                        if (手雷递送 == "")
                        {
                            Antitankgrenade[计算地址] = Antitankgrenade[计算地址] + 1;
                            if (Antitankgrenade[计算地址] == 15)
                            {
                                计时器.removeTimer(计算地址);
                                Antitankgrenade.erase(计算地址);
                            }
                        }
                        else
                        {
                            计时器.renameTimer(计算地址, 手雷递送);
                            手雷类.remove(手雷递送);
                        }
                    }
                }
            }

            bool 自身是否掐雷 = false;
            if (按钮.瞬爆雷预测)
            {
                if (自身数据.手持id == 602004 or 自身数据.手持id == 9825004)
                {
                    std::string 状态字符串 = std::to_string(自身数据.自身状态);
                    if (状态字符串.length() == 5)
                    {
                        计时器.addTimer("self", 0);
                        自身是否掐雷 = true;
                    }
                    else
                    {
                        if (计时器.hasTimer("self"))
                        {
                            string 手雷递送 = 手雷类.calculateKey(自身数据.坐标.X, 自身数据.坐标.Y);
                            if (手雷递送 == "")
                            {
                                SelfAntitankgrenade["self"] = SelfAntitankgrenade["self"] + 1;
                                if (SelfAntitankgrenade["self"] == 15)
                                {
                                    计时器.removeTimer("self");
                                    SelfAntitankgrenade.erase("self");
                                }
                            }
                            else
                            {
                                计时器.renameTimer("self", 手雷递送);
                                手雷类.remove(手雷递送);
                            }
                        }
                    }
                }
                else
                {
                    if (计时器.hasTimer("self"))
                    {
                        string 手雷递送 = 手雷类.calculateKey(自身数据.坐标.X, 自身数据.坐标.Y);
                        if (手雷递送 == "")
                        {
                            SelfAntitankgrenade["self"] = SelfAntitankgrenade["self"] + 1;
                            if (SelfAntitankgrenade["self"] == 15)
                            {
                                计时器.removeTimer("self");
                                SelfAntitankgrenade.erase("self");
                            }
                        }
                        else
                        {
                            计时器.renameTimer("self", 手雷递送);
                            手雷类.remove(手雷递送);
                        }
                    }
                }
            }
            if (对象信息.敌人信息.队伍 == 自身数据.自身队伍)
            {
                continue;
            }

            bool LineOfSightTo1 = false;
            bool LineOfSightToTab[15] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
            if (按钮.雷达)
            {
                if (对象信息.敌人信息.距离 <= 300)
                {
                    if (对象信息.敌人信息.isboot == 1)
                    {
                        ImGui::GetForegroundDrawList()->AddCircleFilled(
                            {按钮.雷达X + 对象信息.敌人信息.雷达.X, 按钮.雷达Y + 对象信息.敌人信息.雷达.Y},
                            8.0f,
                            ImColor(255, 255, 255, 200));
                    }
                    else
                    {
                        int team = 对象信息.敌人信息.队伍;

                        int adjusted_team = team % 50;
                        adjusted_team = adjusted_team < 0 ? adjusted_team + 50 : adjusted_team;

                        float hue = static_cast<float>(adjusted_team) / 50.0f;

                        float saturation = 1.0f;
                        float value = 1.0f - (static_cast<float>(adjusted_team % 5) * 0.1f);

                        ImColor team_color = ImColor::HSV(hue, saturation, value, 0.8f);

                        ImGui::GetForegroundDrawList()->AddCircleFilled(
                            {按钮.雷达X + 对象信息.敌人信息.雷达.X, 按钮.雷达Y + 对象信息.敌人信息.雷达.Y},
                            15.0f,
                            team_color);

                        ImVec2 text_pos = {
                            按钮.雷达X + 对象信息.敌人信息.雷达.X,
                            按钮.雷达Y + 对象信息.敌人信息.雷达.Y};

                        ImFont *font = ImGui::GetFont();
                        float customFontSize = 13.0f;
                        ImVec2 text_size = font->CalcTextSizeA(customFontSize, FLT_MAX, 0.0f, std::to_string(team).c_str());
                        text_pos.x -= text_size.x / 2.0f;
                        text_pos.y -= text_size.y / 2.0f;

                        ImGui::GetForegroundDrawList()->AddText(
                            font, customFontSize, text_pos,
                            ImColor(0, 0, 0, 200),
                            std::to_string(team).c_str());

                        ImGui::GetForegroundDrawList()->AddText(
                            font, customFontSize, text_pos,
                            ImColor(255, 255, 255, 255),
                            std::to_string(team).c_str());
                    }
                }
            }
            if (strstr(ClassName, "BPPawn_Escape_Raven") != 0 or strstr(ClassName, "BPPawn_Escape_UAV_C") != 0)
            {
                continue;
            }

            if (对象信息.敌人信息.状态 == 1048592 || 对象信息.敌人信息.状态 == 1048576)
                continue;
            if (对象信息.敌人信息.isboot == 1)
            {
                绘制人机++;
            }
            else
            {
                绘制真人++;
            }
            if (按钮.自救倒计时)
            {
                string a = GetHol(对象信息.敌人信息.状态);
                char 计算地址[256];
                sprintf(计算地址, "%lx", 对象信息.敌人信息.状态);
                if (对象信息.敌人信息.状态 == 131104 || 对象信息.敌人信息.状态 == 655360 || 对象信息.敌人信息.状态 == 131105)
                {
                    auto it = 自救Timers.find(计算地址);
                    if (it != 自救Timers.end())
                    {
                        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                                           std::chrono::steady_clock::now() - it->second)
                                           .count();
                        if (elapsed > 6)
                        {
                            自救Timers[计算地址] = std::chrono::steady_clock::now();
                        }
                    }
                    else
                    {
                        自救Timers[计算地址] = std::chrono::steady_clock::now();
                    }

                    auto &timer = 自救Timers[计算地址];
                    int 剩余时间 = 10 - std::chrono::duration_cast<std::chrono::seconds>(
                                            std::chrono::steady_clock::now() - timer)
                                            .count();

                    if (剩余时间 >= 0 && t_屏幕坐标.W >= 0)
                    {
                        绘图.绘制自救(剩余时间);
                    }
                }
                else
                {
                    auto it = 自救Timers.find(计算地址);
                    if (it != 自救Timers.end())
                    {
                        自救Timers.erase(it);
                    }
                }
            }
            更新自救倒计时();
            骨骼数据 t_骨骼数据 = 计算.计算骨骼(自身数据.相机坐标, 对象信息.敌人信息.骨骼坐标, PX, PY);
            std::vector<D2DVector *> 骨骼二维坐标 = t_骨骼数据.获取所有骨骼指针();
            绘图.初始化坐标(t_屏幕坐标, t_骨骼数据);

            if (按钮.被瞄预警)
            {
                auto aimAngle = 计算.rotateCoord(对象信息.敌人信息.骨骼坐标[1], 自身数据.坐标);
                auto aimMZ = FRotator(0, 对象信息.敌人信息.Rotator, 0);
                aimMZ.Clamp();
                float AimX = abs(aimAngle.X - aimMZ.Yaw);

                if (AimX <= 4)
                {
                    被瞄信息[被瞄准对象数量].距离 = 对象信息.敌人信息.距离;
                    被瞄信息[被瞄准对象数量].名字 = 对象信息.敌人信息.名字;
                    被瞄信息[被瞄准对象数量].瞄准武器 = GetHolGunItem(对象信息.敌人信息.手持);
                    被瞄准对象数量++;
                }
            }

            if (按钮.背敌预警)
            {

                float distance = 对象信息.敌人信息.距离;
                int team = 对象信息.敌人信息.队伍;

                auto GetTeamColor = [](int team_id) -> ImColor
                {
                    team_id = team_id % 50;
                    float hue = (team_id * 360.0f) / 50.0f;
                    float sat = 0.8f, val = 0.9f;

                    float r, g, b;
                    ImGui::ColorConvertHSVtoRGB(hue / 360.0f, sat, val, r, g, b);
                    return ImColor(r, g, b, 1.0f);
                };

                int alpha = 255;
                if (distance > 200.0f)
                {
                    alpha = 200;
                }
                else if (distance >= 100.0f)
                {
                    float ratio = (distance - 100.0f) / 100.0f;
                    alpha = 255 - static_cast<int>(ratio * 55);
                }

                ImColor base_color = GetTeamColor(team);
                ImColor alert_color = ImColor(
                    static_cast<int>(base_color.Value.x * 255 + 0.5f),
                    static_cast<int>(base_color.Value.y * 255 + 0.5f),
                    static_cast<int>(base_color.Value.z * 255 + 0.5f),
                    alpha);

                OffScreen(
                    ImGui::GetForegroundDrawList(),
                    屏外预警坐标,
                    camear_r,
                    alert_color,
                    250,
                    distance);
            }

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "PUBG_Cheat", __VA_ARGS__)
#else
#define LOGI(...)        \
    printf(__VA_ARGS__); \
    printf("\n")
#endif

            if (t_屏幕坐标.W >= 0)
            {
                if (按钮.头甲包显示 && 对象信息.敌人信息.实体列表地址 && 对象信息.敌人信息.实体数量 > 0 && 对象信息.敌人信息.实体数量 <= 10)
                {
                    头甲包文本高度 = 0;
                    for (int j = 0; j < 对象信息.敌人信息.实体数量; ++j)
                    {
                        uint64_t 实体地址 = 读写.getPtr64(对象信息.敌人信息.实体列表地址 + j * 0x18);
                        int 头甲包id = 读写.getDword(实体地址 + 0x58 + 0x88 + 0x4);
                        绘图.绘制头甲包(头甲包id);
                    }
                }
                if (按钮.超体职业 && 对象信息.敌人信息.实体列表地址 && 对象信息.敌人信息.实体数量 > 0 && 对象信息.敌人信息.实体数量 <= 50)
                {
                    for (int j = 0; j < 对象信息.敌人信息.实体数量; ++j)
                    {
                        uint64_t 实体地址 = 读写.getPtr64(对象信息.敌人信息.实体列表地址 + j * 0x18);
                        int 头甲包id = 读写.getDword(实体地址 + 0x58 + 0x88 + 0x4);
                        string a = 超体职业(头甲包id);
                        if (a != "NULL")
                            对象信息.敌人信息.名字 += '[' + a + ']';
                    }
                }

                if (按钮.方框)
                    绘图.绘制方框(LineOfSightToTab[0], 对象信息.敌人信息.isboot);

                if (按钮.射线)
                    绘图.绘制射线(LineOfSightToTab[0], t_骨骼数据);
                if (按钮.漏手模式)
                {
                    绘图.漏手模式();
                }
                if (按钮.距离)
                    绘图.绘制距离(对象信息.敌人信息.距离, 对象信息.敌人信息.队伍);
                if (按钮.血量)
                    绘图.绘制血量(对象信息.敌人信息.最大血量, 对象信息.敌人信息.当前血量, 对象信息.敌人信息.isboot);
                if (按钮.名字)
                {
                    float 计时_eroc = 7 - (计时器.getTimerSeconds(计算地址) / 7 * (60 / ImGui::GetIO().Framerate * 0.115));
                    绘图.绘制名字(对象信息.敌人信息.名字, 对象信息.敌人信息.isboot, 计时_eroc, 是否掐雷, ClassName, 对象信息.敌人信息.队伍, Bonecount, false, 对象信息.敌人信息.高级人机);
                }
                if (按钮.瞬爆雷预测)
                {
                    float 计时_eroc = 7 - (计时器.getTimerSeconds("self") / 7 * (60 / ImGui::GetIO().Framerate * 0.115));
                    绘图.瞬爆雷预测(对象信息.敌人信息.距离, 计时_eroc, 自身是否掐雷);
                }
                if (按钮.骨骼)
                {
                    绘图.绘制骨骼(t_骨骼数据, t_屏幕坐标, LineOfSightToTab, 对象信息.敌人信息.距离, Bonecount);
                }
                if (按钮.动作 && !是否掐雷)
                {
                    绘图.绘制动作(对象信息.敌人信息.状态);
                }
                if ((按钮.手持 || 按钮.手持2) && !是否掐雷)
                    绘图.绘制手持(对象信息.敌人信息.手持, 对象信息.敌人信息.状态, 对象信息.敌人信息.子弹数量, 对象信息.敌人信息.子弹最大数量);
            }
        }
    }
    // 阈值：屏幕距离小于 50 像素认为准星对准
    if (minDistDebug < 50.0f && 按钮.Debug)
    {
        DebugAimedClassName = closestClassName;
        bDebugAimedValid = true;
    }
    else
    {
        DebugAimedClassName.clear();
        bDebugAimedValid = false;
    }
    if (自瞄.初始化 && 自瞄.准星射线 && 自瞄.瞄准目标 != -1 && 自瞄函数[自瞄.瞄准目标].准心距离 <= 自瞄.自瞄范围)
    {
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(PX, PY), ImVec2(自瞄函数[自瞄.瞄准目标].对象骨骼.X, 自瞄函数[自瞄.瞄准目标].对象骨骼.Y), ImColor(255, 255, 255, 255), 2.1);
    }
    if (按钮.被瞄预警)
        绘图.绘制瞄准信息();
    if (按钮.人数)
        绘图.绘制人数(绘制人机, 绘制真人);
    自瞄.瞄准总数量 = 自瞄.瞄准对象数量;
}

void 绘制::运行绘制()
{
    更新地址数据();
    更新对象数据();
    if (按钮.全图人数)
    {
        ImGui::SetNextWindowPos(ImVec2(400, 100), ImGuiCond_Always);

        // 推送透明背景样式
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // 背景透明
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));   // 边框透明
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 1.0f, 1.0f));     // 粉色文本 (RGBA: 1,0,1,1)

        ImGui::Begin("游戏信息", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration);

        // 显示存活真实玩家数量
        ImGui::Text("剩余真人: %d", 自身数据.真人数量);

        // 显示人机数量
        ImGui::Text("剩余人机: %d", 自身数据.人机数量);

        // 显示存活队伍数
        ImGui::Text("剩余队伍: %d", 自身数据.队伍数量);

        // 显示全图人数（人机 + 真人）
        ImGui::Text("全图剩余人数: %d", 自身数据.全图数量);

        // 结束窗口
        ImGui::End();

        // 弹出之前推送的样式（注意弹出的顺序要与推送的顺序相反）
        ImGui::PopStyleColor(3); // 弹出3次（文本颜色、边框颜色、背景颜色）
    }
    if (按钮.手雷预警 || 按钮.瞬爆雷预测 || 按钮.自救倒计时)
    {
        计时器.updateTimers();
        计时器.checkAndRemoveTimers();
    }
}

const char *绘制::Level(char *name)
{
    static const std::unordered_map<std::string, const char *> levelMap = {
        {"Lv1", "Lv1"},
        {"Lv2", "Lv2"},
        {"Lv3", "Lv3"},
        {"Lv4", "Lv4"},
        {"Lv5", "Lv5"},
        {"Lv6", "Lv6"},
        {"Lv7", "Lv7"}};

    for (const auto &[key, value] : levelMap)
    {
        if (strstr(name, key.c_str()) != nullptr)
        {
            return value;
        }
    }
    return "未知";
}
const char *绘制::getMaterialName(char *name)
{
    static char dealname[32];
    memset(dealname, '\0', sizeof(dealname));

    // 载具名称映射表 <子串, <显示名称, VehicleID>>
    static const std::vector<std::pair<std::string, std::pair<const char *, int>>> 载具列表 = {
        {"VH_MotorcycleCart", {"三轮摩托车", 43}},
        {"VH_Scooter", {"小绵羊车", 45}},
        {"VH_Motorcycle", {"摩托车", 44}},
        {"VH_Tuk", {"三轮摩托车", 48}},
        {"Buggy", {"蹦蹦", 30}},
        {"Mirado", {"敞篷跑车", 35}},
        {"CoupeRB", {"跑车", 39}},
        {"Dacia", {"轿车", 31}},
        {"PickUp_02_C", {"皮卡", 33}},
        {"Rony", {"皮卡", 38}},
        {"_StationWagon_C", {"旅行车", 40}},
        {"UAZ", {"吉普", 32}},
        {"PG117", {"快艇", 49}},
        {"AquaRail", {"冲锋艇", 50}},
        {"MiniBus", {"面包车", 37}},
        {"VH_BRDM", {"两栖装甲车", 0}},
        {"LadaNiva", {"雪地越野车", 36}},
        {"_4SportCar_C", {"敞篷跑车", 41}},
        {"_Bigfoot_C", {"大脚车", 53}},
        {"_Training_C", {"自行车", 0}},
        {"ATV1_C", {"越野摩托车", 0}},
        {"_Horse", {"马", 52}},
        {"_02_C", {"雪地摩托车", 46}},
        {"Snowmobile_C", {"雪地摩托车", 47}},
        {"_Motorglider_C", {"滑翔机", 51}},
        {"_DesertCar_C", {"大脚越野车", 0}},
        {"_New_C", {"赛车", 0}},
        {"VH_Blanc_C", {"白色SUV", 0}},
        {"VH_Excavator_C", {"挖掘机", 0}},
        {"VH_DumpTruck_C", {"渣土车", 0}},
        {"BP_VH_EV3F4_C", {"履带车", 0}},
        {"VH_SeaPlane_C", {"四人滑向机", 0}},
        {"VH_ATV3_C", {"沙滩车", 0}},
        {"VH_ATV2_C", {"沙滩车", 0}},
        {"VH_ATV1_C", {"沙滩车", 0}},
        {"VH_Picobus_C", {"电动小巴", 0}}};

    for (const auto &载具 : 载具列表)
    {
        if (strstr(name, 载具.first.c_str()) != nullptr)
        {
            strcpy(dealname, 载具.second.first);
            按钮.VehicleID = 载具.second.second;
            return dealname;
        }
    }

    strcpy(dealname, "Error");
    按钮.VehicleID = 0;
    return dealname;
}

string 绘制::getBoxName(int id)
{
    static const std::unordered_map<int, const char *> 医疗物资 = {
        {601006, "医疗箱"}, {601005, "急救包"}, {601004, "绷带"}, {601001, "可乐"}, {601002, "肾上腺素"}, {601003, "止痛药"}};

    static const std::unordered_map<int, const char *> 防具 = {
        {503003, "三级防弹衣"}, {522629, "二级包"}, {502003, "三级头盔"}, {501003, "三级包"}};

    static const std::unordered_map<int, const char *> 子弹 = {
        {302001, "7.62MM"}, {303001, "5.56MM"}, {301001, "9MM"}, {304001, "12口径"}, {305001, "45口径"}, {308001, "信号弹"}, {307001, "箭矢"}};

    static const std::unordered_map<int, const char *> 倍镜列表 = {
        {203001, "红点"}, {203014, "3倍镜"}, {203015, "6倍镜"}, {203005, "8倍镜"}};

    static const std::unordered_map<int, const char *> 配件 = {
        {204014, "子弹袋"}, {204010, "子弹袋"}, {205004, "箭袋"}, {202007, "激光瞄准器"}, {202004, "轻型握把"}, {202005, "半截握把"}, {205001, "UZI枪托"}, {205003, "狙击枪托"}, {205002, "步枪枪托"}, {201003, "狙击枪补偿器"}, {201005, "狙击枪消焰器"}, {201007, "狙击枪消音器"}, {201011, "步枪消音器"}, {201009, "步枪补偿器"}, {201010, "步枪消焰器"}, {201006, "冲锋枪消音器"}, {201004, "冲锋枪消焰器"}, {201002, "冲锋枪消焰器"}, {202006, "拇指握把"}, {202002, "垂直握把"}, {202001, "直角握把"}, {205011, "撞火枪托"}, {204017, "霰弹快速"}, {201012, "鸭嘴枪口"}, {201001, "霰弹收束"}};

    static const std::unordered_map<int, const char *> 扩容配件 = {
        {204009, "狙击枪快扩"}, {204007, "狙击枪扩容"}, {204013, "步枪快扩"}, {204011, "步枪扩容"}, {204006, "冲锋枪快扩"}, {204004, "冲锋枪扩容"}};

    static const std::unordered_map<int, const char *> 投掷物 = {
        {602003, "燃烧瓶"}, {602002, "烟雾弹"}, {602001, "震爆弹"}, {602004, "手榴弹"}};
    static const std::unordered_map<int, const char *> 狙击枪列表 = {
        {103017, "M1加兰德"}, {107001, "十字弩"}, {103003, "AWM"}, {103001, "Kar98K"}, {103002, "M24"}, {103006, "Mini14"}, {103008, "Win94"}, {103009, "SLR"}, {103012, "AMR狙击枪"}, {103013, "M417"}, {103014, "MK20-H"}, {103100, "MK12"}, {103010, "QBU"}, {103004, "SKS"}, {103007, "MK14"}, {103015, "M200"}, {103011, "莫辛狙击枪"}};

    static const std::unordered_map<int, const char *> 步枪列表 = {
        {101016, "ARX突击步枪"}, {101012, "蜜獾"}, {101008, "M762"}, {101011, "VC-VAL突击步枪"}, {101010, "G36C步枪"}, {101013, "Famas突击步枪"}, {105002, "DP28"}, {101005, "狗砸"}, {101001, "AKM"}, {101007, "QBZ"}, {101009, "MK47"}, {101006, "AUG"}, {101002, "M16A4"}, {101003, "SCAR"}, {101014, "ACE-32"}, {101004, "M416"}, {105001, "M249"}, {105012, "PKM"}, {105010, "MG3"}};

    static const std::unordered_map<int, const char *> 冲锋枪列表 = {
        {102001, "UZI"}, {102004, "汤姆逊冲锋枪"}, {102003, "维克托"}, {102002, "UMP9"}, {102008, "AKS-74U"}, {102009, "JS9冲锋枪"}, {102005, "野牛冲锋枪"}, {102007, "MP5K"}, {102105, "P90冲锋枪"}};

    static const std::unordered_map<int, const char *> 散弹枪列表 = {
        {106003, "R1895"}, {106006, "短管散弹枪"}, {104003, "S12K"}, {104002, "S1897"}, {104001, "双管猎枪"}, {104004, "DBS"}, {104100, "SPAS-12"}, {104005, "AA12-G"}};
    if (按钮.狙击枪械)
    {
        if (auto it = 狙击枪列表.find(id); it != 狙击枪列表.end())
        {
            return it->second;
        }
    }
    if (按钮.显示步枪)
    {
        if (auto it = 步枪列表.find(id); it != 步枪列表.end())
        {
            return it->second;
        }
    }
    if (按钮.显示止痛药 && id == 601003)
    {
        return "止痛药";
    }
    if (按钮.显示可乐 && id == 601001)
    {
        return "可乐";
    }
    if (按钮.显示肾上腺素 && id == 601002)
    {
        return "肾上腺素";
    }
    if (按钮.冲锋枪械)
    {
        if (auto it = 冲锋枪列表.find(id); it != 冲锋枪列表.end())
        {
            return it->second;
        }
    }
    if (按钮.散弹枪械)
    {
        if (auto it = 散弹枪列表.find(id); it != 散弹枪列表.end())
        {
            return it->second;
        }
    }
    if (按钮.显示扩容)
    {
        if (auto it = 扩容配件.find(id); it != 扩容配件.end())
        {
            return it->second;
        }
    }
    if (按钮.显示配件)
    {
        if (auto it = 配件.find(id); it != 配件.end())
        {
            return it->second;
        }
    }
    if (按钮.投掷物品)
    {
        if (auto it = 投掷物.find(id); it != 投掷物.end())
        {
            return it->second;
        }
    }
    if (按钮.显示倍镜)
    {
        if (auto it = 倍镜列表.find(id); it != 倍镜列表.end())
        {
            return it->second;
        }
    }
    if (按钮.显示子弹)
    {
        if (auto it = 子弹.find(id); it != 子弹.end())
        {
            return it->second;
        }
    }
    if (按钮.显示药品)
    {
        if (auto it = 医疗物资.find(id); it != 医疗物资.end())
        {
            return it->second;
        }
    }
    if (按钮.显示防具)
    {
        if (auto it = 防具.find(id); it != 防具.end())
        {
            return it->second;
        }
    }

    return "Error";
}

string 绘制::getBoxName1(int id)
{
    const auto &map = GetBox1NameMap();
    auto it = map.find(id);
    if (it != map.end())
        return it->second;
    return "NULL";
}