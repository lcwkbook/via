#include "Updater.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <thread>
#include <chrono>

// ---------- 工具函数 ----------

static bool execCmd(const std::string &cmd) {
    return (system(cmd.c_str()) == 0);
}

static bool commandExists(const std::string &cmd) {
    std::string check = "which " + cmd + " > /dev/null 2>&1";
    return execCmd(check);
}

static bool fileValid(const std::string &path) {
    struct stat st;
    return (stat(path.c_str(), &st) == 0 && st.st_size > 0);
}

static std::string getSelfExe() {
    char buf[1024] = {0};
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len != -1) {
        buf[len] = '\0';
        return std::string(buf);
    }
    return "";
}

static bool isNewer(const std::string &remote, const std::string &local) {
    int r[4] = {0}, l[4] = {0};
    sscanf(remote.c_str(), "%d.%d.%d.%d", &r[0], &r[1], &r[2], &r[3]);
    sscanf(local.c_str(), "%d.%d.%d.%d", &l[0], &l[1], &l[2], &l[3]);
    for (int i = 0; i < 4; ++i) {
        if (r[i] > l[i]) return true;
        if (r[i] < l[i]) return false;
    }
    return false;
}

// 组装下载命令，优先 curl，带进度条显示（单行刷新，不刷屏）
static std::string getDownloadCmd(const std::string &url, const std::string &outFile) {
    if (commandExists("curl")) {
        // -# : 显示进度条（# 符号）
        // -L : 跟随重定向
        // -o : 输出到文件
        return "curl -# -L -o \"" + outFile + "\" \"" + url + "\"";
    } else if (commandExists("wget")) {
        // --show-progress : 显示进度条
        // -O : 输出到文件
        return "wget --show-progress -O \"" + outFile + "\" \"" + url + "\"";
    } else {
        return "";
    }
}

// ---------- 核心更新函数 ----------

void StartUpdate(const std::string &serverVersion,
                 const std::string &downloadUrl,
                 const std::string &mustUpdate) {
    // ***** 重要：每次发版前修改此版本号为你当前程序的真实版本 *****
    const std::string currentVersion = "1.36.4.64";

    if (!isNewer(serverVersion, currentVersion)) {
        std::cout << "[+] 已是最新版本" << std::endl;
        return;
    }

    std::cout << "========================================\n";
    std::cout << "  发现新版本: " << serverVersion << "\n";
    std::cout << "  当前版本: " << currentVersion << "\n";
    std::cout << "  下载地址: " << downloadUrl << "\n";
    std::cout << "========================================\n";

    if (mustUpdate != "y") {
        std::cout << "是否立即更新？(1-是 / 0-否): ";
        int choice;
        std::cin >> choice;
        if (choice != 1) {
            std::cout << "[*] 已取消更新\n";
            return;
        }
    } else {
        std::cout << "[!] 本次为强制更新，即将自动下载并安装...\n";
    }

    // ---------- 下载 ----------
    std::string downloadCmd = getDownloadCmd(downloadUrl, "/data/local/tmp/aura_update.tmp");
    if (downloadCmd.empty()) {
        std::cerr << "[!] 错误：设备上没有 curl 或 wget，无法下载更新。" << std::endl;
        if (mustUpdate == "y") exit(1);
        return;
    }

    std::cout << "[*] 正在下载更新包..." << std::endl;
    bool downloadOK = execCmd(downloadCmd);
    if (!downloadOK || !fileValid("/data/local/tmp/aura_update.tmp")) {
        std::cerr << "\n[!] 下载失败，请检查网络或下载地址。" << std::endl;
        execCmd("rm -f /data/local/tmp/aura_update.tmp");
        if (mustUpdate == "y") exit(1);
        return;
    }
    std::cout << "\n[+] 下载完成。" << std::endl;

    // ---------- 替换自身 ----------
    std::string selfExe = getSelfExe();
    if (selfExe.empty()) {
        std::cerr << "[!] 无法获取自身路径，放弃更新。" << std::endl;
        if (mustUpdate == "y") exit(1);
        return;
    }

    // 备份原文件
    execCmd("cp -f \"" + selfExe + "\" /data/local/tmp/aura_update.bak");

    // 若在 system 分区，挂载为可写
    if (selfExe.find("/system") == 0) {
        execCmd("mount -o remount,rw /system");
    }

    std::string replaceCmd = "cp -f /data/local/tmp/aura_update.tmp \"" + selfExe + "\"";
    if (!execCmd(replaceCmd)) {
        std::cerr << "[!] 替换文件失败，尝试恢复备份..." << std::endl;
        execCmd("cp -f /data/local/tmp/aura_update.bak \"" + selfExe + "\"");
        execCmd("rm -f /data/local/tmp/aura_update.tmp /data/local/tmp/aura_update.bak");
        if (mustUpdate == "y") exit(1);
        return;
    }

    // 设置 777 权限
    execCmd("chmod 777 \"" + selfExe + "\"");

    // 清理临时文件
    execCmd("rm -f /data/local/tmp/aura_update.tmp /data/local/tmp/aura_update.bak");

    std::cout << "[+] 更新成功！" << std::endl;

    if (mustUpdate == "y") {
        std::cout << "[*] 程序即将退出，请重新启动。" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        exit(0);
    } else {
        std::cout << "[*] 更新已完成，下次启动生效。\n";
    }
}