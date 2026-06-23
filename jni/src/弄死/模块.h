#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <random>
#include <vector>
#include <cstdlib>
#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
#include <cctype>
#include <unistd.h>

#include "src/模块/dev/dev-ioctl-4.14.117.h"
#include "src/模块/dev/dev-ioctl-4.14.141.h"
#include "src/模块/dev/dev-ioctl-4.14.180.h"
#include "src/模块/dev/dev-ioctl-4.14.186.h"
#include "src/模块/dev/dev-ioctl-4.19.81.h"
#include "src/模块/dev/dev-ioctl-4.19.113.h"
#include "src/模块/dev/dev-ioctl-4.19.157.h"
#include "src/模块/dev/dev-ioctl-4.19.157coloros.h"
#include "src/模块/dev/dev-ioctl-4.19.191.h"
#include "src/模块/dev/dev-ioctl-5.4.61.h"
#include "src/模块/dev/dev-ioctl-5.4.86.h"
#include "src/模块/dev/dev-ioctl-5.4.147.h"
#include "src/模块/dev/dev-ioctl-5.4.147coloros.h"
#include "src/模块/dev/dev-ioctl-5.10.h"
#include "src/模块/dev/dev-ioctl-5.15.h"
#include "src/模块/dev/dev-ioctl-6.1.h"
#include "src/模块/dev/dev-ioctl-6.6.h"

using namespace std;

inline bool ms_isDirectory(const std::string &path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return S_ISDIR(info.st_mode);
}

inline std::string ms_getRandomDirectory(const std::string &path) {
    std::vector<std::string> directories;
    DIR *dir = opendir(path.c_str());
    if (!dir) {
        return path;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string fullPath = path + "/" + entry->d_name;
        if (entry->d_type == DT_DIR &&
            std::string(entry->d_name) != "." &&
            std::string(entry->d_name) != ".." &&
            ms_isDirectory(fullPath)) {
            directories.push_back(fullPath);
        }
    }
    closedir(dir);
    if (directories.empty()) {
        return path;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(directories.size()) - 1);
    return directories[dis(gen)];
}

inline std::string ms_generateRandomString(size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const size_t maxIndex = sizeof(charset) - 1;
    std::default_random_engine engine(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<size_t> dist(0, maxIndex - 1);
    std::string randomString;
    randomString.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        randomString += charset[dist(engine)];
    }
    return randomString;
}

inline bool ms_hexStringToFile(const string &hexData, const string &outputFilename) {
    ofstream outputFile(outputFilename, ios::binary);
    if (!outputFile) {
        return false;
    }
    for (size_t i = 0; i + 1 < hexData.length(); i += 2) {
        string byteStr = hexData.substr(i, 2);
        char byte = static_cast<char>(stoi(byteStr, nullptr, 16));
        outputFile.put(byte);
    }
    outputFile.close();
    return true;
}

inline bool ms_isColorOS() {
    char buffer[256];
    FILE *fp = popen("getprop ro.build.display.id", "r");
    if (fp) {
        if (fgets(buffer, sizeof(buffer), fp)) {
            std::string val(buffer);
            for (char &c : val) {
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
            pclose(fp);
            if (val.find("coloros") != std::string::npos ||
                val.find("realmeui") != std::string::npos ||
                val.find("oppo") != std::string::npos) {
                return true;
            }
        } else {
            pclose(fp);
        }
    }
    fp = popen("getprop ro.build.version.opporom", "r");
    if (fp) {
        if (fgets(buffer, sizeof(buffer), fp)) {
            std::string val(buffer);
            for (char &c : val) {
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
            pclose(fp);
            if (val.find("coloros") != std::string::npos) {
                return true;
            }
        } else {
            pclose(fp);
        }
    }
    return false;
}

inline bool 模块刷入() {
    printf("\n\033[1;36m");
    printf("[Aura驱动------自动刷入\n]");
    printf("\033[0m");

    char buffer[256];
    FILE *fp = popen("uname -r", "r");
    if (!fp) {
        printf("\033[1;31m[!] 无法获取内核版本信息\n\033[0m");
        return false;
    }
    if (!fgets(buffer, sizeof(buffer), fp)) {
        pclose(fp);
        printf("\033[1;31m[!] 读取内核版本失败\n\033[0m");
        return false;
    }
    buffer[strcspn(buffer, "\n")] = 0;
    pclose(fp);

    printf("\033[1;33m[内核版本] %s\033[0m\n", buffer);

    std::string randomDir = ms_getRandomDirectory("/data/user/0");
    randomDir += "/";
    randomDir += ms_generateRandomString(40);

    bool isColor = ms_isColorOS();
    std::string insmodCommand;
    bool ok = false;

    if (strncmp(buffer, "5.10", 4) == 0) {
        printf("\033[36m[-] 检测到 5.10 内核, 正在刷入Aura驱动...\033[0m\n");
        ok = ms_hexStringToFile(dev_ioctl_510, randomDir);
    } else if (strncmp(buffer, "6.1", 3) == 0) {
        printf("\033[36m[-] 检测到 6.1 内核, 正在刷入Aura驱动...\033[0m\n");
        ok = ms_hexStringToFile(dev_ioctl_61, randomDir);
    } else if (strncmp(buffer, "6.6", 3) == 0) {
        printf("\033[36m[-] 检测到 6.6 内核, 正在刷入Aura驱动...\033[0m\n");
        ok = ms_hexStringToFile(dev_ioctl_66, randomDir);
    } else if (strncmp(buffer, "5.15", 4) == 0) {
        printf("\033[36m[-] 检测到 5.15 内核, 正在刷入Aura驱动...\033[0m\n");
        ok = ms_hexStringToFile(dev_ioctl_515, randomDir);
    } else if (strncmp(buffer, "5.4.", 4) == 0) {
        if (strncmp(buffer, "5.4.61", 6) == 0) {
            printf("\033[36m[-] 检测到 5.4.61 内核, 正在刷入Aura驱动...\033[0m\n");
            ok = ms_hexStringToFile(dev_ioctl_5461, randomDir);
        } else if (strncmp(buffer, "5.4.86", 6) == 0) {
            printf("\033[36m[-] 检测到 5.4.86 内核, 正在刷入Aura驱动...\033[0m\n");
            ok = ms_hexStringToFile(dev_ioctl_5486, randomDir);
        } else if (strncmp(buffer, "5.4.147", 7) == 0) {
            if (isColor) {
                printf("\033[36m[-] 检测到 5.4.147 ColorOS 内核, 正在刷入Aura驱动...\033[0m\n");
                ok = ms_hexStringToFile(dev_ioctl_54147coloros, randomDir);
            } else {
                printf("\033[36m[-] 检测到 5.4.147 内核, 正在刷入Aura驱动...\033[0m\n");
                ok = ms_hexStringToFile(dev_ioctl_54147, randomDir);
            }
        } else {
            printf("\033[1;31m[!] 当前 5.4 内核小版本未适配\n\033[0m");
            return false;
        }
    } else if (strncmp(buffer, "4.19.157", 8) == 0) {
        if (isColor) {
            printf("\033[36m[-] 检测到 4.19.157 ColorOS 内核, 正在刷入Aura驱动...\033[0m\n");
            ok = ms_hexStringToFile(dev_ioctl_419157coloros, randomDir);
        } else {
            printf("\033[36m[-] 检测到 4.19.157 内核, 正在刷入Aura驱动...\033[0m\n");
            ok = ms_hexStringToFile(dev_ioctl_419157, randomDir);
        }
    } else if (strncmp(buffer, "4.19.191", 8) == 0) {
        printf("\033[36m[-] 检测到 4.19.191 内核, 正在刷入Aura驱动...\033[0m\n");
        ok = ms_hexStringToFile(dev_ioctl_419191, randomDir);
    } else if (strncmp(buffer, "4.19.113", 8) == 0) {
        printf("\033[36m[-] 检测到 4.19.113 内核, 正在刷入Aura驱动...\033[0m\n");
        ok = ms_hexStringToFile(dev_ioctl_419113, randomDir);
    } else if (strncmp(buffer, "4.19.81", 7) == 0) {
        printf("\033[36m[-] 检测到 4.19.81 内核, 正在刷入Aura驱动...\033[0m\n");
        ok = ms_hexStringToFile(dev_ioctl_41981, randomDir);
    } else if (strncmp(buffer, "4.14.186", 8) == 0) {
        printf("\033[36m[-] 检测到 4.14.186 内核, 正在刷入Aura驱动...\033[0m\n");
        ok = ms_hexStringToFile(dev_ioctl_414186, randomDir);
    } else if (strncmp(buffer, "4.14.180", 8) == 0) {
        printf("\033[36m[-] 检测到 4.14.180 内核, 正在刷入Aura驱动...\033[0m\n");
        ok = ms_hexStringToFile(dev_ioctl_414180, randomDir);
    } else if (strncmp(buffer, "4.14.141", 8) == 0) {
        printf("\033[36m[-] 检测到 4.14.141 内核, 正在刷入Aura驱动...\033[0m\n");
        ok = ms_hexStringToFile(dev_ioctl_414141, randomDir);
    } else if (strncmp(buffer, "4.14.117", 8) == 0) {
        printf("\033[36m[-] 检测到 4.14.117 内核, 正在刷入Aura驱动...\033[0m\n");
        ok = ms_hexStringToFile(dev_ioctl_414117, randomDir);
    } else {
        printf("\033[1;31m[!] 未找到适合的Aura驱动...\n\033[0m");
        return false;
    }

    if (!ok) {
        printf("\033[1;31m[!] 写入临时模块文件失败\n\033[0m");
        return false;
    }

    std::string cmd = "insmod " + randomDir + " > /dev/null 2>&1";
    int ret = system(cmd.c_str());
    std::remove(randomDir.c_str());
    if (ret != 0) {
        printf("\033[1;31m[!] insmod 执行失败, 返回值: %d\n\033[0m", ret);
        return false;
    }

    printf("\033[1;32m[+] Aura驱动刷入完成 ; 正在等待驱动初始化...\033[0m\n");
    sleep(1);
    return true;
}
