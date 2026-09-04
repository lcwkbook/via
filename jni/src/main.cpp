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
#include <ctime>
#include "Draw.h"
#include "辅助类.h"
#include "图片调用.h"
#include "弄死/模块.h"
#include <sys/utsname.h>
#include <thread>
#include <chrono>
#include <sys/types.h>
#include <sys/wait.h>
#include <sstream>
#include "Updater.h"
#include <cstdlib>
#include "paradise/paradise_api.h"
#include "network_program.h"
#include <openssl/dh.h>
#include "Offsets.h"


using namespace std;

extern int g_driver_mode;

// ====================== 新增：getIMEI 本地实现（解决链接报错） ======================
std::string getIMEI()
{
    char buf[128] = {0};
    // 优先读取设备序列号作为唯一标识
    FILE *fp = popen("getprop ro.serialno 2>/dev/null", "r");
    if (fp)
    {
        if (fgets(buf, sizeof(buf), fp))
        {
            buf[strcspn(buf, "\n\r")] = 0;
        }
        pclose(fp);
    }

    // 序列号为空则降级读取 Android ID
    if (strlen(buf) == 0)
    {
        fp = popen("settings get secure android_id 2>/dev/null", "r");
        if (fp)
        {
            if (fgets(buf, sizeof(buf), fp))
            {
                buf[strcspn(buf, "\n\r")] = 0;
            }
            pclose(fp);
        }
    }

    return strlen(buf) > 0 ? buf : "aura_unknown_device";
}

// 全局变量
void type_print(const char *str, int ms = 5)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("%s", str);
    fflush(stdout);
    (void)ms;
}

int abs_ScreenX, abs_ScreenY;
int 无后台;
string Wht, FlP;
int ZM;
布局 布局;
绘制 绘制;

int g_driver_mode = 0;

// 驱动标记文件路径
const string DRIVER_INSTALLED_FLAG = "/sdcard/AuraKernel/driver_installed.flag";

// 判断文件是否存在
bool isFileExists(const string &path)
{
    ifstream f(path.c_str());
    return f.good();
}

// 创建驱动已安装标记
void createDriverFlag()
{
    ofstream flag(DRIVER_INSTALLED_FLAG);
    if (flag.is_open())
    {
        flag << "ok";
        flag.close();
    }
}

// ========== 主渲染逻辑（可被看门狗拉起） ==========
void RunAuraKernel()
{
    布局.初始化程序();
    绘制.读取配置();

    // 异步加载图片
    std::thread([]()
                { 加载内存图片(); })
        .detach();
    布局.开启悬浮窗(); // 内部死循环，正常不返回
}

int main()
{

    printf("\n");
    printf("  \033[1;36m┌──────────────────────────────────────────┐\033[0m\n");
    printf("  \033[1;36m│  🔒 正在验证卡密信息...\033[0m                      │\n");
    printf("  \033[1;36m└──────────────────────────────────────────┘\033[0m\n\n");
    fflush(stdout);

    network_verify();

     // ========== ★ 新增：远程偏移加载（不通过就退出） ==========
    printf("\n");
    printf("  \033[1;36m┌──────────────────────────────────────────┐\033[0m\n");
    printf("  \033[1;36m│  📡 正在连接授权服务器...\033[0m              │\n");
    printf("  \033[1;36m└──────────────────────────────────────────┘\033[0m\n\n");

    // 建议再加一个 version 参数，方便以后强制更新
    std::string offsetsUrl = "https://aura.xiaon.top/U2FsdGVkX18gof90SQh00kKAnpM=/offsets.json?v=1.0";
    // 偏移全部由服务器 offsets.json 管理：每次启动自动追加时间戳参数绕过 CDN 缓存，
    // 以后只改服务器数据即可，无需再改 ?v= 或本项目代码
    offsetsUrl += "&_t=" + std::to_string(static_cast<long long>(time(nullptr)));

    if (!Offsets::LoadFromRemote(offsetsUrl)) {
        printf("\n  \033[1;31m╔══════════════════════════════════════════════╗\033[0m\n");
        printf("  \033[1;31m║   ❌ 授权服务器无法连接                     ║\033[0m\n");
        printf("  \033[1;31m║   请联系开发者或稍后再试                     ║\033[0m\n");
        printf("  \033[1;31m╚══════════════════════════════════════════════╝\033[0m\n\n");
        sleep(3);
        _exit(1); // 直接退出，不给用
    }

    setvbuf(stdout, NULL, _IONBF, 0);
    printf("\n");
    printf("  ┌─────────────────────────────────────────┐\n");
    printf("  │  \033[1;36m  🚀 AuraKernel 驱动选择  \033[0m              │\n");
    printf("  ├─────────────────────────────────────────┤\n");
    printf("  │  \033[1;33m  [1]\033[0m  KPM驱动          \033[2m(暂不推荐)\033[0m      │\n");
    printf("  │  \033[1;32m  [2]\033[0m  ditpro_kpm驱动   \033[1;32m✨ 最新推荐\033[0m     │\n");
    printf("  │  \033[1;36m  [3]\033[0m  Paradise驱动     \033[2m(次选推荐)\033[0m       │\n");
    printf("  │  \033[1;37m  [4]\033[0m  备用驱动         \033[2m(自动刷入)\033[0m       │\n");
    printf("  └─────────────────────────────────────────┘\n");
    printf("  \033[1;33m⮕\033[0m 请选择 \033[1m[1-4]\033[0m: ");
    int choice = 0;
    scanf("%d", &choice);

    if (choice == 1) // KPM驱动
    {
        g_driver_mode = 1;
        printf("\n  \033[1;36m┌─\033[0m 已选择: \033[1;33mKPM驱动\033[0m \033[2m(跳过刷入)\033[0m\n");
    }
    else if (choice == 2) // ditpro_kpm驱动
    {
        g_driver_mode = 3;
        printf("\n  \033[1;36m┌─\033[0m 已选择: \033[1;32mditpro_kpm驱动\033[0m \033[2m(跳过刷入)\033[0m\n");
    }
    else if (choice == 3) // Paradise驱动
    {
        g_driver_mode = 2;
        printf("\n  \033[1;36m┌─\033[0m 已选择: \033[1;36mParadise驱动\033[0m \033[2m(跳过刷入)\033[0m\n");
    }
    else if (choice == 4) // 备用驱动(自动刷入)
    {
        g_driver_mode = 0;

        const string flag_path = "/sdcard/AuraKernel/driver_installed.flag";
        bool has_flag = false;

        if (access(flag_path.c_str(), 0) == 0)
        {
            has_flag = true;
            printf("  \033[1;32m  ✔\033[0m 检测到驱动已刷入，跳过刷入\n");
        }

        if (!has_flag)
        {
            printf("  \033[1;33m  ⚡\033[0m 首次启动，自动刷入驱动...\n");
            if (!模块刷入())
            {
                printf("  \033[1;31m  ✘\033[0m 驱动刷入失败\n");
                return 1;
            }

            int fd = open(flag_path.c_str(), O_CREAT | O_RDWR, 0777);
            if (fd >= 0)
            {
                write(fd, "ok", 2);
                close(fd);
                printf("  \033[1;32m  ✔\033[0m 驱动刷入成功，已创建标记: \033[2m%s\033[0m\n", flag_path.c_str());
            }
            else
            {
                printf("  \033[1;31m  ✘\033[0m 标记文件创建失败\n");
            }
        }
    }
    else
    {
        printf("  \033[1;31m  ✘\033[0m 无效输入\n");
        return 1;
    }

    if (mkdir("/sdcard/AuraKernel", 0777) == -1)
    {
        if (errno != EEXIST)
        {
            perror("创建AuraKernel文件夹失败,请手动在/sdcard/下创建AuraKernel文件夹");
        }
    }
    system("chmod 777 -R /sdcard/AuraKernel");

    // ===================== 驱动状态检测 =====================
    if (g_driver_mode == 0)
    {
        printf("\n  \033[1;36m┌──────────────────────────────────────────┐\033[0m\n");
        printf("  \033[1;36m│  🔍 Aura独家驱动状态检测\033[0m                  │\n");
        printf("  \033[1;36m└──────────────────────────────────────────┘\033[0m\n");
        printf("  \033[1;34m  ⏳ 正在检测驱动状态...\033[0m\n");
        绘制.读写.reopen_dev();

        bool moduleOk = false;
        if (绘制.读写.fd > 0 && 绘制.读写.get_Module_On())
        {
            moduleOk = true;
        }

        if (!moduleOk)
        {
            if (isFileExists(DRIVER_INSTALLED_FLAG))
            {
                printf("  \033[1;33m  ⚠ 驱动已刷入但未加载，尝试重新打开...\033[0m\n");
                绘制.读写.reopen_dev();
                if (绘制.读写.fd > 0 && 绘制.读写.get_Module_On())
                {
                    moduleOk = true;
                    printf("  \033[1;32m  ✔ 驱动打开成功！\033[0m\n");
                }
            }

            if (!moduleOk)
            {
                printf("  \033[1;33m  ⚠ 驱动未激活，开始自动刷入...\033[0m\n");
                if (!模块刷入())
                {
                    printf("  \033[1;31m  ✘ 驱动刷入失败\033[0m\n");
                    return 0;
                }

                createDriverFlag();
                printf("  \033[1;32m  ✔ 驱动刷入成功，已标记无需重复刷入！\033[0m\n");
                绘制.读写.reopen_dev();
                if (!绘制.读写.get_Module_On())
                {
                    printf("  \033[1;31m  ✘ 驱动加载失败\033[0m\n");
                    return 0;
                }
            }
        }
        printf("  \033[1;32m  ✔ Aura驱动已就绪，正在启动功能...\033[0m\n");
    }
    else if (g_driver_mode == 2)
    {
        printf("  \033[1;36m  ℹ 请确保Paradise驱动已成功刷入\033[0m\n");
    }

    // ========== 防录屏设置 ==========
    if (绘制.防录屏 == 999)
    {
        printf("\n  \033[1;36m┌─────────────────────────────────────────┐\033[0m\n");
        printf("  \033[1;36m│  🛡️ 防录屏设置\033[0m                             │\n");
        printf("  \033[1;36m├─────────────────────────────────────────┤\033[0m\n");
        printf("  \033[1;33m  ⮕ 是否开启防录屏？\033[0m\n");
        printf("     \033[1;32m  [1]\033[0m ✅ 开启\n");
        printf("     \033[1;31m  [2]\033[0m ❌ 关闭\n");
        printf("  \033[1;33m  ⮕ 输入: \033[0m");
        std::cin >> FlP;
        if (FlP == "1")
        {
            绘制.防录屏 = 1;
        }
        else
        {
            绘制.防录屏 = 0;
        }
        绘制.读写.选择配置.防录屏 = 绘制.防录屏;
    }

    // ========== 后台模式选择 ==========
    if (choice == 1)
    {
        printf("  \033[1;36m  ℹ\033[0m KPM模式默认有后台\n");
        无后台 = 1;
    }
    else if (choice == 2)
    {
        printf("  \033[1;36m  ℹ\033[0m ditpro_kpm模式默认有后台\n");
        无后台 = 1;
    }
    else
    {
        printf("\n  \033[1;36m┌─────────────────────────────────────────┐\033[0m\n");
        printf("  \033[1;36m│  ⚙️ 后台模式设置\033[0m                           │\n");
        printf("  \033[1;36m├─────────────────────────────────────────┤\033[0m\n");
        printf("  \033[1;33m  ⮕ 请选择运行模式:\033[0m\n");
        printf("     \033[1;32m  [1]\033[0m 🔄 有后台\n");
        printf("     \033[1;33m  [2]\033[0m 🕊️  无后台\n");
        printf("  \033[1;33m  ⮕ 输入: \033[0m");
        std::cin >> 无后台;

        if (无后台 == 1)
        {
            std::cout << "有后台开启成功\n";
        }
        else
        {
            std::cout << "无后台开启成功\n";
        }
    }

    printf("\n");
    printf("  \033[1;35m┌─────────────────────────────────────────┐\033[0m\n");
    printf("  \033[1;35m│  🎨 正在加载悬浮窗...\033[0m                    │\n");
    printf("  \033[1;35m└─────────────────────────────────────────┘\033[0m\n\n");
    fflush(stdout);
    signal(SIGPIPE, SIG_IGN);

    // ========== 无后台模式：守护进程 + 看门狗（崩溃/被杀后自动拉起） ==========
    if (无后台 == 2)
    {
        while (true)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                // 子进程：跑主逻辑
                RunAuraKernel();
                _exit(0); // 正常到不了这里
            }
            if (pid < 0)
            {
                printf("  \033[1;31m✘ fork 失败，3 秒后重试...\033[0m\n");
                sleep(3);
                continue;
            }

            std::cout << "  \033[1;32m✔ 无后台启动成功，进程已分离!\033[0m\n";

            int status = 0;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
            {
                int code = WEXITSTATUS(status);
                if (code == 42)
                {
                    // 42 = 子进程内部错误（Vulkan device lost / 重建失败），请求重启
                    printf("  \033[1;33m⚠ 内部错误请求重启，3 秒后拉起...\033[0m\n");
                    sleep(3);
                    continue;
                }
                // 其他退出码 = 用户主动退出（点了退出程序或卡密到期），不再拉起
                _exit(code);
            }
            printf("  \033[1;33m⚠ 进程被信号 %d 终止，3 秒后自动重启...\033[0m\n",
                   WIFSIGNALED(status) ? WTERMSIG(status) : 0);
            sleep(3);
        }
    }
    else
    {
        RunAuraKernel();
    }

    std::cout << std::endl;
    return 0;
}