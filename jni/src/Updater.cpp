#include "Updater.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <thread>
#include <chrono>

// ---------- 工具函数（你已有的） ----------
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

static std::string getDownloadCmd(const std::string &url, const std::string &outFile) {
    if (commandExists("curl")) {
        return "curl -f -# -L -o \"" + outFile + "\" \"" + url + "\"";
    } else if (commandExists("wget")) {
        return "wget --show-progress -O \"" + outFile + "\" \"" + url + "\"";
    }
    return "";
}

// ---------- 核心更新函数（完整版） ----------
void StartUpdate(const std::string &currentVersion,
                 const std::string &serverVersion,
                 const std::string &downloadUrl,
                 const std::string &mustUpdate,
                 const std::string &customTargetPath) {

    if (!isNewer(serverVersion, currentVersion)) {
        std::cout << "[+] 已是最新版本" << std::endl;
        return;
    }

    std::cout << "========================================\n";
    std::cout << "  发现新版本: " << serverVersion << "\n";
    std::cout << "  当前版本: " << currentVersion << "\n";
    std::cout << "  下载地址: " << downloadUrl << "\n";
    std::cout << "========================================\n";

    // ---------- 用户确认（非强制时） ----------
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

    // ---------- 决定真正的目标文件路径 ----------
    std::string targetFile;

    if (!customTargetPath.empty()) {
        // 外部指定了真实路径（如通过环境变量传入），直接使用
        targetFile = customTargetPath;
    } else {
        // 未指定，则自动检测：使用当前进程所在目录
        std::string selfExe = getSelfExe();
        if (selfExe.empty()) {
            std::cerr << "[!] 无法获取自身路径，放弃更新。" << std::endl;
            if (mustUpdate == "y") exit(1);
            return;
        }

        size_t lastSlash = selfExe.find_last_of("/");
        if (lastSlash == std::string::npos) {
            std::cerr << "[!] 无法解析程序目录，放弃更新。" << std::endl;
            if (mustUpdate == "y") exit(1);
            return;
        }
        targetFile = selfExe.substr(0, lastSlash) + "/Aurakernel.sh";
    }

    std::cout << "[*] 目标更新文件：" << targetFile << std::endl;

    // ---------- 下载 ----------
    const std::string tmpFile = "/data/local/tmp/aura_update.tmp";
    std::string downloadCmd = getDownloadCmd(downloadUrl, tmpFile);
    if (downloadCmd.empty()) {
        std::cerr << "[!] 错误：设备上没有 curl 或 wget，无法下载更新。" << std::endl;
        if (mustUpdate == "y") exit(1);
        return;
    }

    std::cout << "[*] 正在下载更新包..." << std::endl;
    bool downloadOK = execCmd(downloadCmd);
    if (!downloadOK || !fileValid(tmpFile)) {
        std::cerr << "\n[!] 下载失败（网络异常或文件不存在），请检查地址。" << std::endl;
        execCmd("rm -f " + tmpFile);
        if (mustUpdate == "y") exit(1);
        return;
    }
    std::cout << "\n[+] 下载完成：" << tmpFile << std::endl;

    // ---------- 备份与替换 ----------
    // 如果目标文件已存在，先备份
    if (fileValid(targetFile)) {
        std::string backupCmd = "cp -f \"" + targetFile + "\" \"" + targetFile + ".bak\"";
        execCmd(backupCmd);
    }

    // system 分区特殊处理（若目标在 /system 下）
    if (targetFile.find("/system") == 0) {
        execCmd("mount -o remount,rw /system");
    }

    // 将下载好的文件复制到目标路径
    std::string replaceCmd = "cp -f \"" + tmpFile + "\" \"" + targetFile + "\"";
    if (!execCmd(replaceCmd)) {
        std::cerr << "[!] 替换文件失败，尝试恢复备份..." << std::endl;
        execCmd("cp -f \"" + targetFile + ".bak\" \"" + targetFile + "\" 2>/dev/null");
        execCmd("rm -f " + tmpFile + " " + targetFile + ".bak");
        if (mustUpdate == "y") exit(1);
        return;
    }

    // 设置可执行权限
    execCmd("chmod 777 \"" + targetFile + "\"");

    // 清理临时文件
    execCmd("rm -f " + tmpFile);
    execCmd("rm -f \"" + targetFile + ".bak\"");  // 成功则删除备份

    std::cout << "[+] 更新成功！新版本：" << targetFile << std::endl;

    // ---------- 判断是否需要删除当前运行文件 ----------
    std::string selfExe = getSelfExe();
    if (!selfExe.empty() && selfExe == targetFile) {
        // 当前运行的就是目标文件本身，绝对不能删除！
        std::cout << "[*] 当前运行文件即目标文件，跳过自我删除。" << std::endl;
    } else {
        // 当前运行的是临时副本或其他路径，目标文件已经替换好，
        // 无需删除任何东西（临时副本将在进程退出后由运行器或系统清理）
        std::cout << "[*] 旧版本文件已自动被覆盖为最新版。" << std::endl;
    }

    // ---------- 退出提示 ----------
    if (mustUpdate == "y") {
        std::cout << "[*] 程序即将退出，请重新启动 Aurakernel.sh。" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        exit(0);
    } else {
        std::cout << "[*] 更新已完成，直接运行 Aurakernel.sh 即可使用新版本。\n";
    }
}