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

// 版本比较（四段数字 a.b.c.d）
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

// 组装下载命令（-f 使 curl 在 HTTP 错误时失败）
static std::string getDownloadCmd(const std::string &url, const std::string &outFile) {
    if (commandExists("curl")) {
        // -f : fail on HTTP errors (如 404)
        // -# : 进度条
        // -L : 跟随重定向
        // -o : 输出文件
        return "curl -f -# -L -o \"" + outFile + "\" \"" + url + "\"";
    } else if (commandExists("wget")) {
        // wget 遇到 404 默认返回非零，无需额外参数
        return "wget --show-progress -O \"" + outFile + "\" \"" + url + "\"";
    } else {
        return "";
    }
}

// ---------- 核心更新函数 ----------

void StartUpdate(const std::string &currentVersion,
                 const std::string &serverVersion,
                 const std::string &downloadUrl,
                 const std::string &mustUpdate) {

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
        std::cerr << "\n[!] 下载失败（网络异常或文件不存在），请检查地址。" << std::endl;
        execCmd("rm -f /data/local/tmp/aura_update.tmp");
        if (mustUpdate == "y") exit(1);
        return;
    }
    std::cout << "\n[+] 下载完成。" << std::endl;

    // ---------- 替换自身（固定为 Aurakernel.sh + 删除旧文件） ----------
    std::string selfExe = getSelfExe();
    if (selfExe.empty()) {
        std::cerr << "[!] 无法获取自身路径，放弃更新。" << std::endl;
        if (mustUpdate == "y") exit(1);
        return;
    }

    // 提取程序目录，生成固定目标文件名 Aurakernel.sh
    size_t lastSlash = selfExe.find_last_of("/");
    if (lastSlash == std::string::npos) {
        std::cerr << "[!] 无法解析程序目录，放弃更新。" << std::endl;
        if (mustUpdate == "y") exit(1);
        return;
    }
    std::string selfDir = selfExe.substr(0, lastSlash);
    std::string targetFile = selfDir + "/Aurakernel.sh"; // 固定新文件名
    std::cout << "[*] 目标更新文件：" << targetFile << std::endl;

    // 备份旧的 Aurakernel.sh
    execCmd("cp -f \"" + targetFile + "\" /data/local/tmp/aura_update.bak 2>/dev/null");

    // system 分区挂载可写
    if (selfDir.find("/system") == 0) {
        execCmd("mount -o remount,rw /system");
    }

    // 替换为新版本
    std::string replaceCmd = "cp -f /data/local/tmp/aura_update.tmp \"" + targetFile + "\"";
    if (!execCmd(replaceCmd)) {
        std::cerr << "[!] 替换文件失败，尝试恢复备份..." << std::endl;
        execCmd("cp -f /data/local/tmp/aura_update.bak \"" + targetFile + "\" 2>/dev/null");
        execCmd("rm -f /data/local/tmp/aura_update.tmp /data/local/tmp/aura_update.bak");
        if (mustUpdate == "y") exit(1);
        return;
    }

    // 设置可执行权限
    execCmd("chmod 777 \"" + targetFile + "\"");

    // ====================== 核心新增：删除原旧版本文件 ======================
    std::cout << "[*] 正在清理旧版本文件：" << selfExe << std::endl;
    execCmd("rm -f \"" + selfExe + "\" 2>/dev/null"); // 安全删除旧文件
    // ======================================================================

    // 清理临时文件
    execCmd("rm -f /data/local/tmp/aura_update.tmp /data/local/tmp/aura_update.bak");

    std::cout << "[+] 更新成功！新版本：" << targetFile << std::endl;
    std::cout << "[+] 旧版本文件已自动删除" << std::endl;

    if (mustUpdate == "y") {
        std::cout << "[*] 程序即将退出，请重新启动 Aurakernel.sh。" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        exit(0);
    } else {
        std::cout << "[*] 更新已完成，直接运行 Aurakernel.sh 即可使用新版本。\n";
    }
}