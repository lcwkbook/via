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
#include <fstream>
#include<iostream>
#include<ctime>
using namespace std;
#include "Draw.h"
#include "辅助类.h"
#include "图片调用.h"
#include "弄死/模块.h"
#include <sys/utsname.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
using namespace std;

// 全局变量
int abs_ScreenX, abs_ScreenY;
int 无后台;
string Wht, FlP;
int ZM;
布局 布局;
绘制 绘制;

// void displayAgreement() {
//     system("clear");
    
//     // 统一使用亮白色粗体
//     printf("\033[1;37m"); 
//     printf("免！责！声！明！\n\n");
//     printf("•仅限于技术研究 安全测试 逆向工程教育用途\n");
//     printf("• 严禁用于游戏作弊、网络攻击、商业盈利等非法活动\n");
//     printf("• 违反使用条款可能导致手机自动重启并清除相关数据\n\n");
//     printf("• 使用本工具需遵守当地所有相关法律法规\n");
//     printf("• 如所在地法律禁止此类工具，请立即停止使用\n");
//     printf("• 任何违法行为与开发者无关\n\n");
//     printf("\033[1;37m"); // 保持白色
//     printf("回车键同意...\033[0m");
//     getchar();
// }
#include "weiyan/Util.h"//导入微验库(每次注入的库不通用，请使用对应注入的库)
int main() {
    if (mkdir("/sdcard/AuraKernel", 0777) == -1) {
        if (errno != EEXIST) {
            perror("创建AuraKernel文件夹失败,请手动在/sdcard/下创建AuraKernel文件夹");
        }
    }
    system("chmod 777 -R /sdcard/AuraKernel");
    // 微验接口域名
	const string p53adc95617fa3801e961094a9623f569 = "wy.llua.cn";
	// 当前版本，用于检查更新
	const string currentVersion = "1.36.2.15";
	// 卡密存储路径
	const string kmPath = "/sdcard/AuraKernel/AuraKernel.km";
	
	// 获取公告
	string notice_data = httppost(p53adc95617fa3801e961094a9623f569,"v2/7a53d5145d003d56b56a6a93675ebfc5",q055b46e7ab1787e2889e16edbec3fe53(q055b46e7ab1787e2889e16edbec3fe53(ic564ef4492dc141853f75f5a1a7f8c79(q055b46e7ab1787e2889e16edbec3fe53(ic564ef4492dc141853f75f5a1a7f8c79(e86fce770c123cd27a1bc7e98ba5d8636(e86fce770c123cd27a1bc7e98ba5d8636("id=MqLA3bZQzdY","6mP4biWXu5VCqL1RBgnGY02M3kQfdlJjeycHNAwUZDav+9tTIzK8/EFrhoOSsxp7"),"gobrO8/4nKY+0ypCsBPwtWfqeDxQichLH9zImAZadXRUu72j31kFvVJTS6GNE5Ml"),"x589e22663c9931c59123645a089706340e7b")),"w124dafa8243aed37c90d40"))));
	//try {
		json notice_y2c1b45fb981342df242c3dc897a3692c = json::parse(ic564ef4492dc141853f75f5a1a7f8c79(a2a3b5fd0f867f4c9a13c5eb5eac8b13b(notice_data),"be89813622d88e96c29c3d2"));
		if (notice_y2c1b45fb981342df242c3dc897a3692c.contains("msg")) {
			std::string gg = notice_y2c1b45fb981342df242c3dc897a3692c["msg"]["app_gg"];
			if (!gg.empty()) {
				std::cout << "公告:\n" << gg << std::endl;
			} else {
				std::cerr << "公告解析失败[-1]" << std::endl;
			}
		} else {
			std::cerr << "公告解析失败[-2]" << std::endl;
		}
	//}
	//catch(const std::exception & e) {
		//std::cerr << "公告获取失败" << e.what() << std::endl;
	//}
	std::cout << std::endl;
	
	// 检查更新
	string ini_data = httppost(p53adc95617fa3801e961094a9623f569,"v2/7a53d5145d003d56b56a6a93675ebfc5",q055b46e7ab1787e2889e16edbec3fe53(q055b46e7ab1787e2889e16edbec3fe53(ic564ef4492dc141853f75f5a1a7f8c79(q055b46e7ab1787e2889e16edbec3fe53(ic564ef4492dc141853f75f5a1a7f8c79(e86fce770c123cd27a1bc7e98ba5d8636(e86fce770c123cd27a1bc7e98ba5d8636("id=W766ThtI6Sh","6mP4biWXu5VCqL1RBgnGY02M3kQfdlJjeycHNAwUZDav+9tTIzK8/EFrhoOSsxp7"),"gobrO8/4nKY+0ypCsBPwtWfqeDxQichLH9zImAZadXRUu72j31kFvVJTS6GNE5Ml"),"x589e22663c9931c59123645a089706340e7b")),"w124dafa8243aed37c90d40"))));
	//try {
		json ini_y2c1b45fb981342df242c3dc897a3692c = json::parse(ic564ef4492dc141853f75f5a1a7f8c79(a2a3b5fd0f867f4c9a13c5eb5eac8b13b(ini_data),"be89813622d88e96c29c3d2"));
		if (ini_y2c1b45fb981342df242c3dc897a3692c.contains("msg")) {
			std::string version = ini_y2c1b45fb981342df242c3dc897a3692c["msg"]["version"];
			std::string updateshow = ini_y2c1b45fb981342df242c3dc897a3692c["msg"]["updateshow"];
			std::string updateurl = ini_y2c1b45fb981342df242c3dc897a3692c["msg"]["updateurl"];
			std::string updatemust = ini_y2c1b45fb981342df242c3dc897a3692c["msg"]["updatemust"];
			if (version != currentVersion){
				std::cout << "有新版本:" << std::endl << 	
				             "当前版本:" << currentVersion << std::endl <<
				             "最新版本:" << version << std::endl <<
				             "更新内容:" << updateshow << std::endl <<
				             "更新地址:" << updateurl << std::endl;
				if (updatemust == "y"){
		    	    std::cout << "强制更新，请更新至最新版本后使用！" << std::endl;
		    	    exit(0);
		        }
		    }
		} else {
			std::cerr << "更新解析失败" << std::endl;
		}
	//}
	//catch(const std::exception & e) {
		//std::cerr << "检查更新失败" << e.what() << std::endl;
	//}
	std::cout << std::endl;
	
	// 单码登录
	while(true){
	string q214b1ab12aef458ff8e4dc1110f0a8d0;
	string p5b29f9084d505517b0c6932f04fcda60 = getIMEI();
	std::cout << "请输入卡密(输入y使用上次登录卡密): ";
    std::cin >> q214b1ab12aef458ff8e4dc1110f0a8d0;
    
    if (q214b1ab12aef458ff8e4dc1110f0a8d0 == "y"){
    	ifstream file(kmPath);
        if (!file.is_open()) {
        	std::cout << "没有获取到上次登录卡密" << std::endl;
            continue;
        }
        getline(file, q214b1ab12aef458ff8e4dc1110f0a8d0);
        file.close();
        std::cout << "使用上次卡密:" << q214b1ab12aef458ff8e4dc1110f0a8d0 << std::endl;
    }
    
    std::random_device rd;
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 引擎
    std::uniform_int_distribution<> dist(100000, 999999);
    
    auto now = std::chrono::system_clock::now();
    auto epoch = now.time_since_epoch();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(epoch).count();
    
	string a8b468f277261cfffd655145d0912c52c = std::to_string(timestamp);
	string d475c614433d02f303823a9406060656b = std::to_string(dist(gen));
	string p75df889611dc4eddb2ce2c1e88ab7877 = db7bd233ab6ac18a259369a77c3952d2d("kami=" + q214b1ab12aef458ff8e4dc1110f0a8d0 + "&markcode=" + p5b29f9084d505517b0c6932f04fcda60 + "&t=" + a8b468f277261cfffd655145d0912c52c + "&z94211e857b77b73a9bb848d9f68ce0");
	string g774df365df26e2ee72d51a0d65da47d3 = httppost(p53adc95617fa3801e961094a9623f569,"v2/7a53d5145d003d56b56a6a93675ebfc5",q055b46e7ab1787e2889e16edbec3fe53(q055b46e7ab1787e2889e16edbec3fe53(ic564ef4492dc141853f75f5a1a7f8c79(q055b46e7ab1787e2889e16edbec3fe53(ic564ef4492dc141853f75f5a1a7f8c79(e86fce770c123cd27a1bc7e98ba5d8636(e86fce770c123cd27a1bc7e98ba5d8636("id=fF0i668I3r6&kami=" + q214b1ab12aef458ff8e4dc1110f0a8d0 + "&markcode=" + p5b29f9084d505517b0c6932f04fcda60 + "&t=" + a8b468f277261cfffd655145d0912c52c + "&sign=" + p75df889611dc4eddb2ce2c1e88ab7877 + "&value=" + d475c614433d02f303823a9406060656b +"","6mP4biWXu5VCqL1RBgnGY02M3kQfdlJjeycHNAwUZDav+9tTIzK8/EFrhoOSsxp7"),"gobrO8/4nKY+0ypCsBPwtWfqeDxQichLH9zImAZadXRUu72j31kFvVJTS6GNE5Ml"),"x589e22663c9931c59123645a089706340e7b")),"w124dafa8243aed37c90d40"))));
	//try {
		json y2c1b45fb981342df242c3dc897a3692c = json::parse(ic564ef4492dc141853f75f5a1a7f8c79(a2a3b5fd0f867f4c9a13c5eb5eac8b13b(j92cd8698fe044759e0345c9dc72b9131(e2504be4a3abb946792a7730ba9343294(e2504be4a3abb946792a7730ba9343294(ic564ef4492dc141853f75f5a1a7f8c79(a2a3b5fd0f867f4c9a13c5eb5eac8b13b(g774df365df26e2ee72d51a0d65da47d3),"gb754377998120d50d0bfb6bf"),"OEfGRcS9nhVPb/WkQ7o+NardUzHl35Tjvpu40YytsZ6ID12FqBKieg8AJmLMwxXC"),"NyCYmhk7DHZ94jRJ5nOetfqEauWdFr1plGgPAB3woI0T+x2civK6LQVsX/bMSU8z"))),"b178cd7bab2a15ca0aa89d19c13041a"));
		if (y2c1b45fb981342df242c3dc897a3692c["ic8a4e9d355910436d611033c4213d6dc"] == 73623 && y2c1b45fb981342df242c3dc897a3692c["a249f1f161a85b953f25dd7a204a8d37f"]["w410f9a00b2c0adcda28a6329997abe88"] == "26cdbd1ef7212b299cfb310bdb34742c"){
            long c68c788fe2ac2264971ab89b2d986e74e = y2c1b45fb981342df242c3dc897a3692c["l91899b4b30440727fb22845b8067065a"];
            if (c68c788fe2ac2264971ab89b2d986e74e-std::stol(a8b468f277261cfffd655145d0912c52c)>30 || c68c788fe2ac2264971ab89b2d986e74e-std::stol(a8b468f277261cfffd655145d0912c52c)<-30){
                std::cout << "设备时间不准\n" << std::endl;
            }else{
                std::string t1a48e52e2ce98b437c2706ceda90d16d = std::to_string(c68c788fe2ac2264971ab89b2d986e74e);
                long l0a5d48822e2cc06726e0f7b1ab45ad20 = y2c1b45fb981342df242c3dc897a3692c["ic8a4e9d355910436d611033c4213d6dc"];
                std::string s6c1bf7791a6ecfc2b4feaa9f0beb098f = std::to_string(l0a5d48822e2cc06726e0f7b1ab45ad20);
                long ob930674dd0a4f9e2dd9d1bcb3ac7d8b6 = y2c1b45fb981342df242c3dc897a3692c["a249f1f161a85b953f25dd7a204a8d37f"]["z6ed443048173f6627433180d7f4c44f7"];
                std::string o55b80d863f1518d3b8242c3bcd28ac2e = std::to_string(ob930674dd0a4f9e2dd9d1bcb3ac7d8b6);
                if (y2c1b45fb981342df242c3dc897a3692c["a249f1f161a85b953f25dd7a204a8d37f"]["d9f9b1dc29369e9"] != g4de9172b159299af73efc415232a9d72(db7bd233ab6ac18a259369a77c3952d2d(t1a48e52e2ce98b437c2706ceda90d16d+d475c614433d02f303823a9406060656b+"")) || y2c1b45fb981342df242c3dc897a3692c["a249f1f161a85b953f25dd7a204a8d37f"]["d1294e3d4729d12b9c1"] != g4de9172b159299af73efc415232a9d72(g4de9172b159299af73efc415232a9d72(p75df889611dc4eddb2ce2c1e88ab7877+d475c614433d02f303823a9406060656b+p75df889611dc4eddb2ce2c1e88ab7877+"")) || y2c1b45fb981342df242c3dc897a3692c["a249f1f161a85b953f25dd7a204a8d37f"]["z8315b78388f6"] != g4de9172b159299af73efc415232a9d72(g4de9172b159299af73efc415232a9d72(g098bb6a6def44379eeede0700f5d89a0(o55b80d863f1518d3b8242c3bcd28ac2e+d475c614433d02f303823a9406060656b+s6c1bf7791a6ecfc2b4feaa9f0beb098f+"z94211e857b77b73a9bb848d9f68ce0"))) || y2c1b45fb981342df242c3dc897a3692c["a249f1f161a85b953f25dd7a204a8d37f"]["r06fcf994bf"] != db7bd233ab6ac18a259369a77c3952d2d(g098bb6a6def44379eeede0700f5d89a0("z94211e857b77b73a9bb848d9f68ce0"+a8b468f277261cfffd655145d0912c52c+"z94211e857b77b73a9bb848d9f68ce0"+p75df889611dc4eddb2ce2c1e88ab7877+""))){
                    std::cout << "校验失败\n" << std::endl;
                }else{
                    if (y2c1b45fb981342df242c3dc897a3692c["a249f1f161a85b953f25dd7a204a8d37f"]["fc797893ed1a2e6ef2f49095a7e141835"] == "single"){
                        std::cout << "登录成功，剩余可登录次数：" << y2c1b45fb981342df242c3dc897a3692c["a249f1f161a85b953f25dd7a204a8d37f"]["uecec18643874ab2a4c11357cf53a6643"] << std::endl;
                    }else{
                        long deec2c1a253e47d87c140167be8075125 = y2c1b45fb981342df242c3dc897a3692c["a249f1f161a85b953f25dd7a204a8d37f"]["c942da3aa46097dbda112da917bc5c631"];
                        std::tm tm = *std::localtime(&deec2c1a253e47d87c140167be8075125 );
                        std::stringstream ss;
                        ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
                        std::cout << "登录成功，到期时间：" << ss.str() << std::endl;
                        //到期自动退出
				        signal(SIGALRM, _exit); 
                        alarm(deec2c1a253e47d87c140167be8075125-timestamp);
                    }
                    ofstream file(kmPath);
                    if (file.is_open()) {
                        file << q214b1ab12aef458ff8e4dc1110f0a8d0 << endl;
                        file.close();
                    } else {
                        cerr << "无法保存卡密到文件: " << kmPath << endl;
                    }
			        break;
                }
            }
		}else{
		    std::string msg = y2c1b45fb981342df242c3dc897a3692c["a249f1f161a85b953f25dd7a204a8d37f"];
			std::cout << msg << std::endl;
		}
	//}
	//catch(const std::exception & e) {
		//std::cerr << "登录失败" << e.what() << std::endl;
	//}
	std::cout << std::endl;
	}
	
	

    // 显示免责声明
    // displayAgreement();
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