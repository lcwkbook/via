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
#include "HookRW.h" // 添加HookRW头文件

// 添加ptrace过检测功能
#include <sys/ptrace.h>
#include "json.hpp"
using json = nlohmann::json;
extern float statusBarAlpha;
int decrypt_zero_x()
{
    return 0x8; // 默认偏移值，根据你的游戏修改
}

int decrypt_zero_y()
{
    return 0x10; // 默认偏移值，根据你的游戏修改
}

extern 绘制 绘制;
std::map<std::string, std::chrono::steady_clock::time_point> 自救Timers;
bool 线程开启状态 = false;
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
        uintptr_t 临时地址1 = 读写.getPtr64(地址.libue4 + 0x1495C2A0);
        if (临时地址1 == 0)
        {
            printf("错误: 第一步读取失败\n");
            return;
        }

        uintptr_t 临时地址2 = 读写.getPtr64(临时地址1 + 0x30);
        if (临时地址2 == 0)
        {
            printf("错误: 第二步读取失败\n");
            return;
        }

        this->特征地址 = 读写.getPtr64(临时地址2 + 0x5D0);
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

// 修改 ShelterJudgment 函数
int ShelterJudgment(bool LineOfSightToTab[15], int enemyState = 16)
{
    // 如果选择智能瞄准部位，根据敌人状态动态选择
    if (绘制.自瞄.瞄准部位 == 3)
    { // 假设3是智能选项
        int smartBone = GetSmartAimBone(enemyState);
        if (!LineOfSightToTab[smartBone])
        {
            return smartBone;
        }
    }

    // 优先检查设置的瞄准部位
    if (!LineOfSightToTab[绘制.自瞄.瞄准部位])
    {
        return 绘制.自瞄.瞄准部位;
    }

    // 遍历其他骨骼部位
    for (int i = 0; i < 15; i++)
    {
        if (i == 绘制.自瞄.瞄准部位)
            continue; // 跳过已检查的瞄准部位
        if (!LineOfSightToTab[i])
        {
            return i;
        }
    }
    return 999;
}

float calculateDistance(float x1, float y1, float x2, float y2)
{
    return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
std::unordered_map<int, 武器触发条件> 绘制::武器触发配置;
std::unordered_map<int, 武器参数> 绘制::武器参数配置;
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
    // 保存自瞄和武器配置
    nlohmann::json aim_config;
    json config;
    // ... 其他配置项 ...
    config["status_bar_alpha"] = statusBarAlpha; // 现在 json 和 statusBarAlpha 均已声明

    std::ofstream file("/sdcard/AuraKernel/config.json"); // 示例路径
    if (file.is_open())
    {
        file << config.dump(4);
    }
    // 自瞄配置
    aim_config["自瞄"] = {
        {"当前自瞄范围", 自瞄.当前自瞄范围},
        {"触摸范围", 自瞄.触摸范围},
        {"自瞄速度", 自瞄.自瞄速度},
        {"腰射自瞄速度", 自瞄.腰射自瞄速度},
        {"开镜自瞄速度", 自瞄.开镜自瞄速度},
        {"压枪力度", 自瞄.压枪力度},
        {"预判力度", 自瞄.预判力度},
        {"趴下位置调节", 自瞄.趴下位置调节},
        {"触摸采样率", 自瞄.触摸采样率},
        {"喷子距离限制", 自瞄.喷子距离限制},
        {"掉血自瞄数率", 自瞄.掉血自瞄数率},
        {"腰射距离限制", 自瞄.腰射距离限制},
        {"自瞄距离限制", 自瞄.自瞄距离限制},
        {"触摸范围X", 自瞄.触摸范围X},
        {"触摸范围Y", 自瞄.触摸范围Y},
        {"自瞄条件", 自瞄.自瞄条件},
        {"充电口方向", 自瞄.充电口方向},
        {"瞄准部位", 自瞄.瞄准部位},
        {"喷子自瞄条件", 自瞄.喷子自瞄条件},
        {"喷子自瞄范围", 自瞄.喷子自瞄范围},
        {"连点速度", 自瞄.连点速度},
        {"定速巡航速率", 自瞄.定速巡航速率},
        {"三倍压枪", 自瞄.三倍压枪},
        {"四倍压枪", 自瞄.四倍压枪},
        {"六倍压枪", 自瞄.六倍压枪},
        {"开镜自瞄范围", 自瞄.开镜自瞄范围},
        {"连点位置", {{"startX", 自瞄.连点位置.startX}, {"startY", 自瞄.连点位置.startY}, {"endX", 自瞄.连点位置.endX}, {"endY", 自瞄.连点位置.endY}}},
        {"初始化", 自瞄.初始化},
        {"隐藏自瞄圈", 自瞄.隐藏自瞄圈},
        {"随机触摸点", 自瞄.随机触摸点},
        {"持续锁定", 自瞄.持续锁定},
        {"触摸位置", 自瞄.触摸位置},
        {"动态自瞄", 自瞄.动态自瞄},
        {"准星射线", 自瞄.准星射线},
        {"倒地不瞄", 自瞄.倒地不瞄},
        {"掉血自瞄", 自瞄.掉血自瞄},
        {"自瞄控件", 自瞄.自瞄控件},
        {"喷子自瞄", 自瞄.喷子自瞄},
        {"狙击自瞄", 自瞄.狙击自瞄},
        {"人机不瞄", 自瞄.人机不瞄},
        {"框内自瞄", 自瞄.框内自瞄},
        {"软锁自瞄", 自瞄.软锁自瞄},
        {"开启单发狙连点", 自瞄.开启单发狙连点},
        {"开启喷子连点", 自瞄.开启喷子连点},
        {"扫车不瞄", 自瞄.扫车不瞄},
        {"定速巡航", 自瞄.定速巡航},
        {"适应系数", 自瞄.适应系数},
        {"自动适应灵敏度", 自瞄.自动适应灵敏度},
    };

    // 武器配置
    aim_config["武器触发配置"] = nlohmann::json::object();
    for (const auto &pair : 武器触发配置)
    {
        aim_config["武器触发配置"][std::to_string(pair.first)] = {
            {"独立调节", pair.second.独立调节},
            {"独立压枪", pair.second.独立压枪},
            {"独立预判", pair.second.独立预判},
            {"独立距离限制", pair.second.独立距离限制},
            {"启用自瞄", pair.second.启用自瞄},
            {"腰射距离限制", pair.second.腰射距离限制},
            {"自瞄距离限制", pair.second.自瞄距离限制},
            {"自瞄条件", pair.second.自瞄条件}};
    }

    aim_config["武器参数配置"] = nlohmann::json::object();
    for (const auto &pair : 武器参数配置)
    {
        aim_config["武器参数配置"][std::to_string(pair.first)] = {
            {"压枪力度", pair.second.压枪力度},
            {"预判力度", pair.second.预判力度},
            {"自瞄速度", pair.second.自瞄速度}};
    }

    // 保存自瞄配置到文件
    // std::ofstream aim_file("/sdcard/AuraKernel/Aura自瞄配置.json");
    // aim_file << aim_config.dump(4);
    // aim_file.close();

    // 保存基础配置
    nlohmann::json base_config;

    // 按钮配置
    base_config["按钮"] = {
        {"雷达X", 按钮.雷达X},
        {"雷达Y", 按钮.雷达Y},
        {"方框粗细", 按钮.方框粗细},
        {"射线粗细", 按钮.射线粗细},
        {"骨骼粗细", 按钮.骨骼粗细},
        {"速度值", 按钮.速度值},
        {"第三人称", 按钮.第三人称},
        {"当前帧率", 按钮.当前帧率},
        {"帧率选项", 按钮.帧率选项},
        {"血条绘图", 按钮.血条绘图},
        {"动作", 按钮.动作},
        {"烟雾倒计时", 按钮.烟雾倒计时},
        {"绘制", 按钮.绘制},
        {"忽略人机", 按钮.忽略人机},
        {"手持2", 按钮.手持2},
        {"被瞄预警", 按钮.被瞄预警},
        {"人物加速", 按钮.人物加速},
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
        {"精英勋章", 按钮.精英勋章},
        {"绘制最大距离", 按钮.绘制最大距离},
    };

    // 连点配置
    base_config["连点"] = {
        {"触摸范围X", 连点.触摸范围X},
        {"触摸范围Y", 连点.触摸范围Y},
        {"连点速度", 连点.连点速度},
        {"监听边长", 连点.监听边长},
        {"监听位置X", 连点.监听位置X},
        {"监听位置Y", 连点.监听位置Y},
        {"启用连点", 连点.启用连点},
        {"连点位置", 连点.连点位置},
        {"跟随自瞄连点", 连点.跟随自瞄连点},
        {"监听位置", 连点.监听位置}};

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
    std::ifstream file("/sdcard/AuraKernel/config.json");
    if (!file.is_open())
        return;

    json config = json::parse(file);
    if (config.contains("status_bar_alpha"))
    {
        statusBarAlpha = config["status_bar_alpha"].get<float>();
    }
    // 读取自瞄配置文件
    // std::ifstream aim_file("/sdcard/AuraKernel/Aura自瞄配置.json");
    // if (aim_file.is_open())
    // {
    //     nlohmann::json aim_config = nlohmann::json::parse(aim_file);

    //     // 读取自瞄配置
    //     if (aim_config.contains("自瞄"))
    //     {
    //         const auto &aim = aim_config["自瞄"];
    //         自瞄.当前自瞄范围 = aim.value("当前自瞄范围", 自瞄.当前自瞄范围);
    //         自瞄.触摸范围 = aim.value("触摸范围", 自瞄.触摸范围);
    //         自瞄.自瞄速度 = aim.value("自瞄速度", 自瞄.自瞄速度);
    //         自瞄.腰射自瞄速度 = aim.value("腰射自瞄速度", 自瞄.腰射自瞄速度);
    //         自瞄.开镜自瞄速度 = aim.value("开镜自瞄速度", 自瞄.开镜自瞄速度);
    //         自瞄.压枪力度 = aim.value("压枪力度", 自瞄.压枪力度);
    //         自瞄.预判力度 = aim.value("预判力度", 自瞄.预判力度);
    //         自瞄.趴下位置调节 = aim.value("趴下位置调节", 自瞄.趴下位置调节);
    //         自瞄.触摸采样率 = aim.value("触摸采样率", 自瞄.触摸采样率);
    //         自瞄.喷子距离限制 = aim.value("喷子距离限制", 自瞄.喷子距离限制);
    //         自瞄.掉血自瞄数率 = aim.value("掉血自瞄数率", 自瞄.掉血自瞄数率);
    //         自瞄.腰射距离限制 = aim.value("腰射距离限制", 自瞄.腰射距离限制);
    //         自瞄.自瞄距离限制 = aim.value("自瞄距离限制", 自瞄.自瞄距离限制);
    //         自瞄.触摸范围X = aim.value("触摸范围X", 自瞄.触摸范围X);
    //         自瞄.触摸范围Y = aim.value("触摸范围Y", 自瞄.触摸范围Y);
    //         自瞄.自瞄条件 = aim.value("自瞄条件", 自瞄.自瞄条件);
    //         自瞄.充电口方向 = aim.value("充电口方向", 自瞄.充电口方向);
    //         自瞄.瞄准部位 = aim.value("瞄准部位", 自瞄.瞄准部位);
    //         自瞄.喷子自瞄条件 = aim.value("喷子自瞄条件", 自瞄.喷子自瞄条件);
    //         自瞄.喷子自瞄范围 = aim.value("喷子自瞄范围", 自瞄.喷子自瞄范围);
    //         自瞄.连点速度 = aim.value("连点速度", 自瞄.连点速度);
    //         自瞄.定速巡航速率 = aim.value("定速巡航速率", 自瞄.定速巡航速率);
    //         自瞄.三倍压枪 = aim.value("三倍压枪", 自瞄.三倍压枪);
    //         自瞄.四倍压枪 = aim.value("四倍压枪", 自瞄.四倍压枪);
    //         自瞄.六倍压枪 = aim.value("六倍压枪", 自瞄.六倍压枪);
    //         自瞄.开镜自瞄范围 = aim.value("开镜自瞄范围", 自瞄.开镜自瞄范围);
    //         if (aim.contains("连点位置"))
    //         {
    //             const auto &pos = aim["连点位置"];
    //             自瞄.连点位置.startX = pos.value("startX", 自瞄.连点位置.startX);
    //             自瞄.连点位置.startY = pos.value("startY", 自瞄.连点位置.startY);
    //             自瞄.连点位置.endX = pos.value("endX", 自瞄.连点位置.endX);
    //             自瞄.连点位置.endY = pos.value("endY", 自瞄.连点位置.endY);
    //         }
    //         自瞄.初始化 = aim.value("初始化", 自瞄.初始化);
    //         自瞄.隐藏自瞄圈 = aim.value("隐藏自瞄圈", 自瞄.隐藏自瞄圈);
    //         自瞄.随机触摸点 = aim.value("随机触摸点", 自瞄.随机触摸点);
    //         自瞄.持续锁定 = aim.value("持续锁定", 自瞄.持续锁定);
    //         自瞄.触摸位置 = aim.value("触摸位置", 自瞄.触摸位置);
    //         自瞄.动态自瞄 = aim.value("动态自瞄", 自瞄.动态自瞄);
    //         自瞄.准星射线 = aim.value("准星射线", 自瞄.准星射线);
    //         自瞄.倒地不瞄 = aim.value("倒地不瞄", 自瞄.倒地不瞄);
    //         自瞄.掉血自瞄 = aim.value("掉血自瞄", 自瞄.掉血自瞄);
    //         自瞄.自瞄控件 = aim.value("自瞄控件", 自瞄.自瞄控件);
    //         自瞄.喷子自瞄 = aim.value("喷子自瞄", 自瞄.喷子自瞄);
    //         自瞄.狙击自瞄 = aim.value("狙击自瞄", 自瞄.狙击自瞄);
    //         自瞄.人机不瞄 = aim.value("人机不瞄", 自瞄.人机不瞄);
    //         自瞄.框内自瞄 = aim.value("框内自瞄", 自瞄.框内自瞄);
    //         自瞄.软锁自瞄 = aim.value("软锁自瞄", 自瞄.软锁自瞄);
    //         自瞄.开启单发狙连点 = aim.value("开启单发狙连点", 自瞄.开启单发狙连点);
    //         自瞄.开启喷子连点 = aim.value("开启喷子连点", 自瞄.开启喷子连点);
    //         自瞄.扫车不瞄 = aim.value("扫车不瞄", 自瞄.扫车不瞄);
    //         自瞄.定速巡航 = aim.value("定速巡航", 自瞄.定速巡航);
    //         自瞄.适应系数 = aim.value("适应系数", 自瞄.适应系数);
    //         自瞄.自动适应灵敏度 = aim.value("自动适应灵敏度", 自瞄.自动适应灵敏度);
    //     }

    //     // 读取武器触发配置
    //     if (aim_config.contains("武器触发配置"))
    //     {
    //         const auto &weapon_trigger = aim_config["武器触发配置"];
    //         for (const auto &[key, value] : weapon_trigger.items())
    //         {
    //             int weapon_id = std::stoi(key);
    //             武器触发条件 condition;
    //             condition.独立调节 = value.value("独立调节", condition.独立调节);
    //             condition.独立压枪 = value.value("独立压枪", condition.独立压枪);
    //             condition.独立预判 = value.value("独立预判", condition.独立预判);
    //             condition.独立距离限制 = value.value("独立距离限制", condition.独立距离限制);
    //             condition.启用自瞄 = value.value("启用自瞄", true);
    //             condition.腰射距离限制 = value.value("腰射距离限制", 50.0f);
    //             condition.自瞄距离限制 = value.value("自瞄距离限制", 100.0f);
    //             condition.自瞄条件 = value.value("自瞄条件", 0);
    //             武器触发配置[weapon_id] = condition;
    //         }
    //     }

    //     // 读取武器参数配置
    //     if (aim_config.contains("武器参数配置"))
    //     {
    //         const auto &weapon_params = aim_config["武器参数配置"];
    //         for (const auto &[key, value] : weapon_params.items())
    //         {
    //             int weapon_id = std::stoi(key);
    //             武器参数 params;
    //             params.压枪力度 = value.value("压枪力度", 2.15f);
    //             params.预判力度 = value.value("预判力度", 1.45f);
    //             params.自瞄速度 = value.value("自瞄速度", 35.f);
    //             武器参数配置[weapon_id] = params;
    //         }
    //     }
    // }

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
            按钮.速度值 = button.value("速度值", 按钮.速度值);
            按钮.第三人称 = button.value("第三人称", 按钮.第三人称);
            按钮.当前帧率 = button.value("当前帧率", 按钮.当前帧率);
            按钮.帧率选项 = button.value("帧率选项", 按钮.帧率选项);
            按钮.血条绘图 = button.value("血条绘图", 按钮.血条绘图);
            按钮.动作 = button.value("动作", 按钮.动作);
            按钮.烟雾倒计时 = button.value("烟雾倒计时", 按钮.烟雾倒计时);
            按钮.绘制 = button.value("绘制", 按钮.绘制);
            按钮.忽略人机 = button.value("忽略人机", 按钮.忽略人机);
            按钮.手持2 = button.value("手持2", 按钮.手持2);
            按钮.被瞄预警 = button.value("被瞄预警", 按钮.被瞄预警);
            按钮.人物加速 = button.value("人物加速", 按钮.人物加速);
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
            按钮.精英勋章 = button.value("精英勋章", 按钮.精英勋章);
            按钮.超体职业 = button.value("超体职业", 按钮.超体职业);
            按钮.绘制最大距离 = button.value("绘制最大距离", 按钮.绘制最大距离);
        }

        // 读取连点配置
        if (base_config.contains("连点"))
        {
            const auto &click = base_config["连点"];
            连点.触摸范围X = click.value("触摸范围X", 连点.触摸范围X);
            连点.触摸范围Y = click.value("触摸范围Y", 连点.触摸范围Y);
            连点.连点速度 = click.value("连点速度", 连点.连点速度);
            连点.监听边长 = click.value("监听边长", 连点.监听边长);
            连点.监听位置X = click.value("监听位置X", 连点.监听位置X);
            连点.监听位置Y = click.value("监听位置Y", 连点.监听位置Y);
            连点.启用连点 = click.value("启用连点", 连点.启用连点);
            连点.连点位置 = click.value("连点位置", 连点.连点位置);
            连点.跟随自瞄连点 = click.value("跟随自瞄连点", 连点.跟随自瞄连点);
            连点.监听位置 = click.value("监听位置", 连点.监听位置);
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
static Gyro *gyro = nullptr;
bool 陀螺仪开启状态 = false;
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

    // 陀螺仪初始化
    if (!陀螺仪开启状态)
    {
        gyro = new Gyro(); // 这里会自动检测并选择驱动模式
        陀螺仪开启状态 = true;
    }

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
}

int 绘制::findminat()
{
    float DistanceMin = 450.0f;
    float min = 自瞄.自瞄范围;
    int minAt = 999;
    bool foundNewTarget = false;

    // 持续锁定逻辑：只在开火状态下生效
    if (自瞄.持续锁定 && 自瞄.持续自瞄中 && 自瞄.瞄准目标 != -1 && 自瞄.瞄准目标 < 自瞄.瞄准总数量 && 自身数据.开火 == 1)
    {
        // 检查当前锁定目标是否仍然有效
        if (自瞄函数[自瞄.瞄准目标].准心距离 <= 自瞄.自瞄范围 &&
            自瞄函数[自瞄.瞄准目标].距离 <= 自瞄.自瞄距离限制 &&
            自瞄函数[自瞄.瞄准目标].掩体部位 != 999)
        {
            return 自瞄.瞄准目标;
        }
        else
        {
            // 当前目标失效，重置持续锁定状态
            自瞄.持续自瞄中 = false;
            strncpy(AimName, "", sizeof(AimName) - 1);
        }
    }

    for (int i = 0; i < 自瞄.瞄准总数量; i++)
    {
        switch ((int)自瞄.瞄准优先)
        {
        case 0:
            if (自瞄.持续自瞄中 && 自身数据.开火 == 1) // 只在开火状态下持续锁定
            {
                if (strcmp(自瞄函数[i].名字.c_str(), AimName) == 0 && 自瞄函数[i].准心距离 <= min)
                {
                    minAt = i;
                    min = 自瞄函数[i].准心距离;
                }
            }
            else
            {
                if (自瞄函数[i].准心距离 < min && 自瞄函数[i].准心距离 != 0)
                {
                    min = 自瞄函数[i].准心距离;
                    foundNewTarget = true;
                    minAt = i;
                }
            }
            break;
        case 1:
            if (自瞄.持续自瞄中 && 自身数据.开火 == 1) // 只在开火状态下持续锁定
            {
                if (strcmp(自瞄函数[i].名字.c_str(), AimName) == 0 &&
                    自瞄函数[i].准心距离 <= 自瞄.自瞄范围 &&
                    自瞄函数[i].距离 < DistanceMin)
                {
                    DistanceMin = 自瞄函数[i].距离;
                    minAt = i;
                }
            }
            else
            {
                if (自瞄函数[i].准心距离 < 自瞄.自瞄范围)
                {
                    if (自瞄函数[i].距离 < DistanceMin)
                    {
                        DistanceMin = 自瞄函数[i].距离;
                        foundNewTarget = true;
                        minAt = i;
                    }
                }
            }
            break;
        }
    }

    if (foundNewTarget && 自瞄.持续锁定 && 自身数据.开火 == 1) // 只在开火状态下设置持续锁定
    {
        strncpy(AimName, 自瞄函数[minAt].名字.c_str(), sizeof(AimName) - 1);
        AimName[sizeof(AimName) - 1] = '\0';
    }
    if (minAt == 999)
    {
        自瞄.瞄准目标 = -1;
        自瞄.持续自瞄中 = false;
        return -1;
    }
    自瞄.瞄准目标 = minAt;
    if (自瞄.持续锁定 && 自身数据.开火 == 1) // 只在开火状态下设置持续自瞄标志
    {
        自瞄.持续自瞄中 = true;
    }
    if (自瞄函数[自瞄.瞄准目标].掩体部位 == 999)
    {
        自瞄.瞄准目标 = -1;
        return -1;
    }
    return minAt;
}

void 绘制::GetTouch()
{
    std::thread *触摸位置线程 = new std::thread([&]
                                                {
    for(;;)
    {
        usleep(1000000 / 120);
        ImGuiIO& iooi = ImGui::GetIO();
        
        // 新增连点位置调整逻辑
        if (连点.连点位置 && iooi.MouseDown[0] && iooi.MousePos.x <= 连点.触摸范围X + 连点.触摸范围 && iooi.MousePos.y <= displayInfo.height - 连点.触摸范围Y + 连点.触摸范围 && iooi.MousePos.x >= 连点.触摸范围X - 连点.触摸范围 && iooi.MousePos.y >= displayInfo.height - 连点.触摸范围Y - 连点.触摸范围)
        {
          usleep(30000);
          if (连点.连点位置 && iooi.MouseDown[0] && iooi.MousePos.x <= 连点.触摸范围X + 连点.触摸范围 && iooi.MousePos.y <= displayInfo.height - 连点.触摸范围Y + 连点.触摸范围 && iooi.MousePos.x >= 连点.触摸范围X - 连点.触摸范围 && iooi.MousePos.y >= displayInfo.height - 连点.触摸范围Y - 连点.触摸范围)
          {
            while (连点.连点位置 && iooi.MouseDown[0] && iooi.MousePos.x <= 连点.触摸范围X + 连点.触摸范围 && iooi.MousePos.y <= displayInfo.height - 连点.触摸范围Y + 连点.触摸范围 && iooi.MousePos.x >= 连点.触摸范围X - 连点.触摸范围 && iooi.MousePos.y >= displayInfo.height - 连点.触摸范围Y - 连点.触摸范围)
            {
              连点.触摸范围X = iooi.MousePos.x;
              连点.触摸范围Y = displayInfo.height - iooi.MousePos.y;
              usleep(500);
            }
          }
        }


      if (自瞄.触摸位置 && iooi.MouseDown[0] && iooi.MousePos.x <= 自瞄.触摸范围X + 自瞄.触摸范围 && iooi.MousePos.y <= displayInfo.height - 自瞄.触摸范围Y + 自瞄.触摸范围 && iooi.MousePos.x >= 自瞄.触摸范围X - 自瞄.触摸范围 && iooi.MousePos.y >= displayInfo.height - 自瞄.触摸范围Y - 自瞄.触摸范围)
      {
        usleep(30000);
        if (自瞄.触摸位置 && iooi.MouseDown[0] && iooi.MousePos.x <= 自瞄.触摸范围X + 自瞄.触摸范围 && iooi.MousePos.y <= displayInfo.height - 自瞄.触摸范围Y + 自瞄.触摸范围 && iooi.MousePos.x >= 自瞄.触摸范围X - 自瞄.触摸范围 && iooi.MousePos.y >= displayInfo.height - 自瞄.触摸范围Y - 自瞄.触摸范围)
        {
          while (自瞄.触摸位置 && iooi.MouseDown[0] && iooi.MousePos.x <= 自瞄.触摸范围X + 自瞄.触摸范围 && iooi.MousePos.y <= displayInfo.height - 自瞄.触摸范围Y + 自瞄.触摸范围 && iooi.MousePos.x >= 自瞄.触摸范围X - 自瞄.触摸范围 && iooi.MousePos.y >= displayInfo.height - 自瞄.触摸范围Y - 自瞄.触摸范围)
          {
            自瞄.触摸范围X = iooi.MousePos.x;
            自瞄.触摸范围Y = displayInfo.height - iooi.MousePos.y;
            usleep(500);
          }
        }
      }
    } });
    触摸位置线程->detach();
}

bool 绘制::自瞄触发(float 距离)
{
    // 如果开启持续锁定且正在持续自瞄中，并且正在开火，直接返回true
    if (自瞄.持续锁定 && 自瞄.持续自瞄中 && 自身数据.开火 == 1)
    {
        return true;
    }

    // 原有的自瞄触发逻辑...
    if (自瞄.喷子自瞄)
    {
        if (自身数据.手持 == 104003 or 自身数据.手持 == 104005 or 自身数据.手持 == 104100 or 自身数据.手持 == 104004)
        {
            if (距离 < 自瞄.喷子距离限制)
            {
                if (自瞄.喷子自瞄条件 == 0 or (自瞄.喷子自瞄条件 == 1 && 自身数据.开镜 != 0) or (自瞄.喷子自瞄条件 == 2 && 自身数据.射出子弹数量 != 0))
                {
                    if (自瞄.开启喷子连点)
                    {
                        clickRegion(true, 自瞄.连点位置);
                    }
                    return true;
                }
            }
        }
    }

    auto &配置 = 武器触发配置[自身数据.手持];
    if (配置.独立距离限制)
    {
        if (自瞄函数[自瞄.瞄准目标].距离 > 配置.自瞄距离限制)
        {
            return false;
        }
        if (自瞄函数[自瞄.瞄准目标].距离 > 配置.腰射距离限制 && 自身数据.Fov > 75)
        {
            return false;
        }
    }
    else
    {
        if (自瞄函数[自瞄.瞄准目标].距离 > 自瞄.自瞄距离限制)
        {
            return false;
        }
        if (自瞄函数[自瞄.瞄准目标].距离 > 自瞄.腰射距离限制 && 自身数据.Fov > 75)
        {
            return false;
        }
    }
    if (自瞄.狙击自瞄)
    {
        if (自身数据.手持 == 103011 or 自身数据.手持 == 103001 or 自身数据.手持 == 103003 or 自身数据.手持 == 103015 or 自身数据.手持 == 103012 or 自身数据.手持 == 103002)
        {
            if (自身数据.Fov < 75)
            {
                return true;
            }
        }
    }

    int 触发条件 = 配置.独立调节 ? 配置.自瞄条件 : 自瞄.自瞄条件;
    switch (触发条件)
    {
    case 0:
        if (自身数据.开火 != 1)
        {
            return false;
        }
        break;
    case 1:
        if (自身数据.开镜 != 1)
        {
            return false;
        }
        break;
    case 2:
        if (自身数据.开火 == 0 && 自身数据.开镜 != 1)
        {
            return false;
        }
        break;
    }
    return true;
}

float 绘制::陀螺仪灵敏度补偿(float Fov)
{
    int 补偿系数 = 自瞄.适应系数;
    float 当前灵敏度 = 0;

    if (自瞄.自适应腰射灵敏度 && Fov > 75)
    {
        // 启用自适应腰射灵敏度，且处于腰射状态
        // 根据FOV计算腰射灵敏度
        float 腰射系数 = 1.0f + (Fov - 75) / 15.0f * 0.5f; // FOV越大，灵敏度越高
        当前灵敏度 = 自身数据.陀螺仪灵敏度第一人称 * 腰射系数;
        return 补偿系数 / 当前灵敏度;
    }

    if (Fov > 80)
    {
        当前灵敏度 = 自身数据.陀螺仪灵敏度第一人称;
        return 补偿系数 / 当前灵敏度;
    }
    else if (Fov > 75 && Fov <= 80)
    {
        当前灵敏度 = 自身数据.陀螺仪灵敏度第三人称;
        return 补偿系数 / 当前灵敏度;
    }
    int 倍镜 = 90 / Fov;
    const std::unordered_map<int, float> scopeMap = {
        {0, 自身数据.陀螺仪灵敏度第三人称},
        {1, 自身数据.陀螺仪灵敏度红点}, // 红点
        {2, 自身数据.陀螺仪灵敏度二倍},
        {3, 自身数据.陀螺仪灵敏度三倍},
        {4, 自身数据.陀螺仪灵敏度四倍},
        {5, 自身数据.陀螺仪灵敏度四倍},
        {6, 自身数据.陀螺仪灵敏度六倍},
        {7, 自身数据.陀螺仪灵敏度六倍},
        {8, 自身数据.陀螺仪灵敏度八倍}};
    auto it = scopeMap.find(倍镜);
    当前灵敏度 = it->second;
    return 补偿系数 / 当前灵敏度;
}

void 绘制::自瞄主线程()
{
    std::thread *自瞄线程 = new std::thread([&]
                                            {
        bool isDown = false;    
        float halfSize = 自瞄.触摸范围 / 2;
        double RandomnumberX = 自瞄.触摸范围Y, RandomnumberY = 自瞄.触摸范围X;
        double tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
        
        if (自瞄.随机触摸点) {
            RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
            RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
            tx = RandomnumberX, ty = RandomnumberY;    
        }
        
        double ScreenX, ScreenY;
        if(displayInfo.orientation == 1 || displayInfo.orientation == 3) {
            ScreenX = displayInfo.height; 
            ScreenY = displayInfo.width;
        } else {
            ScreenX = displayInfo.width; 
            ScreenY = displayInfo.height;
        }
        
        double ScrXH = ScreenX / 2.0f;
        double ScrYH = ScreenY / 2.0f;
        static float TargetX = 0;
        static float TargetY = 0;
        FVector_class obj;
        float NowCoor[3];
        float zm_x, zm_y;
        
        int 目标血量 = 100;
        string 目标名字;
        bool 自瞄测试 = false;
        int 数率 = 0;
        
        timer AimFPS;
        AimFPS.SetFps(120);
        AimFPS.AotuFPS_init();
        AimFPS.setAffinity();
        
        // 新增：平滑过渡变量
        float lastTargetX = 0, lastTargetY = 0;
        const float smoothFactor = 0.3f; // 平滑系数
        
        // 新增：当前自瞄速度变量
        float 当前自瞄速度 = 自瞄.自瞄速度;
        
        // 新增：陀螺仪状态跟踪
        bool 上次陀螺仪状态 = false;
        bool 当前陀螺仪状态 = false;
        int 无目标计数 = 0;
        const int 最大无目标计数 = 1; // 连续5帧无目标则停止陀螺仪

        while (1)
        {
            // 检查自瞄是否初始化
            if (!自瞄.初始化)
            {
                // 自瞄未初始化时，确保陀螺仪停止
                if (上次陀螺仪状态 && gyro != nullptr && gyro->isRunning()) {
                    gyro->update(0, 0, false);
                    上次陀螺仪状态 = false;
                }
                usleep(当前自瞄速度 * 10000);
                continue;
            }
            
            auto &配置 = 武器触发配置[自身数据.手持];
            int TempRange = 0;
            if (自身数据.Fov < 75)
            {
                TempRange = 自瞄.开镜自瞄范围;
            }
            else
            {
                TempRange = 自瞄.当前自瞄范围;
            }
            
            自瞄.自瞄范围 = (自身数据.手持 == 104003 || 自身数据.手持 == 104005 || 
                          自身数据.手持 == 104100 || 自身数据.手持 == 104004) ? 
                          自瞄.喷子自瞄范围 : TempRange;
            
            // 寻找最近目标
            findminat();
            
            // 更新陀螺仪状态
            当前陀螺仪状态 = (自瞄.瞄准目标 != -1 && 
                            自瞄函数[自瞄.瞄准目标].准心距离 <= 自瞄.自瞄范围 &&
                            自瞄触发(自瞄函数[自瞄.瞄准目标].距离));
            
            // 跟踪无目标计数
            if (!当前陀螺仪状态) {
                无目标计数++;
                if (无目标计数 >= 最大无目标计数 && 上次陀螺仪状态) {
                    // 连续多帧无目标，停止陀螺仪
                    if (gyro != nullptr && gyro->isRunning()) {
                        gyro->update(0, 0, false);
                      //  printf("连续%d帧无目标，停止陀螺仪\n", 最大无目标计数);
                    }
                    上次陀螺仪状态 = false;
                    无目标计数 = 0;
                }
            } else {
                无目标计数 = 0; // 重置计数
            }
            
            // 如果陀螺仪状态从开启变为关闭，发送停止命令
            if (上次陀螺仪状态 && !当前陀螺仪状态 && gyro != nullptr && gyro->isRunning()) {
                gyro->update(0, 0, false);
              //  printf("自瞄目标丢失，停止陀螺仪\n");
            }
            
            上次陀螺仪状态 = 当前陀螺仪状态;
            
            if (自瞄.瞄准目标 == -1)
            {
                if (自瞄.持续自瞄中) {
                    自瞄.持续自瞄中 = false;
                    strncpy(AimName, "", sizeof(AimName) - 1);
                }
                usleep(当前自瞄速度 * 10000);
                continue;
            }
            
            if (自瞄.持续锁定 && 自瞄.持续自瞄中 && 自身数据.开火 == 1) {
                if (strcmp(自瞄函数[自瞄.瞄准目标].名字.c_str(), AimName) != 0) {
                    strncpy(AimName, 自瞄函数[自瞄.瞄准目标].名字.c_str(), sizeof(AimName) - 1);
                    AimName[sizeof(AimName) - 1] = '\0';
                }
            }
            
            if (自瞄.持续锁定 && 自瞄.持续自瞄中 && 自身数据.开火 != 1) {
                自瞄.持续自瞄中 = false;
                strncpy(AimName, "", sizeof(AimName) - 1);
            }
            
            // =============== 动态自瞄速度计算 ===============
            当前自瞄速度 = 自瞄.自瞄速度;

            // 根据自瞄条件和FOV状态选择速度
            if (自瞄.自瞄条件 == 0 || 自瞄.自瞄条件 == 2) {
                if (自身数据.Fov > 75) {
                    // 腰射状态
                    当前自瞄速度 = 自瞄.腰射自瞄速度;
                } else {
                    // 开镜状态
                    当前自瞄速度 = 自瞄.开镜自瞄速度;
                }
            } else if (自瞄.自瞄条件 == 1) {
                当前自瞄速度 = 自瞄.开镜自瞄速度;
            }

            // 检查武器独立配置
   /*         if (配置.独立调节) {
                当前自瞄速度 = 武器参数配置[自身数据.手持].自瞄速度;
            }*/
            // ===================================================
            
            float ToReticleDistance = 自瞄函数[自瞄.瞄准目标].准心距离;
            float BulletFlightTime = 自瞄函数[自瞄.瞄准目标].距离 / 自身数据.子弹速度;
            float FlyTime;
            float 预判力度a = 自瞄.预判力度;
            
            if (对象信息.敌人信息.乘坐载具)
            {
                预判力度a = 预判度.扫车;
            }
            else
            {
                if (武器触发配置[自身数据.手持].独立预判)
                {
                    预判力度a = 武器参数配置[自身数据.手持].预判力度;
                }
            }

            if (自瞄函数[自瞄.瞄准目标].距离 >= 40) {
                FlyTime = 自瞄函数[自瞄.瞄准目标].距离 / (自身数据.子弹速度 * 0.01f) * 预判力度a;
            } else {
                FlyTime = 自瞄函数[自瞄.瞄准目标].距离 / (自身数据.子弹速度 * 0.0055f) * 预判力度a;
            }
            
            float DropM = 540.0f * BulletFlightTime * BulletFlightTime;
            float 压枪力度 = 自瞄.压枪力度;
            
            if (武器触发配置[自身数据.手持].独立压枪)
            {
                压枪力度 = 武器参数配置[自身数据.手持].压枪力度;
            }
            
            if (自身数据.人物高度 == 120.0f) {
                压枪力度 = 自瞄.压枪力度 - (Recoil(自身数据.手持) * 自瞄.趴下位置调节);
            }
            
            NowCoor[0] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.X;
            NowCoor[1] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Y;
            NowCoor[2] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Z;
            obj.X = NowCoor[0] + (自瞄函数[自瞄.瞄准目标].人物向量.X * FlyTime);
            obj.Y = NowCoor[1] + (自瞄函数[自瞄.瞄准目标].人物向量.Y * FlyTime);
            obj.Z = NowCoor[2] + (自瞄函数[自瞄.瞄准目标].人物向量.Z * FlyTime) + DropM;

            if (自身数据.开火 == 1) {
                obj.Z -= 自瞄函数[自瞄.瞄准目标].距离 * 压枪力度 * 自身数据.后坐力数据;
            }
            
            if (自身数据.手持握把 == 202004)
            {
                obj.Z += 自瞄函数[自瞄.瞄准目标].距离 * 轻型压枪力度 * GetWeaponId(自身数据.手持);
            } else if(自身数据.手持握把 == 202006) {
                obj.Z += 自瞄函数[自瞄.瞄准目标].距离 * 拇指压枪力度 * GetWeaponId(自身数据.手持);
            } else if(自身数据.手持握把 == 202002) {
                obj.Z += 自瞄函数[自瞄.瞄准目标].距离 * 垂直压枪力度 * GetWeaponId(自身数据.手持);
            } else if(自身数据.手持握把 == 202001) {
                obj.Z += 自瞄函数[自瞄.瞄准目标].距离 * 直角压枪力度 * GetWeaponId(自身数据.手持);
            }
            
            D2DVector vpvp = WorldToScreen2(obj);
            float AimDs = sqrt(pow(PX - vpvp.X, 2) + pow(PY - vpvp.Y, 2));
            
            

            

                        
            
            
            
            
            
            
            if(自瞄.动态自瞄 && (自身数据.开火==1 || 自身数据.开镜 == 1))
            {
                自瞄.动态范围 = AimDs;
            } else {
                自瞄.动态范围 = 自瞄.自瞄范围;
            }      
            
            zm_y = vpvp.X;
            zm_x = ScreenX - vpvp.Y;
            
            if (zm_x <= 0 || zm_x >= ScreenX || zm_y <= 0 || zm_y >= ScreenY)
            {
                if (gyro != nullptr && gyro->isRunning()) {
                    gyro->update(0, 0, false);
                }
                usleep(当前自瞄速度 * 10000);
                continue;
            }
            
            if (ToReticleDistance <= 自瞄.自瞄范围)
            {
                if (!自瞄触发(自瞄函数[自瞄.瞄准目标].距离))
                {
                    if (gyro != nullptr && gyro->isRunning()) {
                        gyro->update(0, 0, false);
                    }
                    usleep(当前自瞄速度 * 10000);
                    continue;
                }
                
                // 优化后的FOV灵敏度计算
                float FovFactor = 计算FovFactor(自身数据.Fov);
                float Acc = getScopeAcc((int)(90 / 自身数据.Fov)) * FovFactor;
                
                // 计算目标偏移量
                float deltaX = zm_x - ScrXH;
                float deltaY = zm_y - ScrYH;
                
                // 应用FOV缩放
                deltaX *= FovFactor;
                deltaY *= FovFactor;
                
                // 使用计算出的自瞄速度进行平滑过渡
                TargetX = lastTargetX * (1 - smoothFactor) + (-deltaX / 当前自瞄速度 * Acc) * smoothFactor;
                TargetY = lastTargetY * (1 - smoothFactor) + (deltaY / 当前自瞄速度 * Acc) * smoothFactor;
                
                // 边界检查
                if (TargetY >= 35 || TargetX >= 35 || TargetY <= -35 || TargetX <= -35)
                {
                    if (gyro != nullptr && gyro->isRunning()) {
                        gyro->update(0, 0, false);
                    }
                    usleep(当前自瞄速度 * 10000);
                    continue;
                }

                // 陀螺仪控制部分
                float dx = vpvp.X - (ScreenY / 2);
                float dy = vpvp.Y - (ScreenX / 2);

                float gyroAcc = getScopeAcc((int)(90 / 自身数据.Fov));
                dx *= gyroAcc;
                dy *= gyroAcc;
                dx *= 当前自瞄速度;
                dy *= 当前自瞄速度;

                float pitch = -dy;
                float yaw = dx;

                if (自瞄.充电口方向)
                {
                    pitch = -pitch;
                    yaw = -yaw;
                }

                float sendX = pitch * 0.004f;
                float sendY = yaw * 0.004f;

                if (displayInfo.orientation == 3)
                {
                    sendX = -sendX;
                    sendY = -sendY;
                }

                if (gyro && 当前陀螺仪状态)
                {
                    gyro->update(sendX, sendY, true);
                }
                
                // 保存当前值用于下一次平滑
                lastTargetX = TargetX;
                lastTargetY = TargetY;
            }
            else
            {
                // 目标不在自瞄范围内，确保陀螺仪停止
                if (上次陀螺仪状态 && gyro != nullptr && gyro->isRunning()) {
                    gyro->update(0, 0, false);
                    上次陀螺仪状态 = false;
                }
            }
            
            usleep(当前自瞄速度 * 10000);
            AimFPS.SetFps(按钮.当前帧率);
            AimFPS.AotuFPS();
        } });
    自瞄线程->detach();
}

// 在 绘制 类中添加停止陀螺仪的方法

void 绘制::停止陀螺仪()
{
    if (gyro != nullptr && gyro->isRunning())
    {
        gyro->stop();
        //    printf("手动停止陀螺仪\n");
    }
}

void 绘制::重置陀螺仪()
{
    if (gyro != nullptr)
    {
        gyro->reset();
        //   printf("重置陀螺仪\n");
    }
}
// 修复：FOV因子计算函数
float 绘制::计算FovFactor(float currentFov)
{
    // FOV越小（开镜），灵敏度越低，瞄准越精确
    // FOV越大（腰射），灵敏度越高，反应越快

    const float baseFov = 80.0f; // 基础FOV
    float factor = 1.0f;

    if (currentFov <= 45.0f)
    {
        // 高倍镜：更低的灵敏度，更精确的瞄准
        factor = 0.3f + (currentFov / 45.0f) * 0.3f;
    }
    else if (currentFov <= 60.0f)
    {
        // 低倍镜：中等灵敏度
        factor = 0.6f + ((currentFov - 45.0f) / 15.0f) * 0.2f;
    }
    else if (currentFov <= 75.0f)
    {
        // 腰射/肩射：较高灵敏度
        factor = 0.8f + ((currentFov - 60.0f) / 15.0f) * 0.15f;
    }
    else
    {
        // 第三人称：最高灵敏度
        factor = 0.95f + ((currentFov - 75.0f) / 15.0f) * 0.05f;
    }

    // 确保因子在合理范围内
    factor = std::max(0.1f, std::min(1.5f, factor));

    return factor;
}

// 修复：删除重复的陀螺仪灵敏度补偿函数，只保留一个
// 注意：删除原来第1326行的重复定义

// 新增：动态FOV范围调整函数
float 绘制::动态Fov范围调整(float baseRange, float currentFov)
{
    // FOV越小，瞄准范围应该越小（更精确）
    // FOV越大，瞄准范围可以适当增大（更宽松）

    float adjustment = 1.0f;

    if (currentFov <= 45.0f)
    {
        adjustment = 0.7f; // 高倍镜缩小范围
    }
    else if (currentFov <= 60.0f)
    {
        adjustment = 0.85f;
    }
    else if (currentFov <= 75.0f)
    {
        adjustment = 1.0f;
    }
    else
    {
        adjustment = 1.15f; // 腰射适当扩大范围
    }

    return baseRange * adjustment;
}
FVector2D 绘制::WorldToScreen(const FVector_class &WorldLocation)
{
    FVector2D ScreenLocation;
    FMatrix_class RotationMatrix = 自身数据.准星.GetMatrix();
    FVector_class AxisX = RotationMatrix.GetScaledAxisX();
    FVector_class AxisY = RotationMatrix.GetScaledAxisY();
    FVector_class AxisZ = RotationMatrix.GetScaledAxisZ();

    FVector_class CameraLocation;
    CameraLocation = 自身数据.相机坐标;
    FVector_class vDelta(WorldLocation - CameraLocation);
    FVector_class vTransformed(vDelta | AxisY, vDelta | AxisZ, vDelta | AxisX);
    if (vTransformed.Z == 0.0f)
        vTransformed.Z = -0.001f;

    auto VieW = vTransformed.Z;

    if (vTransformed.Z < 0.0f)
        vTransformed.Z = -vTransformed.Z;

    float ScreenWidth, ScreenHeight;
    ScreenWidth = displayInfo.width;
    ScreenHeight = displayInfo.height;
    float ScreenCenterX = ScreenWidth / 2.0f;
    float ScreenCenterY = ScreenHeight / 2.0f;
    float TangentFOV = tanf(ConvertToRadians(自身数据.Fov / 2.0f));

    ScreenLocation.X = (ScreenCenterX + vTransformed.X * (ScreenCenterX / TangentFOV) / vTransformed.Z);
    ScreenLocation.Y = (ScreenCenterY - vTransformed.Y * (ScreenCenterX / TangentFOV) / vTransformed.Z);

    if (VieW != INFINITY && VieW > 0.0f)
        return ScreenLocation;
    return FVector2D(INFINITY, INFINITY);
}

D2DVector 绘制::WorldToScreen2(const FVector_class &WorldLocation)
{
    D2DVector ScreenLocation;
    FMatrix_class RotationMatrix = 自身数据.准星.GetMatrix();
    FVector_class AxisX = RotationMatrix.GetScaledAxisX();
    FVector_class AxisY = RotationMatrix.GetScaledAxisY();
    FVector_class AxisZ = RotationMatrix.GetScaledAxisZ();
    FVector_class CameraLocation;
    CameraLocation = 自身数据.相机坐标;
    FVector_class vDelta(WorldLocation - CameraLocation);
    FVector_class vTransformed(vDelta | AxisY, vDelta | AxisZ, vDelta | AxisX);
    if (vTransformed.Z == 0.0f)
        vTransformed.Z = -0.001f;
    auto VieW = vTransformed.Z;
    if (vTransformed.Z < 0.0f)
        vTransformed.Z = -vTransformed.Z;
    float ScreenWidth, ScreenHeight;
    ScreenWidth = displayInfo.width;
    ScreenHeight = displayInfo.height;
    float ScreenCenterX = ScreenWidth / 2.0f;
    float ScreenCenterY = ScreenHeight / 2.0f;
    float TangentFOV = tanf(ConvertToRadians(自身数据.Fov / 2.0f));
    ScreenLocation.X = (ScreenCenterX + vTransformed.X * (ScreenCenterX / TangentFOV) / vTransformed.Z);
    ScreenLocation.Y = (ScreenCenterY - vTransformed.Y * (ScreenCenterX / TangentFOV) / vTransformed.Z);
    return ScreenLocation;
}

// 更新 数据

// ========== 更新后的偏移宏定义 ==========
#define GWorld 0x15772758                 // 世界地址基址
#define STExtraBaseCharacter 0x3478       // 自身Actor偏移 (链中)
#define GName 0x154DAD38                  // 类名基址
#define PawnStateRepSyncData 0x1680       // 状态
#define TeamID 0xb78                      // 队伍ID
#define bIsGunADS 0x17b8                  // 是否开镜
#define bIsWeaponFiring 0x2608            // 是否开火
#define CurrentUsingWeaponSafety 0x1108   // 当前武器指针
#define CachedBulletTrackComponent 0x1f60 // 缓存子弹轨迹组件 (用于喷子开火，原0x1cF0)
#define ShootBursts 0x23c                 // 喷子开火计数 (原0x234)
#define RepWeaponID 0xda8                 // 武器ID
#define CurBulletNumInClip 0x1f80         // 当前弹夹子弹数
#define CurMaxBulletNumInOneClip 0x1f84   // 弹夹最大容量
#define STPlayerController 0x5c58         // 玩家控制器
#define PlayerCameraManager 0x660         // 相机管理器
#define CAMERA_MANAGER_OFFSET 0x658       // 相机管理器偏移 (用于读取相机数据)
#define SpeedValue 0x1018                 // 人物高度/速度值 (原0xee8)
#define HighWalkSpeed 0x38c4              // 高走速度 (用于判断是否为玩家)
#define bIsAI 0xb94                       // 是否AI
#define RootComponent 0x260               // 根组件
#define VehicleCommon 0xbd8               // 载具通用数据指针
#define VehicleHP 0x1f8                   // 载具当前血量
#define VehicleMaxHP 0x1f4                // 载具最大血量
#define VehicleFuel 0x21c                 // 载具当前油量
#define VehicleMaxFuel 0x218              // 载具最大油量
#define PickUpDataList 0xd88              // 拾取数据列表 (盒子物资)
#define Health 0xFF8                      // 当前血量
#define HealthMax 0x1000                  // 最大血量
#define PlayerName 0xaf8                  // 玩家名字
#define PlayerUID 0xb10                   // 玩家UID (新增)
#define VelocitySafety 0x10fc             // 速度向量
#define WeaponEntityComp 0xC68            // 武器实体组件
#define BulletFireSpeed 0x15CC            // 子弹速度
#define AccessoriesVRecoilFactor 0x1EC8   // 后坐力系数 (原0x1e98)
#define Mesh 0x658                        // Mesh组件
#define Matrix 0x1573BEE8                 // 矩阵链基址 (原0x14573BEE8)
#define Matrix_Tol 0x154DAD38             // 备用矩阵链基址 (原0x1495C2A0)
#define MeshPoint 0x828                   // 骨骼节点偏移
// 对局信息 (新偏移链)
#define AliveRealPlayerNum 0x12A0 // 本局真实玩家数
#define AlivePlayerNum 0x129C     // 本局剩余玩家总数
#define AliveTeamNum 0x130C       // 本局剩余队伍数
void 绘制::更新地址数据()
{
    // 本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险

    // ========== 基础地址 (使用新偏移) ==========
    地址.世界地址 = 读写.getPtr64(读写.getPtr64(地址.libue4 + 0x15772758) + 0xb0);                                                                     // GWorld
    地址.自身地址 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x15772758) + 0xb8) + 0x88) + 0x30) + 0x3478); // 自身Actor
    地址.矩阵地址 = 读写.getPtr64(读写.getPtr64(地址.libue4 + 0x1573BEE8) + 0x20) + 0x270;                                                             // ViewMatrix
    地址.矩阵地址_Tol = 读写.getPtr64(读写.getPtr64(地址.libue4 + 0x154DAD38) + 0x98) + 0x10440;                                                       // 备用矩阵链

    // 数组地址与数量 (未解密时)
    地址.数组地址 = 读写.getPtr64(地址.世界地址 + 0xA0);
    世界数量 = 读写.getDword(地址.世界地址 + 0xA8);

    // 解密数组优先 (如果已启用)
    if (解密数组)
    {
        地址.数组地址 = 解密数组;
        世界数量 = 读写.getDword(地址.世界地址 + 0xB8);
    }

    地址.类地址 = 读写.getPtr64(地址.libue4 + 0x14E33C18); // GName 基址

    // ========== 自身坐标 (通过 RootComponent + 0x200) ==========
    uintptr_t rootComp = 读写.getPtr64(地址.自身地址 + 0x260);
    if (rootComp != 0)
    {
        读写.readv(rootComp + 0x200, &自身数据.坐标, sizeof(自身数据.坐标));
    }

    // ========== 自身基础属性 ==========
    自身数据.自身队伍 = 读写.getDword(地址.自身地址 + 0xb78);                 // TeamID
    自身数据.自身状态 = 读写.getDword(读写.getPtr64(地址.自身地址 + 0x1680)); // PawnStateRepSyncData
    自身数据.开镜 = 读写.getDword(地址.自身地址 + 0x17b8);                    // bIsGunADS
    自身数据.开火 = 读写.getDword(地址.自身地址 + 0x2608);                    // bIsWeaponFiring

    // ========== 手持武器信息 ==========
    uintptr_t weaponPtr = 读写.getPtr64(地址.自身地址 + 0x1108); // CurrentUsingWeaponSafety
    if (weaponPtr != 0)
    {
        自身数据.手持id = 读写.getDword(weaponPtr + 0xda8); // RepWeaponID
        自身数据.手持 = heldconversion(自身数据.手持id);
    }

    // ========== 相机与FOV ==========
    uintptr_t controller = 读写.getPtr64(地址.自身地址 + 0x5c58); // STPlayerController
    if (controller != 0)
    {
        uintptr_t camManager = 读写.getPtr64(controller + 0x660); // PlayerCameraManager
        if (camManager != 0)
        {
            读写.readv(camManager + 0x650, &自身数据.相机坐标, sizeof(自身数据.相机坐标));
            读写.readv(camManager + 0x650 + 0x18, &自身数据.准星, sizeof(自身数据.准星));
            自身数据.Fov = 读写.getFloat(camManager + 0x680);
        }
        // 准星Y (从0x5fc改为0x604)
        自身数据.准星Y = 读写.getFloat(controller + 0x604) - 90.0f;
    }

    // ========== 子弹速度与后坐力 ==========
    uintptr_t weaponEntityComp = 读写.getPtr64(weaponPtr + 0xC68); // WeaponEntityComp
    if (weaponEntityComp != 0)
    {
        自身数据.子弹速度 = 读写.getFloat(weaponEntityComp + 0x15CC);   // BulletFireSpeed
        自身数据.后坐力数据 = 读写.getFloat(weaponEntityComp + 0x1EC8); // AccessoriesVRecoilFactor (新偏移)
    }

    // ========== 人物高度 (用于趴下调节) ==========
    自身数据.人物高度 = 读写.getFloat(地址.自身地址 + 0x1018); // 原0xee8改为0x1018

    // ========== 手持握把 (新增) ==========
    uintptr_t weaponEntity = 读写.getPtr64(地址.自身地址 + 0x10E8);
    if (weaponEntity != 0)
    {
        uintptr_t weaponComp = 读写.getPtr64(weaponEntity + 0xBB8);
        if (weaponComp != 0)
        {
            自身数据.手持握把 = 读写.getDword(weaponComp + 0xEA0);
        }
    }

    // ========== 对局信息 (新偏移链) ==========
    if (按钮.显示对局信息) // 原为：if (按钮.显示对局信息 || 按钮.显示全图信息)
    {
        uintptr_t gameState = 读写.getPtr64(地址.世界地址 + 0xAB8);
        if (gameState != 0)
        {
            自身数据.真人数量 = 读写.getDword(gameState + 0x12A0);
            自身数据.人机数量 = 读写.getDword(gameState + 0x129C) - 自身数据.真人数量;
            自身数据.队伍数量 = 读写.getDword(gameState + 0x130C);
        }
    }

    // ========== 陀螺仪灵敏度 (保留原逻辑，但偏移链可能需要后续验证) ==========
    if (按钮.刷新灵敏度)
    {
        // 注意：这里偏移链可能需要更新，暂保持原样，如需要可参照新链调整
        自身数据.陀螺仪灵敏度第三人称 = 读写.getFloat(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x12F65848) + 0x438) + 0x80) + 0xbb8) + 0x58c);
        自身数据.陀螺仪灵敏度第一人称 = 读写.getFloat(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x12F65848) + 0x438) + 0x80) + 0xbb8) + 0x5B0);
        自身数据.陀螺仪灵敏度红点 = 读写.getFloat(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x12F65848) + 0x438) + 0x80) + 0xbb8) + 0x590);
        自身数据.陀螺仪灵敏度二倍 = 读写.getFloat(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x12F65848) + 0x438) + 0x80) + 0xbb8) + 0x594);
        自身数据.陀螺仪灵敏度三倍 = 读写.getFloat(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x12F65848) + 0x438) + 0x80) + 0xbb8) + 0x5A0);
        自身数据.陀螺仪灵敏度四倍 = 读写.getFloat(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x12F65848) + 0x438) + 0x80) + 0xbb8) + 0x598);
        自身数据.陀螺仪灵敏度六倍 = 读写.getFloat(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x12F65848) + 0x438) + 0x80) + 0xbb8) + 0x5a4);
        自身数据.陀螺仪灵敏度八倍 = 读写.getFloat(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x12F65848) + 0x438) + 0x80) + 0xbb8) + 0x59C);
        if (自身数据.陀螺仪灵敏度第三人称 != 0)
        {
            按钮.刷新灵敏度 = false;
        }
    }
}
ImColor 绘制::floatArrToImColor(float arr[4])
{
    return ImColor(arr[0] * 255, arr[1] * 255, arr[2] * 255, arr[3] * 255);
}

void 绘制::更新对象数据()
{
    if (自瞄.触摸位置)
        绘图.绘制自瞄触摸范围(自瞄.触摸范围, 自瞄.触摸范围X, 自瞄.触摸范围Y);
    if (连点.连点位置)
        绘图.绘制连点位置(连点.触摸范围X, 连点.触摸范围Y);
    if (!自瞄.动态自瞄)
    {
        自瞄.动态范围 = 自瞄.自瞄范围;
    }
    if (连点.监听位置)
    {
        绘图.绘制监听区域(连点.监听边长, 连点.监听位置X, 连点.监听位置Y);
    }
    if (自瞄.初始化 && !自瞄.隐藏自瞄圈)
    {
        if (自身数据.手持 != 0)
        {
            ImGui::GetForegroundDrawList()->AddCircle({PX, PY}, 自瞄.动态范围, ImColor(255, 255, 255, 255), 0, 1.5f);
        }
    }
    if (按钮.雷达)
    {
        绘图.RenderRadarScan(ImGui::GetForegroundDrawList(), ImVec2(按钮.雷达X, 按钮.雷达Y), 150.0f, 100, 按钮.rotationAngle, 150.0f, 自身数据.准星Y);
    }
    int 绘制人机 = 0, 绘制真人 = 0;
    被瞄准对象数量 = 0;
    自瞄.瞄准对象数量 = 0;
    for (int a = 0; a < 世界数量; a++)
    {
        对象地址.敌人地址 = 读写.getPtr64(地址.数组地址 + a * 8);
        读写.readv(读写.getPtr64(对象地址.敌人地址 + RootComponent) + 0x200, &对象信息.敌人信息.坐标, sizeof(对象信息.敌人信息.坐标)); // 更新坐标
        对象信息.敌人信息.距离 = 计算.计算距离(自身数据.坐标, 对象信息.敌人信息.坐标);                                                 // 距离
        FVector2D screenPos = WorldToScreen(对象信息.敌人信息.坐标);
        FVector2D footPos = WorldToScreen(FVector_class{对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y, 对象信息.敌人信息.坐标.Z - 5});   // 脚部坐标
        FVector2D headPos = WorldToScreen(FVector_class{对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y, 对象信息.敌人信息.坐标.Z + 205}); // 头部坐标
        float r_x = footPos.X;                                                                                                                // 身体中心X坐标
        float r_y = footPos.Y;                                                                                                                // 身体中心Y坐标
        float r_z = headPos.Y;                                                                                                                // 脚部Y坐标
        float camear_r = (screenPos.X != INFINITY && screenPos.Y != INFINITY) ? 1.0f : -1.0f;
        D4DVector t_屏幕坐标 = {r_x - (r_y - r_z) / 4, r_y, (r_y - r_z) / 2, r_y - r_z};
        if (对象信息.敌人信息.距离 > 按钮.绘制最大距离)
        {
            continue;
        }
        char 计算地址[256] = "我是帅哥";
        sprintf(计算地址, "%lx", 对象地址.敌人地址);
        char 自救计算地址[256] = "我是篮子";
        sprintf(自救计算地址, "%lx", 对象地址.敌人地址);
        if (按钮.手雷预警)
        {
            int 手雷ID = 读写.getDword(对象地址.敌人地址 + 0x794); // 手雷id
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
                        { // 手雷字体显示
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
                                // 绘图.ExplosionRange(对象信息.敌人信息.坐标, ImColor(255, 0, 0, 255), 350, 1.5f, 自身数据.矩阵);
                            }
                            else
                            {
                                // 绘图.ExplosionRange1(对象信息.敌人信息.坐标, ImColor(255, 0, 0, 255), 350, 1.5f, 自身数据.矩阵);
                            }
                        }
                    }
                }

                绘图::VecTor3 坐标 = {对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y, 对象信息.敌人信息.坐标.Z};
            }
        }

        char ClassName[64] = "";
        char 对象信息_max[200] = "";
        int ClassID = 读写.getPtr64(对象地址.敌人地址 + 24);
        long int FNameEntry;
        if (t_屏幕坐标.W > 0)
        { // 车辆物资区域
            FNameEntry = 读写.getPtr64(读写.getPtr64(地址.类地址 + (ClassID / 0x4000) * 0x8) + (ClassID % 0x4000) * 0x8);
            读写.readv(FNameEntry + 0xC, ClassName, 64);

            // 设置描边颜色
            ImColor outlineColor = ImColor(0, 0, 0, 255); // 黑色描边

            if (按钮.Debug)
            {
                if (按钮.Debug模式 == 0)
                {
                    auto textSize = ImGui::CalcTextSize(ClassName, 0, 物资字体大小);
                    ImVec2 textPos = {r_x - (textSize.x / 2), r_y};
                    // 绘制描边
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
                    // 绘制中心文字
                    ImGui::GetForegroundDrawList()->AddText(NULL, 30, textPos, ImColor(255, 255, 255, 255), ClassName);
                }
                else
                {
                    auto textSize = ImGui::CalcTextSize(计算地址, 0, 物资字体大小);
                    ImVec2 textPos = {r_x - (textSize.x / 2), r_y};
                    // 绘制描边
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
                    // 绘制中心文字
                    ImGui::GetForegroundDrawList()->AddText(NULL, 30, textPos, ImColor(255, 255, 255, 255), 计算地址);
                }
            }
            float Mlline = calculateDistance(PX, PY, t_屏幕坐标.X, t_屏幕坐标.Y);
            if (按钮.车辆)
            {
                long long VehicleData = 读写.getPtr64(对象地址.敌人地址 + VehicleCommon);
                float 载具血量 = 读写.getFloat(VehicleData + VehicleHP) / 读写.getFloat(VehicleData + VehicleMaxHP) * 100;
                float 载具油量 = 读写.getFloat(VehicleData + VehicleFuel) / 读写.getFloat(VehicleData + VehicleMaxFuel) * 100;
                if ((int)载具血量 != 0 && (int)载具油量 != 0 && 载具油量 <= 100 && 载具血量 <= 100 && 载具油量 >= 0 && 载具血量 >= 0)
                {
                    std::string name = getMaterialName(ClassName);
                    if (name != "Error" && 读写.getPtr64(地址.自身地址 + 0x1b8) != 对象地址.敌人地址 && 对象信息.敌人信息.距离 > 5)
                    {
                        name += std::to_string((int)对象信息.敌人信息.距离) + "米";
                        auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 20);
                        ImVec2 textPos = {r_x - (textSize.x / 2), r_y};
                        ImColor color = ImColor(static_cast<int>(车辆颜色[0] * 255 + 0.5), static_cast<int>(车辆颜色[1] * 255 + 0.5), static_cast<int>(车辆颜色[2] * 255 + 0.5), static_cast<int>(车辆颜色[3] * 255 + 0.5));

                        // 绘制描边
                        for (int x = -1; x <= 1; x++)
                        {
                            for (int y = -1; y <= 1; y++)
                            {
                                if (x != 0 || y != 0)
                                {
                                    ImGui::GetForegroundDrawList()->AddText(NULL, 20, {textPos.x + x, textPos.y + y}, outlineColor, name.c_str());
                                }
                            }
                        }
                        // 绘制中心文字
                        ImGui::GetForegroundDrawList()->AddText(NULL, 20, textPos, color, name.c_str());

                        char healthText[32];
                        sprintf(healthText, "血量: %.0f", 载具血量);
                        char fuelText[32];
                        sprintf(fuelText, "油量: %.0f", 载具油量);

                        // 计算文本位置
                        ImVec2 healthTextSize = ImGui::CalcTextSize(healthText);
                        ImVec2 fuelTextSize = ImGui::CalcTextSize(fuelText);
                        if (Mlline <= 80.0f)
                        {
                            ImVec2 healthPos = {r_x - (healthTextSize.x + fuelTextSize.x + 10) / 2, r_y + 25};
                            ImVec2 fuelPos = {r_x + (healthTextSize.x + fuelTextSize.x + 10) / 2 - fuelTextSize.x, r_y + 25};

                            // 绘制血量描边
                            for (int x = -1; x <= 1; x++)
                            {
                                for (int y = -1; y <= 1; y++)
                                {
                                    if (x != 0 || y != 0)
                                    {
                                        ImGui::GetForegroundDrawList()->AddText(NULL, 20, {healthPos.x + x, healthPos.y + y}, outlineColor, healthText);
                                    }
                                }
                            }
                            // 绘制血量文字
                            ImGui::GetForegroundDrawList()->AddText(NULL, 20, healthPos, ImColor(255, 0, 0, 255), healthText);

                            // 绘制油量描边
                            for (int x = -1; x <= 1; x++)
                            {
                                for (int y = -1; y <= 1; y++)
                                {
                                    if (x != 0 || y != 0)
                                    {
                                        ImGui::GetForegroundDrawList()->AddText(NULL, 20, {fuelPos.x + x, fuelPos.y + y}, outlineColor, fuelText);
                                    }
                                }
                            }
                            // 绘制油量文字
                            ImGui::GetForegroundDrawList()->AddText(NULL, 20, fuelPos, ImColor(0, 255, 0, 255), fuelText);
                        }
                    }
                }
            }

            if (按钮.物资总开关)
            {
                int MaterialID = 读写.getDword(对象地址.敌人地址 + 0x6f4);
                std::string name = getBoxName(MaterialID);
                if (name != "Error")
                {
                    name += "[";
                    name += std::to_string((int)对象信息.敌人信息.距离);
                    name += "米]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                    ImVec2 textPos = {r_x - (textSize.x / 2), r_y};
                    ImColor textColor = 绘制::floatArrToImColor(绘制::物资颜色);

                    // 绘制描边
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
                    // 绘制中心文字
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
            if (按钮.超级物资箱 && (strstr(ClassName, "EscapeBox_SpeEffect_C") != 0 or strstr(ClassName, "MilitarySupplyBoxBase_Baltic_Classic_C") != 0 or strstr(ClassName, "MilitarySupplyBoxBase_Baltic_Theme_C") != 0 or strstr(ClassName, "EscopeBox_SpeEffect_C") != 0))
            {
                std::string name = "超级物资箱[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (按钮.绘制空投 && (strstr(ClassName, "AirDropBox") != 0))
            {
                std::string name = "空投[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 设置颜色
                ImColor textColor = ImColor(255, 0, 0, 255);  // 黄色
                ImColor outlineColor = ImColor(0, 0, 0, 255); // 黑色描边

                // 绘制描边（8方向偏移1像素）
                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x != 0 || y != 0)
                        { // 不绘制中心点
                            ImGui::GetForegroundDrawList()->AddText(
                                NULL, 30,
                                {textPos.x + x, textPos.y + y},
                                outlineColor,
                                name.c_str());
                        }
                    }
                }

                // 绘制主文本（居中）
                ImGui::GetForegroundDrawList()->AddText(
                    NULL, 30, textPos,
                    textColor,
                    name.c_str());
            }

            if (strstr(ClassName, "_revivalAED_Pickup_C") != 0)
            {
                std::string name = "自救器[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (strstr(ClassName, "MilitarySupplyBoxBase_Baltic_Theme") != 0)
            {
                std::string name = "黑色物资箱子[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (strstr(ClassName, "BP_Pickup_Finger_C") != 0)
            {
                std::string name = "飞索[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (按钮.绘制信号枪 && (strstr(ClassName, "Pistol_Flaregun_") != 0 or strstr(ClassName, "Pistol_RevivalFlaregun_Wrapper") != 0) or strstr(ClassName, "BP_Pistol_RevivalFlaregun_C") != 0)
            {
                std::string name = "信号枪[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 0, 255), name.c_str());
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

            if (按钮.精英勋章 && (strstr(ClassName, "BP_CommercialWrapper_LV1_C") != 0 ||
                                  strstr(ClassName, "BP_CommercialWrapper_LV2_C") != 0 ||
                                  strstr(ClassName, "BP_CommercialWrapper_LV3_C") != 0 ||
                                  strstr(ClassName, "BP_CommercialWrapper_LV4_C") != 0 ||
                                  strstr(ClassName, "BP_CommercialWrapper_LV5_C") != 0))
            {
                std::string name = "精英勋章[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(0, 0, 255, 255), name.c_str());
            }

            if (strstr(ClassName, "MilitarySupplyBoxBase_Baltic_Theme_C") != 0)
            {
                std::string name = "黑色物资箱[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (按钮.爆炸猎弓 && strstr(ClassName, "BP_Other_HuntingBow_Wrapper_C") != 0)
            {
                std::string name = "爆炸猎弓[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (strstr(ClassName, "BP_WAlnnerWrapperList_C") != 0)
            {
                std::string name = "黑色物资箱[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (strstr(ClassName, "ckUp_BP_Bag_Lv3_C") != 0 or strstr(ClassName, "PickUp_BP_Bag_Lv3_B_C") != 0)
            {
                std::string name = "三级包[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (按钮.绘制信号枪 && strstr(ClassName, "Ammo_Flare") != 0)
            {
                std::string name = "信号枪子弹[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 0, 255), name.c_str());
            }

            if (按钮.绘制金插 && strstr(ClassName, "PeopleSkill") != 0)
            {
                std::string name = "金色插件[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 0, 255, 255), name.c_str());
            }

            if (按钮.盒子 && (strstr(ClassName, "CharacterDeadInventoryBox_C") != 0 or strstr(ClassName, "PickUpListWrapperActor") != 0 or strstr(ClassName, "RollTombBox_") != 0 or strstr(ClassName, "EscapePlayerTombBox") != 0 or strstr(ClassName, "DeadInventoryBox") != 0 or strstr(ClassName, "_TrainingBoxLi") != 0))
            {
                std::string name = "盒子[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "M]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 物资字体大小);
                ImVec2 textPos = {r_x - (textSize.x / 2), r_y};

                // 绘制描边
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
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(NULL, 物资字体大小, textPos, ImColor(255, 255, 0, 255), name.c_str());
            }

            int 开启状态 = 读写.getDword(对象地址.敌人地址 + 0x270);
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

                    // 绘制描边
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
                    // 绘制中心文字
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

                    // 绘制描边
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
                    // 绘制中心文字
                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, textPos, ImColor(255, 255, 0, 255), name.c_str());
                }
            }

            if (按钮.盒子物资 && (读写.getDword(读写.getPtr64(对象地址.敌人地址 + PickUpDataList)) > 0 && 读写.getDword(读写.getPtr64(对象地址.敌人地址 + PickUpDataList)) < 10))
            {
                int 盒内物资数量 = 读写.getDword(对象地址.敌人地址 + PickUpDataList + 0x8);
                long int 物资数组 = 读写.getPtr64(对象地址.敌人地址 + PickUpDataList) + 0x4;
                float Aimatdistance = sqrt(pow(PX - r_x, 2) + pow(PY - r_y, 2));
                if (Aimatdistance < 50 && 自瞄.瞄准目标 == -1)
                {
                    int 文本高度 = 50;
                    for (int i = 0; i < 盒内物资数量; i++)
                    {
                        int 物资地址ID = 读写.getDword(物资数组 + 0x38 * i);
                        int 物资地址数量 = 读写.getDword((物资数组 + 0x38 * i) + 0x14);
                        std::string name = getBoxName1(物资地址ID);
                        if (name != "NULL")
                        {
                            name += "[" + to_string(物资地址数量) + "]";
                            auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                            文本高度 += 25;
                            ImVec2 textPos = {r_x - (textSize.x / 2), r_y - 文本高度};

                            // 绘制背景描边（已经有的）
                            ImGui::GetBackgroundDrawList()->AddText(NULL, 30, {(float)(textPos.x - 0.1), (float)(textPos.y - 0.1)}, ImColor(0, 0, 0, 255), name.c_str());
                            ImGui::GetBackgroundDrawList()->AddText(NULL, 30, {(float)(textPos.x + 0.1), (float)(textPos.y + 0.1)}, ImColor(0, 0, 0, 255), name.c_str());

                            // 绘制前景文字
                            ImGui::GetForegroundDrawList()->AddText(NULL, 30, textPos, GetRandomColorById(物资地址ID), name.c_str());
                        }
                    }
                }
            }

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

                    // 绘制描边
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
                    // 绘制中心文字
                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, textPos, ImColor(255, 255, 0, 255), name.c_str());
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
        if (读写.getFloat(对象地址.敌人地址 + HighWalkSpeed) == 479.5 || strstr(ClassName, "BPPawn_Escape_") != 0 || isboss)
        {
            D4DVector 屏外预警坐标(r_x, r_y, r_y - r_z, (r_y - r_z) / 2);
            对象信息.敌人信息.队伍 = 读写.getDword(对象地址.敌人地址 + TeamID);                                       // 敌人队伍编号   队编
            对象信息.敌人信息.isboot = (对象信息.敌人信息.队伍 == -1) ? 1 : 读写.getDword(对象地址.敌人地址 + bIsAI); // 人机
            对象信息.敌人信息.高级人机 = 读写.getDword(对象地址.敌人地址 + 0xb88);
            if (按钮.忽略人机 && 对象信息.敌人信息.isboot == 1)
            {
                continue;
            }
            对象信息.敌人信息.状态 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + PawnStateRepSyncData)); // 敌人状态
            对象信息.敌人信息.雷达 = 计算.rotateCoord(自身数据.准星Y, (自身数据.坐标.X - 对象信息.敌人信息.坐标.X) / 200, (自身数据.坐标.Y - 对象信息.敌人信息.坐标.Y) / 200);
            读写.readv(对象地址.敌人地址 + VelocitySafety, &对象信息.敌人信息.向量, sizeof(对象信息.敌人信息.向量)); // 敌人向量
            对象信息.敌人信息.Rotator = 读写.getFloat(对象地址.敌人地址 + 0x198);                                    // 用于被瞄准
            对象信息.敌人信息.当前血量 = 读写.getFloat(对象地址.敌人地址 + Health);                                  // 血量
            对象信息.敌人信息.最大血量 = 读写.getFloat(对象地址.敌人地址 + HealthMax);                               // 最大血量
            对象信息.敌人信息.乘坐载具 = 读写.getDword(对象地址.敌人地址 + 0x1b8) != 0;                              // 车辆向量
            对象信息.敌人信息.手持 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + CurrentUsingWeaponSafety) + RepWeaponID);
            对象信息.敌人信息.子弹数量 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + CurrentUsingWeaponSafety) + CurBulletNumInClip);
            对象信息.敌人信息.子弹最大数量 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + CurrentUsingWeaponSafety) + CurMaxBulletNumInOneClip);
            对象信息.敌人信息.角色实体 = 读写.getPtr64(对象地址.敌人地址 + 0x39b0);
            对象信息.敌人信息.实体列表地址 = 读写.getPtr64(对象信息.敌人信息.角色实体 + 0x818) + 0x8;
            对象信息.敌人信息.实体数量 = 读写.getDword(对象信息.敌人信息.角色实体 + 0x818 + 0x8);
            long int MeshOffset = 读写.getPtr64(对象地址.敌人地址 + 0x658); // mesh
            int Bonecount = 读写.getPtr64(MeshOffset + MeshPoint + 8);      // 骨骼节点
            D3DVector tempBones[17];
            // 调用时使用临时数组
            骨骼->更新骨骼数据(
                MeshOffset + 0x1f0,                       // 组件到世界变换
                读写.getPtr64(MeshOffset + 0x828) + 0x30, // 骨骼数组指针
                tempBones,                                // 输出数组
                Bonecount,                                // 骨骼数量
                对象信息.敌人信息.队伍,
                ClassName);
            // 将结果复制回原数组
            for (int i = 0; i < 15; i++)
            {
                对象信息.敌人信息.骨骼坐标[i].X = tempBones[i].X;
                对象信息.敌人信息.骨骼坐标[i].Y = tempBones[i].Y;
                对象信息.敌人信息.骨骼坐标[i].Z = tempBones[i].Z;
            }

            char temp[64];
            读写.getUTF8(temp, 读写.getPtr64(对象地址.敌人地址 + PlayerName)); // 敌人名字
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
                                    Antitankgrenade.erase(计算地址); // 没用的东西赶紧滚
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
                                Antitankgrenade.erase(计算地址); // 没用的东西赶紧滚
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
                        // 人机点 - 缩小、固定白色、不显示编号
                        ImGui::GetForegroundDrawList()->AddCircleFilled(
                            {按钮.雷达X + 对象信息.敌人信息.雷达.X, 按钮.雷达Y + 对象信息.敌人信息.雷达.Y},
                            8.0f,                       // 缩小圆点大小
                            ImColor(255, 255, 255, 200) // 白色，80%不透明度
                        );
                    }
                    else
                    {
                        // 真人点 - 放大一点，显示队伍编号
                        int team = 对象信息.敌人信息.队伍;

                        // 确保队伍编号在0-49范围内
                        int adjusted_team = team % 50;
                        adjusted_team = adjusted_team < 0 ? adjusted_team + 50 : adjusted_team;

                        // 将队伍编号映射到HSV色相（0.0~1.0范围）
                        float hue = static_cast<float>(adjusted_team) / 50.0f;

                        // 调整饱和度和明度以增加区分度
                        float saturation = 1.0f;                                             // 饱和度保持最大
                        float value = 1.0f - (static_cast<float>(adjusted_team % 5) * 0.1f); // 明度分五档变化

                        // 生成颜色（带透明度）
                        ImColor team_color = ImColor::HSV(hue, saturation, value, 0.8f); // 80% 不透明度

                        // 绘制更大的圆面
                        ImGui::GetForegroundDrawList()->AddCircleFilled(
                            {按钮.雷达X + 对象信息.敌人信息.雷达.X, 按钮.雷达Y + 对象信息.敌人信息.雷达.Y},
                            15.0f, // 增大圆点半径
                            team_color);

                        // 绘制队伍编号文本（位于圆面中心）
                        ImVec2 text_pos = {
                            按钮.雷达X + 对象信息.敌人信息.雷达.X,
                            按钮.雷达Y + 对象信息.敌人信息.雷达.Y};

                        // 计算文本偏移量（居中显示）
                        ImFont *font = ImGui::GetFont();
                        float customFontSize = 13.0f;
                        ImVec2 text_size = font->CalcTextSizeA(customFontSize, FLT_MAX, 0.0f, std::to_string(team).c_str());
                        text_pos.x -= text_size.x / 2.0f;
                        text_pos.y -= text_size.y / 2.0f;

                        // 绘制文本阴影
                        ImGui::GetForegroundDrawList()->AddText(
                            font, customFontSize, text_pos,
                            ImColor(0, 0, 0, 200),
                            std::to_string(team).c_str());

                        // 绘制文本正文
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
                    // 如果已存在计时且超过阈值，重置计时
                    auto it = 自救Timers.find(计算地址);
                    if (it != 自救Timers.end())
                    {
                        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                                           std::chrono::steady_clock::now() - it->second)
                                           .count();
                        if (elapsed > 6)
                        { // 可调整的阈值
                            自救Timers[计算地址] = std::chrono::steady_clock::now();
                        }
                    }
                    else
                    {
                        // 不存在则插入新计时
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
            if (t_屏幕坐标.W >= 0)
            {
                if (自瞄.倒地不瞄 && 对象信息.敌人信息.当前血量 <= 0)
                {
                }
                else if (自瞄.人机不瞄 && 对象信息.敌人信息.isboot == 1)
                {
                }
                else if (自瞄.扫车不瞄 && 对象信息.敌人信息.状态 == 2097168 || 对象信息.敌人信息.状态 == 4194320)
                {
                }
                else
                {
                    自瞄函数[自瞄.瞄准对象数量].距离 = 对象信息.敌人信息.距离;
                    自瞄函数[自瞄.瞄准对象数量].人物向量 = 对象信息.敌人信息.向量;
                    自瞄函数[自瞄.瞄准对象数量].血量 = 对象信息.敌人信息.当前血量;
                    自瞄函数[自瞄.瞄准对象数量].Bone = 读写.getPtr64(MeshOffset + MeshPoint) + 0x30; // 骨骼指针
                    自瞄函数[自瞄.瞄准对象数量].Human = MeshOffset + 0x1f0;                          // 骨骼human
                    自瞄函数[自瞄.瞄准对象数量].名字 = 对象信息.敌人信息.名字;
                    自瞄函数[自瞄.瞄准对象数量].阵营 = 对象信息.敌人信息.队伍;
                    自瞄函数[自瞄.瞄准对象数量].头 = 对象信息.敌人信息.头;
                    自瞄函数[自瞄.瞄准对象数量].甲 = 对象信息.敌人信息.甲;
                    自瞄函数[自瞄.瞄准对象数量].头甲包地址 = 对象信息.敌人信息.头甲包地址;
                    memcpy(自瞄函数[自瞄.瞄准对象数量].骨骼坐标, 对象信息.敌人信息.骨骼坐标, sizeof(对象信息.敌人信息.骨骼坐标));
                    //       int BonePointer = ShelterJudgment(LineOfSightToTab);
                    // 在 更新对象数据 函数中找到调用 ShelterJudgment 的地方
                    int BonePointer = ShelterJudgment(LineOfSightToTab, 对象信息.敌人信息.状态);
                    自瞄函数[自瞄.瞄准对象数量].掩体部位 = BonePointer;
                    if (BonePointer == 999)
                    {
                        BonePointer = 自瞄.瞄准部位;
                    }
                    自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[BonePointer];
                    自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - 骨骼二维坐标[BonePointer]->X, 2) + pow(PY - 骨骼二维坐标[BonePointer]->Y, 2));
                    自瞄函数[自瞄.瞄准对象数量].对象骨骼 = *骨骼二维坐标[BonePointer];
                    自瞄.瞄准对象数量++;
                }
            }

            /*           if (按钮.被瞄预警)
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
           */
            if (按钮.背敌预警)
            {

                float distance = 对象信息.敌人信息.距离;
                int team = 对象信息.敌人信息.队伍;

                // HSV转RGB函数
                auto GetTeamColor = [](int team_id) -> ImColor
                {
                    team_id = team_id % 50;                 // 确保队伍编号在0-49范围内
                    float hue = (team_id * 360.0f) / 50.0f; // 色相均匀分布
                    float sat = 0.8f, val = 0.9f;           // 适当降低饱和度和亮度提升辨识度

                    float r, g, b;
                    ImGui::ColorConvertHSVtoRGB(hue / 360.0f, sat, val, r, g, b);
                    return ImColor(r, g, b, 1.0f); // 返回不透明的基础颜色
                };

                // 计算透明度
                int alpha = 255;
                if (distance > 200.0f)
                {
                    alpha = 200; // 200米外半透明
                }
                else if (distance >= 100.0f)
                {
                    float ratio = (distance - 100.0f) / 100.0f;
                    alpha = 255 - static_cast<int>(ratio * 55); // 线性过渡透明度
                }

                // 组合最终颜色
                ImColor base_color = GetTeamColor(team);
                ImColor alert_color = ImColor(
                    static_cast<int>(base_color.Value.x * 255 + 0.5f),
                    static_cast<int>(base_color.Value.y * 255 + 0.5f),
                    static_cast<int>(base_color.Value.z * 255 + 0.5f),
                    alpha);

                // 调用绘制函数
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

            // 在不想吃鸡的函数中
            if (按钮.不想吃鸡)
            {
                if (自身数据.真人数量 <= 5 && 自身数据.真人数量 >= 0)
                {
                    LOGI("真人数量:%d，自动结束游戏", 自身数据.真人数量);

                    // 尝试使用完整路径
                    int result = system("/system/bin/am force-stop com.tencent.tmgp.pubgmhd");

                    // 备用方案
                    if (result != 0)
                    {
                        result = system("am force-stop com.tencent.tmgp.pubgmhd");
                    }

                    if (result != 0)
                    {
                        LOGI("结束游戏失败，错误码: %d", result);
                    }
                }
            }

            /*
            if (按钮.显示对局信息)
            {
                // 创建格式化字符串缓冲区
                static char buffer[3][64];

                // 设置颜色
                ImColor textColor = ImColor(255, 255, 255, 255);  // 白色文字
                ImColor outlineColor = ImColor(0, 0, 0, 255);    // 黑色描边

                // 真人数量
                snprintf(buffer[0], sizeof(buffer[0]), "剩余真人数量:%d", 自身数据.真人数量);
                ImVec2 textPos0 = ImVec2(PX / 10, 450);
                // 绘制描边（8方向偏移1像素）
                for (int x = -1; x <= 1; x++) {
                    for (int y = -1; y <= 1; y++) {
                        if (x != 0 || y != 0) {  // 不绘制中心点
                            ImGui::GetForegroundDrawList()->AddText(
                                ImVec2(textPos0.x + x, textPos0.y + y),
                                outlineColor,
                                buffer[0]
                            );
                        }
                    }
                }
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(textPos0, textColor, buffer[0]);

                // 人机数量
                snprintf(buffer[1], sizeof(buffer[1]), "剩余队伍数量:%d", 自身数据.人机数量);
                ImVec2 textPos1 = ImVec2(PX / 10, 490);
                // 绘制描边（8方向偏移1像素）
                for (int x = -1; x <= 1; x++) {
                    for (int y = -1; y <= 1; y++) {
                        if (x != 0 || y != 0) {  // 不绘制中心点
                            ImGui::GetForegroundDrawList()->AddText(
                                ImVec2(textPos1.x + x, textPos1.y + y),
                                outlineColor,
                                buffer[1]
                            );
                        }
                    }
                }
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(textPos1, textColor, buffer[1]);

                // 队伍数量
                snprintf(buffer[2], sizeof(buffer[2]), "剩余人数数量:%d", 自身数据.队伍数量);
                ImVec2 textPos2 = ImVec2(PX / 10, 530);
                // 绘制描边（8方向偏移1像素）
                for (int x = -1; x <= 1; x++) {
                    for (int y = -1; y <= 1; y++) {
                        if (x != 0 || y != 0) {  // 不绘制中心点
                            ImGui::GetForegroundDrawList()->AddText(
                                ImVec2(textPos2.x + x, textPos2.y + y),
                                outlineColor,
                                buffer[2]
                            );
                        }
                    }
                }
                // 绘制中心文字
                ImGui::GetForegroundDrawList()->AddText(textPos2, textColor, buffer[2]);
            }*/

            if (t_屏幕坐标.W >= 0)
            {
                /*    多余        if (按钮.头甲包显示 && 对象信息.敌人信息.实体列表地址 && 对象信息.敌人信息.实体数量 > 0 && 对象信息.敌人信息.实体数量 <= 10)
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
            */
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
    if (自瞄.初始化 && 自瞄.准星射线 && 自瞄.瞄准目标 != -1 && 自瞄函数[自瞄.瞄准目标].准心距离 <= 自瞄.自瞄范围)
    {
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(PX, PY), ImVec2(自瞄函数[自瞄.瞄准目标].对象骨骼.X, 自瞄函数[自瞄.瞄准目标].对象骨骼.Y), ImColor(255, 255, 255, 255), 2.1);
    }
    if (按钮.被瞄预警)
        绘图.绘制瞄准信息();
    if (按钮.人数)
        绘图.绘制人数(绘制人机, 绘制真人, 地址.自身地址);
    自瞄.瞄准总数量 = 自瞄.瞄准对象数量;
}

void 绘制::运行绘制()
{
    更新地址数据();
    更新对象数据();
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
        {503003, "三级防弹衣"}, {502003, "三级头盔"}, {501003, "三级包"}};

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
    static const std::unordered_map<int, const char *> 物品列表 = {
        {106094, "召回信号枪"}, {106120, "召回信号枪"}, {308005, "召回信号弹"}, {601006, "医疗箱"}, {601005, "急救包"}, {601004, "绷带"}, {601001, "可乐"}, {601002, "肾上腺素"}, {601003, "止痛药"}, {503003, "三级防弹衣"}, {502003, "三级头盔"}, {501003, "三级包"}, {302001, "7.62MM"}, {303001, "5.56MM"}, {301001, "9MM"}, {304001, "12口径"}, {305001, "45口径"}, {308001, "信号弹"}, {307001, "箭矢"}, {203001, "红点"}, {203014, "3倍镜"}, {203015, "6倍镜"}, {203005, "8倍镜"}, {204014, "子弹袋"}, {204010, "子弹袋"}, {205004, "箭袋"}, {202007, "激光瞄准器"}, {202004, "轻型握把"}, {202005, "半截握把"}, {205001, "UZI枪托"}, {205003, "狙击枪托"}, {205002, "步枪枪托"}, {201003, "狙击枪补偿器"}, {201005, "狙击枪消焰器"}, {201007, "狙击枪消音器"}, {201011, "步枪消音器"}, {201009, "步枪补偿器"}, {201010, "步枪消焰器"}, {201006, "冲锋枪消音器"}, {201004, "冲锋枪消焰器"}, {201002, "冲锋枪消焰器"}, {202006, "拇指握把"}, {202002, "垂直握把"}, {202001, "直角握把"}, {205011, "撞火枪托"}, {204017, "霰弹快速"}, {201012, "鸭嘴枪口"}, {201001, "霰弹收束"}, {204009, "狙击枪快扩"}, {204007, "狙击枪扩容"}, {204013, "步枪快扩"}, {204011, "步枪扩容"}, {204006, "冲锋枪快扩"}, {204004, "冲锋枪扩容"}, {602003, "燃烧瓶"}, {602002, "烟雾弹"}, {602001, "震爆弹"}, {602004, "手榴弹"}, {107001, "十字弩"}, {103003, "AWM"}, {103001, "Kar98K"}, {103002, "M24"}, {103006, "Mini14"}, {103008, "Win94"}, {103009, "SLR"}, {103012, "AMR狙击枪"}, {103013, "M417"}, {103014, "MK20-H"}, {103100, "MK12"}, {103010, "QBU"}, {103004, "SKS"}, {103007, "MK14"}, {103015, "M200"}, {103011, "莫辛狙击枪"}, {101012, "蜜獾"}, {101008, "M762"}, {101011, "VC-VAL突击步枪"}, {101010, "G36C步枪"}, {101013, "Famas突击步枪"}, {105002, "DP28"}, {101005, "狗砸"}, {101001, "AKM"}, {101007, "QBZ"}, {101009, "MK47"}, {101006, "AUG"}, {101002, "M16A4"}, {101003, "SCAR"}, {101014, "ACE-32"}, {101004, "M416"}, {105001, "M249"}, {105012, "PKM"}, {105010, "MG3"}, {102001, "UZI"}, {102004, "汤姆逊冲锋枪"}, {102003, "维克托"}, {102002, "UMP9"}, {102008, "AKS-74U"}, {102009, "JS9冲锋枪"}, {102005, "野牛冲锋枪"}, {102007, "MP5K"}, {102105, "P90冲锋枪"}, {106003, "R1895"}, {106006, "短管散弹枪"}, {104003, "S12K"}, {104002, "S1897"}, {104001, "双管猎枪"}, {104004, "DBS"}, {104100, "SPAS-12"}, {104005, "AA12-G"}, {9801001, "卧底证"}, {9801002, "能源电池"}, {9801003, "加密房间房卡"}, {9801004, "勘察情报"}, {9802001, "金库密码卡-白"}, {9802002, "金库密码卡-黄"}, {9802003, "金库密码卡-红"}, {9802004, "金库密码卡-绿"}, {9802005, "金库密码卡-黑"}, {9802006, "功勋奖章（铜）"}, {9802007, "功勋奖章（银）"}, {9802008, "功勋奖章（金）"}, {9802009, "撤离信号电话机"}, {9803001, "1级变卖物测试"}, {9803002, "2级变卖物测试"}, {9803003, "3级变卖物测试"}, {9803004, "4级变卖物测试"}, {9803005, "5级变卖物测试"}, {9803006, "6级变卖物测试"}, {9803007, "7级变卖物测试"}, {9803031, "信号发生器"}, {9803032, "净水器"}, {9803033, "CPU处理器"}, {9803034, "探测器"}, {9803035, "平板电脑"}, {9803036, "军用电池"}, {9803037, "军用电路板"}, {9803038, "精密仪器蓝图"}, {9803039, "绝密情报"}, {9803040, "设计图纸"}, {9803041, "显卡"}, {9803042, "镜头"}, {9803043, "珍稀古钱币"}, {9803044, "一根金条"}, {9803045, "光子鸡奖杯"}, {9803050, "装甲能源部件"}, {9803051, "装甲爆发部件"}, {9803052, "紫矿碎片"}, {9803053, "紫矿晶块"}, {9803054, "稀有宝石"}, {9803055, "黄金烤鸡"}, {9803056, "收发信器"}, {9803057, "钛合金"}, {9803058, "制动引擎"}, {9803059, "胶卷"}, {9803060, "拉刀"}, {9803061, "打火机"}, {9803062, "黑胶唱片"}, {9803063, "窃听装置"}, {9803064, "摄像头"}, {9803065, "雷管"}, {9803066, "黑鹰勋章"}, {9803067, "金狼雕像"}, {9803068, "密信"}, {9803069, "紫色试管药剂"}, {9803087, "附加奖励金龙"}, {9804007, "5级头盔"}, {9804008, "5级头盔(铁爪)"}, {9804009, "5级头盔(黑鹰)"}, {9804010, "6级头盔"}, {9804011, "6级头盔(铁爪)"}, {9804012, "6级头盔(黑鹰)"}, {9804013, "7级头盔"}, {9804014, "7级头盔(铁爪)"}, {9804015, "7级头盔(黑鹰)"}, {9804016, "头戴夜视仪"}, {9804017, "7级头盔"}, {9804018, "7级头盔(铁爪)"}, {9804019, "7级头盔(黑鹰)"}, {9804021, "4级头盔(轩辕)"}, {9804022, "5级头盔(轩辕)"}, {9804023, "6级头盔(轩辕)"}, {9804024, "7级头盔(轩辕)"}, {9804025, "7级头盔(轩辕)"}, {9804026, "7级头盔·墨守"}, {9805010, "6级防弹衣"}, {9805011, "6级防弹衣(铁爪)"}, {9805012, "6级防弹衣(黑鹰)"}, {9805013, "7级防弹衣"}, {9805014, "7级防弹衣(铁爪)"}, {9805015, "7级防弹衣(黑鹰)"}, {9805016, "7级防弹衣"}, {9805017, "7级防弹衣(铁爪)"}, {9805018, "7级防弹衣(黑鹰)"}, {9805020, "机甲防具"}, {9805039, "7级防弹衣(轩辕)"}, {9805040, "7级防弹衣(轩辕)"}, {9805094, "7级防弹衣·特劳斯(强化)"}, {9805096, "7级防弹衣·特劳斯"}, {9805099, "7级防弹衣·特劳斯"}, {9806010, "6级背包"}, {9806011, "6级背包(铁爪)"}, {9806012, "6级背包(黑鹰)"}, {9806013, "7级背包"}, {9806014, "7级背包(铁爪)"}, {9806015, "7级背包(黑鹰)"}, {9806016, "7级背包"}, {9806017, "7级背包(铁爪)"}, {9806018, "7级背包(黑鹰)"}, {9806021, "6级背包(轩辕)"}, {9806022, "7级背包(轩辕)"}, {9806023, "7级背包(轩辕)"}, {9803070, "数据芯片"}};

    if (auto it = 物品列表.find(id); it != 物品列表.end())
    {
        return it->second;
    }
    return "NULL";
}
