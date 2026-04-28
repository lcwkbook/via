#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <fstream>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <iostream>
#include<ctime>
#include "Draw.h"
#include "辅助类.h"
#include "图片调用.h"
#include "弄死/模块.h"
#include <sys/utsname.h>
#include <thread>
#include <chrono>
#include <sys/types.h>
#include <sstream>
using namespace std;
extern int g_driver_mode;
// 全局变量
int abs_ScreenX, abs_ScreenY;
int 无后台;
string Wht, FlP;
int ZM;
布局 布局;
绘制 绘制;

int main() {
printf("\n选择驱动:\n");
    printf("  1 - 原驱动 (自动刷入)\n");
    printf("  2 - KPM驱动 (直接连接)\n");
    printf("输入: ");
    int choice = 0;
    scanf("%d", &choice);

    if (choice == 1) {
        g_driver_mode = 0;
        if (!模块刷入()) {
            printf("[-] 驱动刷入失败\n");
            return 1;
        }
    } else if (choice == 2) {
        g_driver_mode = 1;
        printf("[*] KPM模式，跳过刷入\n");
    } else {
        printf("[-] 无效输入\n");
        return 1;
    }
    if (mkdir("/sdcard/AuraKernel", 0777) == -1) {
        if (errno != EEXIST) {
            perror("创建AuraKernel文件夹失败,请手动在/sdcard/下创建AuraKernel文件夹");
        }
    }
    system("chmod 777 -R /sdcard/AuraKernel");
    // 显示免责声明
    // displayAgreement();
if (g_driver_mode == 0) {
        printf("\033[1;34m[+] 正在检测Aura独家驱动状态...\033[0m\n");
        绘制.读写.reopen_dev();
        bool moduleOk = false;
        if (绘制.读写.fd > 0 && 绘制.读写.get_Module_On()) {
            moduleOk = true;
        }
        if (!moduleOk) {
            printf("\033[1;33m[-] 检测到驱动未激活, 开始自动刷入...\033[0m\n");
            if (!模块刷入()) {
                printf("\033[1;31m[!] Aura驱动刷入失败, 程序退出\n\033[0m");
                return 0;
            }
            if (!绘制.读写.reopen_dev() || !绘制.读写.get_Module_On()) {
                printf("\033[1;31m[!] Aura驱动加载失败, 请重启设备后重试\n\033[0m");
                return 0;
            }
        }
        printf("\033[1;32m[+] Aura驱动已就绪, 正在启动功能...\033[0m\n");
    }

    if (绘制.防录屏 == 999) {
        printf("是否开启防录屏[1[是]/2[否]]：");
        cin >> FlP;
        if (FlP == "1" || FlP == "1") {
            绘制.防录屏 = 1;
        } else if (FlP == "2" || FlP == "2") {
            绘制.防录屏 = 0;
        } else {
            printf("输入错误!!!");
            绘制.防录屏 = 0;
        }
        绘制.读写.选择配置.防录屏 = 绘制.防录屏;
    }

    printf("1.有后台\n2.无后台\n\n");
    std::cin >> 无后台;
    
    if (无后台 == 1) {
    std::cout << "有后台开启成功\n";
} else {
    pid_t pids = fork();
    if (pids > 0) {
        exit(0);
    }
    std::cout << "无后台开启成功\n";
}

    布局.初始化程序();
    加载内存图片();
    绘制.自瞄主线程();
    绘制.GetTouch();
    绘制.读取配置();
    布局.开启悬浮窗();

    return 0;
}