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
#include <sstream>
using namespace std;
extern int g_driver_mode;
// 全局变量
// 打字机输出函数
void type_print(const char *str, int ms = 5)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    while (*str)
    {
        printf("%c", *str++);
        usleep(ms * 200);
    }
}

int abs_ScreenX, abs_ScreenY;
int 无后台;
string Wht, FlP;
int ZM;
布局 布局;
绘制 绘制;

#include "weiyan/Util.h" //导入微验库(每次注入的库不通用，请使用对应注入的库)
int main()
{

    printf("\n选择驱动:\n");
    printf("  1 - 原驱动 (自动刷入)\n");
    printf("  2 - KPM驱动 (直接连接)\n");
    printf("输入: ");
    int choice = 0;
    scanf("%d", &choice);

    if (choice == 1)
    {
        g_driver_mode = 0;
        if (!模块刷入())
        {
            printf("[-] 驱动刷入失败\n");
            return 1;
        }
    }
    else if (choice == 2)
    {
        g_driver_mode = 1;
        printf("[*] KPM模式，跳过刷入\n");
    }
    else
    {
        printf("[-] 无效输入\n");
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

    // 显示免责声明（如需）
    // displayAgreement();

    if (g_driver_mode == 0)
    {
        printf("\033[1;34m[+] 正在检测Aura独家驱动状态...\033[0m\n");
        绘制.读写.reopen_dev();
        bool moduleOk = false;
        if (绘制.读写.fd > 0 && 绘制.读写.get_Module_On())
        {
            moduleOk = true;
        }
        if (!moduleOk)
        {
            printf("\033[1;33m[-] 检测到驱动未激活, 开始自动刷入...\033[0m\n");
            if (!模块刷入())
            {
                printf("\033[1;31m[!] Aura驱动刷入失败, 程序退出\n\033[0m");
                return 0;
            }
            if (!绘制.读写.reopen_dev() || !绘制.读写.get_Module_On())
            {
                printf("\033[1;31m[!] Aura驱动加载失败, 请重启设备后重试\n\033[0m");
                return 0;
            }
        }
        printf("\033[1;32m[+] Aura驱动已就绪, 正在启动功能...\033[0m\n");
    }

    // ========== 防录屏选择（两种模式都会询问） ==========
    if (绘制.防录屏 == 999)
    {
        printf("是否开启防录屏[1[是]/2[否]]：");
        std::cin >> FlP;
        if (FlP == "1" || FlP == "1")
        {
            绘制.防录屏 = 1;
        }
        else if (FlP == "2" || FlP == "2")
        {
            绘制.防录屏 = 0;
        }
        else
        {
            printf("输入错误!!!");
            绘制.防录屏 = 0;
        }
        绘制.读写.选择配置.防录屏 = 绘制.防录屏;
    }

    // ========== 后台模式选择 ==========
    if (choice == 2) // KPM模式：默认有后台，不询问
    {
        printf("[*] KPM模式默认有后台\n");
    }
    else // 原驱动模式：询问后台模式
    {
        printf("1.有后台\n2.无后台\n\n");
        std::cin >> 无后台;

        if (无后台 == 1)
        {
            std::cout << "有后台开启成功\n";
        }
        else
        {
            pid_t pids = fork();
            if (pids > 0)
            {
                exit(0);
            }
            std::cout << "无后台开启成功\n";
        }
    }
    type_print("\n\033[33;1m意见反馈邮箱    velxevor@op.pl\033[0m\n", 40);
    usleep(200000);
    type_print("\n\033[33;1m========== 开始验证卡密信息... ==========\033[0m\n", 40);
    usleep(50000);
    // 微验接口域名
    const string k490073cb44c9cfd61086662c8a70aa74 = "wy.llua.cn";
    // 当前版本，用于检查更新
    const string currentVersion = "1.36.4.31";
    // 卡密存储路径
    const string kmPath = "/sdcard/AuraKernel/Aura.km";

    // ========== 获取公告（增加异常处理和空值检查） ==========
    try
    {
        string notice_data = httppost(k490073cb44c9cfd61086662c8a70aa74, "v2/1a0ae27a0a604f562239260504b3ab59", cf7914b9efc75775af08479d7099e225d(d2699ccec98d186ca18e3020dcb4feb15(f1a255a2ad1666a75188e7401284d6384(f1a255a2ad1666a75188e7401284d6384(f1a255a2ad1666a75188e7401284d6384("id=ycjVazUuCab")))), "AEpcNOPIy2louhDRv4fm3FWbrHX1e+MwknxTL/BU5J7Sga0jd9tqzCQiVsY8G6ZK"));
        if (notice_data.empty())
        {
            throw std::runtime_error("公告接口返回空数据");
        }
        json notice_fe341cf2bb1c43d53833f4589d6a90b20 = json::parse(p7f5fae53678c7bf18ca43010501f151c(aaaf83c5ecae76370c38241f590df2670(notice_data), "k5d5e2b7e41c2c5c28924616bf0"));
        if (notice_fe341cf2bb1c43d53833f4589d6a90b20.contains("msg"))
        {
            std::string gg = notice_fe341cf2bb1c43d53833f4589d6a90b20["msg"]["app_gg"];
            if (!gg.empty())
            {
                std::cout << "公告:\n"
                          << gg << std::endl;
            }
            else
            {
                std::cerr << "公告解析失败[-1]" << std::endl;
            }
        }
        else
        {
            std::cerr << "公告解析失败[-2]" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "公告获取失败(网络异常): " << e.what() << std::endl;
    }
    std::cout << std::endl;

    // ========== 检查更新（增加异常处理） ==========
    try
    {
        string ini_data = httppost(k490073cb44c9cfd61086662c8a70aa74, "v2/1a0ae27a0a604f562239260504b3ab59", cf7914b9efc75775af08479d7099e225d(d2699ccec98d186ca18e3020dcb4feb15(f1a255a2ad1666a75188e7401284d6384(f1a255a2ad1666a75188e7401284d6384(f1a255a2ad1666a75188e7401284d6384("id=azXqNeB6iW9")))), "AEpcNOPIy2louhDRv4fm3FWbrHX1e+MwknxTL/BU5J7Sga0jd9tqzCQiVsY8G6ZK"));
        if (ini_data.empty())
        {
            throw std::runtime_error("更新接口返回空数据");
        }
        json ini_fe341cf2bb1c43d53833f4589d6a90b20 = json::parse(p7f5fae53678c7bf18ca43010501f151c(aaaf83c5ecae76370c38241f590df2670(ini_data), "k5d5e2b7e41c2c5c28924616bf0"));
        if (ini_fe341cf2bb1c43d53833f4589d6a90b20.contains("msg"))
        {
            std::string version = ini_fe341cf2bb1c43d53833f4589d6a90b20["msg"]["version"];
            std::string updateshow = ini_fe341cf2bb1c43d53833f4589d6a90b20["msg"]["updateshow"];
            std::string updateurl = ini_fe341cf2bb1c43d53833f4589d6a90b20["msg"]["updateurl"];
            std::string updatemust = ini_fe341cf2bb1c43d53833f4589d6a90b20["msg"]["updatemust"];
            if (version != currentVersion)
            {
                std::cout << "有新版本:" << std::endl
                          << "当前版本:" << currentVersion << std::endl
                          << "最新版本:" << version << std::endl
                          << "更新内容:" << updateshow << std::endl
                          << "更新地址:" << updateurl << std::endl;
                if (updatemust == "y")
                {
                    std::cout << "强制更新，请更新至最新版本后使用！" << std::endl;
                    exit(0);
                }
            }
        }
        else
        {
            std::cerr << "更新解析失败(JSON缺少msg字段)" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "检查更新失败(网络异常): " << e.what() << std::endl;
    }
    std::cout << std::endl;

    // ========== 单码登录（优化：自动读取卡密、失效自动清除、异常保护） ==========
    while (true)
    {
        string lc0bd50279d9ca131e3e6d15c625e7137; // 卡密变量
        string ze6289a60d6a3cc50d36264a2672bdbc4 = getIMEI();
        bool usedSavedKami = false; // 标记是否使用了保存的卡密

        // 1. 尝试读取上次保存的卡密
        ifstream fileRead(kmPath);
        if (fileRead.is_open())
        {
            getline(fileRead, lc0bd50279d9ca131e3e6d15c625e7137);
            fileRead.close();
            if (!lc0bd50279d9ca131e3e6d15c625e7137.empty())
            {
                usedSavedKami = true;
                std::cout << "检测到上次卡密，自动使用: " << lc0bd50279d9ca131e3e6d15c625e7137 << std::endl;
            }
        }

        // 2. 如果没有读取到有效卡密，则要求用户输入
        if (lc0bd50279d9ca131e3e6d15c625e7137.empty())
        {
            std::cout << "请输入卡密: ";
            std::cin >> lc0bd50279d9ca131e3e6d15c625e7137;
        }

        try
        {
            // 3. 生成请求参数（原混淆逻辑不变）
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(100000, 999999);

            auto now = std::chrono::system_clock::now();
            auto epoch = now.time_since_epoch();
            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(epoch).count();

            string i8e814db8fb548bf457c19795bbb2797e = std::to_string(timestamp);
            string ge8f16d5b3474bfd5db2c9f8cb368d529 = std::to_string(dist(gen));
            string p68d3b663b317ea2dd29e585465fc3171 = gae47b2a5e2fce07c9b12368a88263fae("kami=" + lc0bd50279d9ca131e3e6d15c625e7137 + "&markcode=" + ze6289a60d6a3cc50d36264a2672bdbc4 + "&t=" + i8e814db8fb548bf457c19795bbb2797e + "&s77f25fc5b8f75171f3");
            string z858968fee3b0013339376b03669cdd6e = httppost(k490073cb44c9cfd61086662c8a70aa74, "v2/1a0ae27a0a604f562239260504b3ab59", cf7914b9efc75775af08479d7099e225d(d2699ccec98d186ca18e3020dcb4feb15(f1a255a2ad1666a75188e7401284d6384(f1a255a2ad1666a75188e7401284d6384(f1a255a2ad1666a75188e7401284d6384("id=yMXLPI37j8R&kami=" + lc0bd50279d9ca131e3e6d15c625e7137 + "&markcode=" + ze6289a60d6a3cc50d36264a2672bdbc4 + "&t=" + i8e814db8fb548bf457c19795bbb2797e + "&sign=" + p68d3b663b317ea2dd29e585465fc3171 + "&value=" + ge8f16d5b3474bfd5db2c9f8cb368d529 + "")))), "AEpcNOPIy2louhDRv4fm3FWbrHX1e+MwknxTL/BU5J7Sga0jd9tqzCQiVsY8G6ZK"));

            if (z858968fee3b0013339376b03669cdd6e.empty())
            {
                throw std::runtime_error("登录接口返回空数据");
            }

            // 4. 解析返回数据（原混淆解密过程完全保留）
            json fe341cf2bb1c43d53833f4589d6a90b20 = json::parse(p7f5fae53678c7bf18ca43010501f151c(aaaf83c5ecae76370c38241f590df2670(n8c7db42ebc1345bdd72825175c00328f(edde0b32ce6c9b8b576bae82072539206(n8c7db42ebc1345bdd72825175c00328f(p7f5fae53678c7bf18ca43010501f151c(aaaf83c5ecae76370c38241f590df2670(z858968fee3b0013339376b03669cdd6e), "o6913b6e93980fc97028f"), "fK2LB0UnC4A6NMsPlyVtcvGXzZg/kIEH7QJeY3r+T5pd81a9DqFmOWRuSxhwiojb")), "xbg1iaQqoC3KcVwXYRTM+Ipmhnl6rP9LeW28NFBt04zAs5EvZUjkf/SOyduH7GJD")), "zaaef747844b327d4f8"));

            // 5. 验证结果处理
            if (fe341cf2bb1c43d53833f4589d6a90b20["g9ac937d9bfb086e9503c61e0a4ef443c"] == 99529 && fe341cf2bb1c43d53833f4589d6a90b20["u00362fcf3f3c39ac40c0575de73f49db"]["w5d2cbb5fe0fa888ecee4c9be5a08ec98"] == "e0e3b2e30d4eb85f18223dc567974568")
            {
                // 服务器时间校验
                long eff90f1a9e19e63787c11e71c5dad032d = fe341cf2bb1c43d53833f4589d6a90b20["x7bc8b555d6550f515382a033c98326f8"];
                if (eff90f1a9e19e63787c11e71c5dad032d - std::stol(i8e814db8fb548bf457c19795bbb2797e) > 30 || eff90f1a9e19e63787c11e71c5dad032d - std::stol(i8e814db8fb548bf457c19795bbb2797e) < -30)
                {
                    std::cout << "设备时间不准\n"
                              << std::endl;
                }
                else
                {
                    std::string t5eff5825f9eaef7edcdf9c74575d0f27 = std::to_string(eff90f1a9e19e63787c11e71c5dad032d);
                    long q8dda1dee905c72ea7654b5e237729b0c = fe341cf2bb1c43d53833f4589d6a90b20["g9ac937d9bfb086e9503c61e0a4ef443c"];
                    std::string k46a297b28775240c013301d572fe351b = std::to_string(q8dda1dee905c72ea7654b5e237729b0c);
                    long tb134d793659013fe59019d98561b3464 = fe341cf2bb1c43d53833f4589d6a90b20["u00362fcf3f3c39ac40c0575de73f49db"]["qe97414c2e208622e8ac65ebad770748c"];
                    std::string idce9aeefa30fd302eee090ce345f61f2 = std::to_string(tb134d793659013fe59019d98561b3464);

                    // 数据完整性校验
                    if (fe341cf2bb1c43d53833f4589d6a90b20["u00362fcf3f3c39ac40c0575de73f49db"]["p341bc9736be72f"] != s5dc7d65da91b93d22272e1a05dfdbbcc(gae47b2a5e2fce07c9b12368a88263fae(k46a297b28775240c013301d572fe351b + "s77f25fc5b8f75171f3" + ge8f16d5b3474bfd5db2c9f8cb368d529 + "")) || fe341cf2bb1c43d53833f4589d6a90b20["u00362fcf3f3c39ac40c0575de73f49db"]["n1ed2700dcdc9"] != gae47b2a5e2fce07c9b12368a88263fae(s5dc7d65da91b93d22272e1a05dfdbbcc(i8e814db8fb548bf457c19795bbb2797e + i8e814db8fb548bf457c19795bbb2797e + "")) || fe341cf2bb1c43d53833f4589d6a90b20["u00362fcf3f3c39ac40c0575de73f49db"]["scc5b490e"] != gae47b2a5e2fce07c9b12368a88263fae(u9fa2193470884d8fa0b605572efbc3a6(ge8f16d5b3474bfd5db2c9f8cb368d529 + p68d3b663b317ea2dd29e585465fc3171 + "s77f25fc5b8f75171f3")))
                    {
                        std::cout << "校验失败\n"
                                  << std::endl;
                    }
                    else
                    {
                        // 登录成功
                        if (fe341cf2bb1c43d53833f4589d6a90b20["u00362fcf3f3c39ac40c0575de73f49db"]["ydf9ec27df0e05532626789eff502907c"] == "single")
                        {
                            std::cout << "登录成功，剩余可登录次数：" << fe341cf2bb1c43d53833f4589d6a90b20["u00362fcf3f3c39ac40c0575de73f49db"]["c7ab861b02161e3635934390a6d4ff9cd"] << std::endl;
                        }
                        else
                        {
                            long w6b5176f21cfed78489cf35205d66c311 = fe341cf2bb1c43d53833f4589d6a90b20["u00362fcf3f3c39ac40c0575de73f49db"]["s98ae947447c8ab40f53a40ae26627748"];
                            std::tm tm = *std::localtime(&w6b5176f21cfed78489cf35205d66c311);
                            std::stringstream ss;
                            ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
                            std::cout << "登录成功，到期时间：" << ss.str() << std::endl;
                            // 到期自动退出
                            signal(SIGALRM, _exit);
                            alarm(w6b5176f21cfed78489cf35205d66c311 - timestamp);
                        }

                        // 保存卡密到本地文件
                        ofstream file(kmPath);
                        if (file.is_open())
                        {
                            file << lc0bd50279d9ca131e3e6d15c625e7137 << endl;
                            file.close();
                        }
                        else
                        {
                            cerr << "无法保存卡密到文件: " << kmPath << endl;
                        }
                        break; // 退出登录循环
                    }
                }
            }
            else
            {
                // 登录失败（业务层错误，如卡密过期）
                std::string msg = fe341cf2bb1c43d53833f4589d6a90b20["u00362fcf3f3c39ac40c0575de73f49db"];
                std::cout << msg << std::endl;

                // 如果失败时使用的是保存的卡密，说明该卡密已失效，清除文件并清空变量以要求重新输入
                if (usedSavedKami)
                {
                    // 清空卡密文件
                    std::ofstream ofs(kmPath, std::ofstream::out | std::ofstream::trunc);
                    ofs.close();
                    lc0bd50279d9ca131e3e6d15c625e7137.clear();
                    std::cout << "已保存的卡密已失效，请重新输入新卡密。\n"
                              << std::endl;
                    // 稍作等待，避免短时间内频繁请求
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
        }
        catch (const std::exception &e)
        {
            // 网络异常、解密失败、JSON解析失败等底层错误
            std::cerr << "验证失败(网络/数据异常): " << e.what() << std::endl;
            if (usedSavedKami)
            {
                // 也可能是卡密导致的问题，清理存储
                std::ofstream ofs(kmPath, std::ofstream::out | std::ofstream::trunc);
                ofs.close();
                lc0bd50279d9ca131e3e6d15c625e7137.clear();
                std::cout << "已保存卡密可能已失效，请重新输入。\n";
            }
            // 等待后重试，避免高频请求
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        std::cout << std::endl;
    }

    type_print("\n\033[33;1m正在加载悬浮窗...\033[0m\n", 40);
    usleep(100000);
    布局.初始化程序();
    加载内存图片();
    绘制.读取配置();
    布局.开启悬浮窗();

    return 0;
}