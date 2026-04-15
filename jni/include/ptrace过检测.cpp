#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <android/log.h>
#include <string.h>
#include <signal.h>
#include <sys/ucontext.h>
#include <vector>

// 定义日志标签和日志输出宏
#define LOG_TAG "BypassCheck"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// 为不支持PTRACE_GETREGS的系统定义常量
#ifndef PTRACE_GETREGS
#define PTRACE_GETREGS 12
#endif

// 定义内存检测函数的类型签名
typedef int (*memory_check_func_t)(void* addr, size_t size);

// ARM64 BRK指令
#define BRK_OPCODE 0xD4200000

// 断点结构体
struct Breakpoint {
    uintptr_t addr;
    uint32_t original_instr;
    bool enabled;
};

// 全局变量
static std::vector<Breakpoint> breakpoints;
static int g_cleanup_registered = 0;

// 断点信号处理器
static void breakpoint_handler(int sig, siginfo_t* info, void* context) {
    #if defined(__aarch64__)
    ucontext_t* ucontext = (ucontext_t*)context;
    uintptr_t pc = (uintptr_t)ucontext->uc_mcontext.pc;
    
    // 查找匹配的断点
    for (auto& bp : breakpoints) {
        if (bp.enabled && bp.addr == pc) {
            LOGI("捕获到断点异常，绕过检测: addr=0x%lx", bp.addr);
            
            // 设置返回值为0（绕过检测）
            ucontext->uc_mcontext.regs[0] = 0;
            
            // 将PC指向下一条指令
            ucontext->uc_mcontext.pc += sizeof(uint32_t);
            
            LOGI("检测绕过成功，返回0");
            return;
        }
    }
    #endif
    
    // 如果不是我们的断点，继续默认处理
    signal(sig, SIG_DFL);
    raise(sig);
}

// 初始化信号处理
static void init_signal_handler() {
    struct sigaction sa;
    struct sigaction old_sa;
    
    sa.sa_sigaction = breakpoint_handler;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&sa.sa_mask);
    
    if (sigaction(SIGTRAP, &sa, &old_sa) == -1) {
        LOGI("信号处理器设置失败");
        // 备用方案
        signal(SIGTRAP, (void (*)(int))breakpoint_handler);
    } else {
        LOGI("断点信号处理器设置成功");
    }
}

// 设置软件断点
static int set_software_breakpoint(void* target_func) {
    if (!target_func) {
        LOGI("目标函数地址为空");
        return -1;
    }
    
    uintptr_t addr = (uintptr_t)target_func;
    
    // 检查地址是否4字节对齐
    if (addr % 4 != 0) {
        LOGI("地址未对齐: %p", target_func);
        return -1;
    }
    
    // 计算页面对齐的地址
    uintptr_t page_mask = ~(uintptr_t)0xFFF;
    uintptr_t func_page = addr & page_mask;
    
    // 修改内存保护属性
    if (mprotect((void*)func_page, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        LOGI("修改内存权限失败: %p", target_func);
        return -1;
    }
    
    // 保存原始指令
    uint32_t original_instr = *(uint32_t*)addr;
    
    // 写入断点指令
    *(uint32_t*)addr = BRK_OPCODE;
    
    // 保存断点信息
    Breakpoint bp;
    bp.addr = addr;
    bp.original_instr = original_instr;
    bp.enabled = true;
    breakpoints.push_back(bp);
    
    // 刷新指令缓存
    __builtin___clear_cache((char*)addr, (char*)addr + sizeof(uint32_t));
    
    LOGI("软件断点设置成功: %p (原指令: 0x%x)", target_func, original_instr);
    return 0;
}

// 清理函数
static void cleanup_hooks() {
    for (auto& bp : breakpoints) {
        if (bp.enabled) {
            uintptr_t page_mask = ~(uintptr_t)0xFFF;
            uintptr_t func_page = bp.addr & page_mask;
            
            if (mprotect((void*)func_page, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
                *(uint32_t*)bp.addr = bp.original_instr;
                __builtin___clear_cache((char*)bp.addr, (char*)bp.addr + sizeof(uint32_t));
                LOGI("恢复原始指令: 0x%lx", bp.addr);
            }
            bp.enabled = false;
        }
    }
    LOGI("所有钩子已清理");
}

// 主要的内存检测绕过函数
void bypass_memory_detection() {
    // 初始化信号处理
    init_signal_handler();
    
    // 动态加载目标库文件libUE4.so到内存中
    void* libue4_handle = dlopen("libUE4.so", RTLD_NOW);
    if (!libue4_handle) {
        LOGI("无法加载libUE4.so");
        return;
    }
    
    // 计算需要修改的特定函数地址（硬编码偏移量）
    //绕过
    void* track_func = (void*)((uintptr_t)libue4_handle + 0x79907e8);
    void* track_func1 = (void*)((uintptr_t)libue4_handle + 0xbafd27c);
    void* track_func2 = (void*)((uintptr_t)libue4_handle + 0xB36B0F4);
    void* track_func3 = (void*)((uintptr_t)libue4_handle + 0x73F4594);
    void* track_func3 = (void*)((uintptr_t)libue4_handle + 0x36F5504);
   // 防行为
    void* track_func3 = (void*)((uintptr_t)libue4_handle + 0x18A9C4); 
    void* track_func3 = (void*)((uintptr_t)libue4_handle + 0x18A9E4);
    void* track_func3 = (void*)((uintptr_t)libue4_handle + 0x18ABCC);
    void* track_func3 = (void*)((uintptr_t)libue4_handle + 0x18ABEC);
    void* track_func3 = (void*)((uintptr_t)libue4_handle + 0x2CCCB0);
    void* track_func3 = (void*)((uintptr_t)libue4_handle + 0x3E9FC4);
    void* track_func3 = (void*)((uintptr_t)libue4_handle + 0xDAB81C);
    //防10
    void* track_func3 = (void*)((uintptr_t)libue4_handle + 0xDAB844);
    void* track_func3 = (void*)((uintptr_t)libue4_handle + 0xDAB850);
    void* track_func3 = (void*)((uintptr_t)libue4_handle + 0x3FC10);
    
    

    
    
    
    // 记录计算出的函数地址
    LOGI("弹道函数地址1: %p", track_func);
    LOGI("弹道函数地址2: %p", track_func1);
    
    // 计算页面对齐的地址
    uintptr_t page_mask = ~(uintptr_t)0xFFF;
    uintptr_t track_page = (uintptr_t)track_func & page_mask;
    uintptr_t track_page1 = (uintptr_t)track_func1 & page_mask;
    
    // 修改内存保护属性
    if (mprotect((void*)track_page, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
        LOGI("修改内存权限成功1");
    }
    
    if (mprotect((void*)track_page1, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
        LOGI("修改内存权限成功2");
    }
    
    // 定义需要钩子的内存检测函数名称数组
    void* check_funcs[] = {
        dlsym(libue4_handle, "_Z12checkMemoryPKvj"),  // C++修饰的函数名：checkMemory
        dlsym(libue4_handle, "_Z16memoryIntegrityv"), // C++修饰的函数名：memoryIntegrity
        dlsym(libue4_handle, "_Z15antiCheatCheckv"),  // C++修饰的函数名：antiCheatCheck
        NULL
    };
    
    // 遍历所有检测函数，设置软件断点
    for (int i = 0; check_funcs[i]; i++) {
        if (check_funcs[i]) {
            // 计算函数所在页面的起始地址
            uintptr_t func_page = (uintptr_t)check_funcs[i] & page_mask;
            // 修改页面保护属性
            if (mprotect((void*)func_page, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
                set_software_breakpoint(check_funcs[i]);
                LOGI("钩子函数%d设置成功", i);
            }
        }
    }
    
    // 专门处理CRC校验检测函数
    void* crc_check = dlsym(libue4_handle, "_Z13crc32CheckAllv");
    if (crc_check) {
        // 计算CRC函数所在页面的起始地址
        uintptr_t crc_page = (uintptr_t)crc_check & page_mask;
        // 修改页面保护属性
        if (mprotect((void*)crc_page, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
            set_software_breakpoint(crc_check);
            LOGI("CRC检测绕过成功");
        }
    }
    
    // 关闭动态库句柄
    dlclose(libue4_handle);
}

// 主要的ptrace跟踪函数
int ptrace_main(int argc, char *argv[]) {
    pid_t pid;
    int orig_rax;
    int iscalling = 0;
    int status = 0;
    uint64_t arg1, arg2, arg3;
    
    struct user_regs_struct {
        unsigned long r15;
        unsigned long r14;
        unsigned long r13;
        unsigned long r12;
        unsigned long rbp;
        unsigned long rbx;
        unsigned long r11;
        unsigned long r10;
        unsigned long r9;
        unsigned long r8;
        unsigned long rax;
        unsigned long rcx;
        unsigned long rdx;
        unsigned long rsi;
        unsigned long rdi;
        unsigned long orig_rax;
        unsigned long rip;
        unsigned long cs;
        unsigned long eflags;
        unsigned long rsp;
        unsigned long ss;
        unsigned long fs_base;
        unsigned long gs_base;
        unsigned long ds;
        unsigned long es;
        unsigned long fs;
        unsigned long gs;
    } regs;

    LOGI("启动内存修改检测绕过");
    bypass_memory_detection();

    // 创建子进程用于ptrace跟踪
    pid = fork();

    if (pid == 0) {
        // 子进程代码
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
            perror("ptrace TRACEME err");
            return -1;
        }

        pid_t current_pid = getpid();
        if (kill(current_pid, SIGSTOP) != 0) {
            perror("kill sigstop err");
        }

        // 测试输出
        write(STDOUT_FILENO, "aaaa -> ", 8);
        write(STDOUT_FILENO, "bbbb -> ", 8);
        write(STDOUT_FILENO, "cccc -> ", 8);

        return 0;
    } else if (pid < 0) {
        perror("fork err");
        return -1;
    }

    // 父进程等待子进程状态变化
    wait(&status);
    if (WIFEXITED(status))
        return 0;

    if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) < 0) {
        perror("ptrace SYSCALL err");
        return -1;
    }

    // 主循环：监控系统调用
    while (1) {
        wait(&status);
        if (WIFEXITED(status))
            break;

        if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) < 0) {
            perror("ptrace GETREGS err");
            return -1;
        }

        orig_rax = regs.orig_rax;
        
        if (orig_rax == SYS_write) {
            if (!iscalling) {
                iscalling = 1;
                arg1 = regs.rdi;
                arg2 = regs.rsi;
                arg3 = regs.rdx;
                LOGI("write调用: fd=%lu, buf=%p, size=%lu", arg1, (void*)arg2, arg3);
            } else {
                LOGI("write返回: %ld", regs.rax);
                iscalling = 0;
            }
        }

        if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) < 0) {
            perror("CONT err");
            return -1;
        }
    }

    return 0;
}

// 手动清理函数
void manual_cleanup() {
    cleanup_hooks();
}

// 构造函数
__attribute__((constructor)) void init() {
    LOGI("程序初始化，开始绕过检测");
    bypass_memory_detection();
    g_cleanup_registered = 1;
}

// 析构函数
__attribute__((destructor)) void deinit() {
    if (g_cleanup_registered) {
        cleanup_hooks();
    }
    LOGI("程序退出清理完成");
}
