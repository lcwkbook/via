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

// ---------- 核心更新函数（美化版） ----------
void StartUpdate(const std::string &currentVersion,
                 const std::string &serverVersion,
                 const std::string &downloadUrl,
                 const std::string &mustUpdate,
                 const std::string &customTargetPath) {

    if (!isNewer(serverVersion, currentVersion)) {
        printf("  \033[1;32m  ✔ 当前已是最新版本: %s\033[0m\n\n", currentVersion.c_str());
        return;
    }

    printf("\n");
    printf("  \033[1;33m┌──────────────────────────────────────────┐\033[0m\n");
    printf("  \033[1;33m│  🆕 发现新版本！\033[0m                              │\n");
    printf("  \033[1;33m├──────────────────────────────────────────┤\033[0m\n");
    printf("  \033[1;33m│  当前版本: %-32s\033[0m│\n", currentVersion.c_str());
    printf("  \033[1;33m│  最新版本: %-32s\033[0m│\n", serverVersion.c_str());
    printf("  \033[1;33m└──────────────────────────────────────────┘\033[0m\n\n");

    // ---------- 用户确认（非强制时） ----------
    if (mustUpdate != "y") {
        printf("  \033[1;33m  ⮕ 是否立即更新？[1-是 / 0-否]: \033[0m");
        fflush(stdout);
        int choice;
        std::cin >> choice;
        if (choice != 1) {
            printf("  \033[1;33m  ⮕ 已取消更新，继续使用当前版本\033[0m\n\n");
            return;
        }
    } else {
        printf("  \033[1;31m  ⚠ 该版本为强制更新！\033[0m\n");
        printf("  \033[1;34m  ⏳ 即将自动下载并安装...\033[0m\n\n");
    }

    // ---------- 决定真正的目标文件路径 ----------
    std::string targetFile;

    if (!customTargetPath.empty()) {
        targetFile = customTargetPath;
    } else {
        std::string selfExe = getSelfExe();
        if (selfExe.empty()) {
            printf("  \033[1;31m  ✘ 无法获取自身路径，放弃更新。\033[0m\n");
            if (mustUpdate == "y") exit(1);
            return;
        }

        size_t lastSlash = selfExe.find_last_of("/");
        if (lastSlash == std::string::npos) {
            printf("  \033[1;31m  ✘ 无法解析程序目录，放弃更新。\033[0m\n");
            if (mustUpdate == "y") exit(1);
            return;
        }
        targetFile = selfExe.substr(0, lastSlash) + "/Aurakernel.sh";
    }

    printf("  \033[1;34m  📦 目标更新文件: %s\033[0m\n", targetFile.c_str());

    // ---------- 下载 ----------
    const std::string tmpFile = "/data/local/tmp/aura_update.tmp";
    std::string downloadCmd = getDownloadCmd(downloadUrl, tmpFile);
    if (downloadCmd.empty()) {
        printf("  \033[1;31m  ✘ 设备上没有 curl 或 wget，无法下载更新。\033[0m\n");
        if (mustUpdate == "y") exit(1);
        return;
    }

    printf("  \033[1;34m  ⏳ 正在下载更新包...\033[0m\n");
    fflush(stdout);

    bool downloadOK = execCmd(downloadCmd);
    if (!downloadOK || !fileValid(tmpFile)) {
        printf("\n  \033[1;31m  ✘ 下载失败（网络异常或文件不存在），请检查地址。\033[0m\n");
        execCmd("rm -f " + tmpFile);
        if (mustUpdate == "y") exit(1);
        return;
    }
    printf("  \033[1;32m  ✔ 下载完成！\033[0m\n");

    // ---------- 备份与替换 ----------
    if (fileValid(targetFile)) {
        printf("  \033[1;33m  ⚠ 检测到旧版本，正在备份...\033[0m\n");
        std::string backupCmd = "cp -f \"" + targetFile + "\" \"" + targetFile + ".bak\"";
        execCmd(backupCmd);
    }

    // system 分区特殊处理
    if (targetFile.find("/system") == 0) {
        printf("  \033[1;33m  ⚠ 目标位于系统分区，正在挂载可写...\033[0m\n");
        execCmd("mount -o remount,rw /system");
    }

    printf("  \033[1;34m  ⏳ 正在替换文件...\033[0m\n");
    std::string replaceCmd = "cp -f \"" + tmpFile + "\" \"" + targetFile + "\"";
    if (!execCmd(replaceCmd)) {
        printf("  \033[1;31m  ✘ 替换文件失败，尝试恢复备份...\033[0m\n");
        execCmd("cp -f \"" + targetFile + ".bak\" \"" + targetFile + "\" 2>/dev/null");
        execCmd("rm -f " + tmpFile + " " + targetFile + ".bak");
        if (mustUpdate == "y") exit(1);
        return;
    }

    // 设置可执行权限
    execCmd("chmod 777 \"" + targetFile + "\"");

    // 清理临时文件
    execCmd("rm -f " + tmpFile);
    execCmd("rm -f \"" + targetFile + ".bak\"");

    printf("  \033[1;32m  ✔ 更新成功！\033[0m\n");

    // ---------- 判断是否需要删除当前运行文件 ----------
    std::string selfExe = getSelfExe();
    if (!selfExe.empty() && selfExe == targetFile) {
        printf("  \033[1;36m  ℹ 当前运行文件即目标文件，跳过自我删除。\033[0m\n");
    } else {
        printf("  \033[1;36m  ℹ 旧版本文件已自动被覆盖为最新版。\033[0m\n");
    }

    // ---------- 退出提示 ----------
    if (mustUpdate == "y") {
        printf("\n  \033[1;31m┌──────────────────────────────────────────┐\033[0m\n");
        printf("  \033[1;31m│  🔄 程序即将退出，请重新启动！\033[0m              │\n");
        printf("  \033[1;31m└──────────────────────────────────────────┘\033[0m\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        exit(0);
    } else {
        printf("\n  \033[1;32m┌──────────────────────────────────────────┐\033[0m\n");
        printf("  \033[1;32m│  ✅ 更新已完成，请重启程序以使用新版本！\033[0m       │\n");
        printf("  \033[1;32m└──────────────────────────────────────────┘\033[0m\n\n");
    }
}
