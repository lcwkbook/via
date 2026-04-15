#include <linux/input.h>
//#include "悬浮窗.cpp"

static bool 悬浮窗 = true;

// 补充必要头文件（必须保留，否则编译报错）
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/input.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>

// 全局悬浮窗状态（替换原中文变量，避免语法错误）
bool g_FloatWindow = false;
// 全局运行标志（解决无限循环无法退出问题）
bool g_IsRunning = true;

// 猎鹰监测核心函数（修复原逻辑的语法/资源问题）
void 猎鹰() {
    // 仅初始化一次随机数种子（原代码每次循环初始化会导致随机数重复）
    srand(static_cast<unsigned int>(time(nullptr)));
    const char* dirPath = "/data/猎鹰判断";

    // 创建目录，避免文件写入时的权限/目录不存在错误
    if (mkdir(dirPath, 0777) == -1 && errno != EEXIST) {
        return;
    }

    while (g_IsRunning) {
        int random_number = rand() % 100;

        if (random_number < 1) {
            std::ofstream file("/data/猎鹰判断/监测到猎鹰巡查正在观战...");
            if (file.is_open()) {
                file.close();
            }
        }

        // 原500s休眠保留，如需测试可临时改为5s
        sleep(500);
    }
}

// 输入设备数量获取（原“数据”函数，修复资源泄漏）
int 数据() {
    DIR *dir = opendir("/dev/input/");
    if (!dir) {
        return -1;
    }

    dirent *ptr = NULL;
    int count = 0;
    while ((ptr = readdir(dir)) != NULL) {
        if (strstr(ptr->d_name, "event"))
            count++;
    }

    closedir(dir); // 释放目录句柄，避免资源泄漏
    return count ? count : -1;
}

// 音量键监听核心函数（原“音量”函数，解决驱动冲突/语法/资源问题）
int 音量() {
    int EventCount = 数据();
    if (EventCount < 0) {
        printf("未找到输入设备\n");
        return -1;
    }

    int *fdArray = (int *)malloc(EventCount * sizeof(int));
    if (!fdArray) { // 内存分配失败判断，避免崩溃
        return -1;
    }

    // 打开输入设备：改用只读模式，避免和驱动抢占读写权限
    for (int i = 0; i < EventCount; i++) {
        char temp[128];
        sprintf(temp, "/dev/input/event%d", i);
        fdArray[i] = open(temp, O_RDONLY | O_NONBLOCK);
    }

    input_event ev;

    while (g_IsRunning) {
        for (int i = 0; i < EventCount; i++) {
            if (fdArray[i] < 0) continue; // 跳过打开失败的设备

            memset(&ev, 0, sizeof(ev));
            ssize_t ret = read(fdArray[i], &ev, sizeof(ev));
            if (ret != sizeof(ev) || ev.type != EV_KEY) continue;

            // 处理音量键按下事件，保留原逻辑的键值判断
            if (ev.value == 1) {
                if (ev.code == 115) { // 音量+
                    g_FloatWindow = true;
                } else if (ev.code == 114) { // 音量-
                    g_FloatWindow = false;
                }
            }
        }
        usleep(20000); // 优化休眠时间，降低CPU占用，避免驱动检测异常
    }

    // 资源释放：避免驱动层文件描述符泄漏
    for (int i = 0; i < EventCount; i++) {
        if (fdArray[i] >= 0) close(fdArray[i]);
    }
    free(fdArray);

    return 0;
}

// 可选：程序退出时调用，用于停止所有循环（需在主逻辑中触发）
void StopAllModules() {
    g_IsRunning = false;
}
