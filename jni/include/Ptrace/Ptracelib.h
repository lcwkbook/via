#include <asm/ptrace.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>
#include <elf.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <asm/unistd.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/system_properties.h>
#include <vector>
/**
 * @brief 内存映射信息结构体
 * 用于存储从/proc/[pid]/maps解析出的内存区域信息
 */
typedef struct
{
    unsigned long start;  // 内存区域起始地址
    unsigned long end;    // 内存区域结束地址
    unsigned long offset; // 文件映射偏移量
    unsigned long inode;  // 文件inode号
    int perms;            // 内存权限（PROT_READ | PROT_WRITE | PROT_EXEC）
    char path[512];       // 映射文件路径
} ProcMapInfo;
enum deviceArchitecture
{
    armeabi_v7a,
    arm64_v8a,
    x86,
    x86_64,
    Unknown,
};
struct vma
{
    uint64_t saddr;
    uint64_t eaddr;
    size_t size;
    int r;
    int w;
    int x;
    int p;
    int s;
};
// System libs
static const char *libcPath = "";
static const char *linkerPath = "";
static const char *libDLPath = "";
static deviceArchitecture arch;
static int sdkVersion = 0;

static void setupSystemLibs()
{
    char _sdkVersion[32];
    char sysArchitecture[32];
    __system_property_get("ro.build.version.sdk", _sdkVersion);
    __system_property_get("ro.product.cpu.abi", sysArchitecture);
    sdkVersion = atoi(_sdkVersion);

    if (strcmp(sysArchitecture, "x86_64") == 0)
    {
        arch = deviceArchitecture::x86_64;
        libcPath = "/system/lib64/libc.so";
        linkerPath = "/system/bin/linker64";
        libDLPath = "/system/lib64/libdl.so";
    }
    else if (strcmp(sysArchitecture, "x86") == 0)
    {
        arch = deviceArchitecture::x86;
        libcPath = "/system/lib/libc.so";
        linkerPath = "/system/bin/linker";
        libDLPath = "/system/lib/arm/nb/libdl.so";
    }
    else if (strcmp(sysArchitecture, "arm64-v8a") == 0)
    {
        arch = deviceArchitecture::arm64_v8a;
        if (sdkVersion >= 29)
        { // Android 10
            libcPath = "/apex/com.android.runtime/lib64/bionic/libc.so";
            linkerPath = "/apex/com.android.runtime/bin/linker64";
            libDLPath = "/apex/com.android.runtime/lib64/bionic/libdl.so";
        }
        else
        {
            libcPath = "/system/lib64/libc.so";
            linkerPath = "/system/bin/linker64";
            libDLPath = "/system/lib64/libdl.so";
        }
    }
    else if (strcmp(sysArchitecture, "armeabi-v7a") == 0)
    {
        arch = deviceArchitecture::armeabi_v7a;
    }
    else
    {
        arch = deviceArchitecture::Unknown;
    }
}

static int isSELinuxEnabled()
{
    bool result = 0;
    FILE *fp = fopen("/proc/filesystems", "r");
    char line[100];
    while (fgets(line, 100, fp))
    {
        if (strstr(line, "selinuxfs"))
        {
            result = 1;
        }
    }
    fclose(fp);
    return result;
}

static void disableSELinux()
{
    char line[1024];
    FILE *fp = fopen("/proc/mounts", "r");
    if (fp == NULL)
    {
        return;
    }

    while (fgets(line, 1024, fp))
    {
        if (strstr(line, "selinuxfs"))
        {
            strtok(line, " ");
            char *selinux_dir = strtok(NULL, " ");

            // 使用安全的路径构建方式
            char selinux_path[1024];
            snprintf(selinux_path, sizeof(selinux_path), "%s/enforce", selinux_dir);

            FILE *fp_selinux = fopen(selinux_path, "w");
            if (fp_selinux != NULL)
            {
                char buf[2] = "0"; // 0 = Permissive
                fwrite(buf, strlen(buf), 1, fp_selinux);
                fclose(fp_selinux);
            }
            break;
        }
    }
    fclose(fp);
}
void enableSELinux()
{
    char line[1024];
    FILE *fp = fopen("/proc/mounts", "r");
    if (fp == NULL)
    {
        return; // 无法打开文件，直接返回
    }

    while (fgets(line, 1024, fp))
    {
        if (strstr(line, "selinuxfs"))
        {
            strtok(line, " ");
            char *selinux_dir = strtok(NULL, " ");

            // 为了避免缓冲区溢出，使用更安全的方式构建路径
            char selinux_path[1024];
            snprintf(selinux_path, sizeof(selinux_path), "%s/enforce", selinux_dir);

            FILE *fp_selinux = fopen(selinux_path, "w");
            if (fp_selinux != NULL)
            {
                char buf[2] = "1"; // 1 = Enforcing
                fwrite(buf, strlen(buf), 1, fp_selinux);
                fclose(fp_selinux);
            }
            break;
        }
    }
    fclose(fp);
}
static void launchApp(char *appLaunchActivity)
{
    // Test on termux:
    // am start packagename/launchActivity
    char start_cmd[1024] = "am start ";

    strcat(start_cmd, appLaunchActivity);

    system(start_cmd);
}
/**
 * @brief 在指定进程中搜索对应模块的基址
 *
 * @param pid pid表示远程进程的ID 若为-1表示自身进程
 * @param ModuleName ModuleName表示要搜索的模块的名称
 * @return void* 返回0表示获取模块基址失败，返回非0为要搜索的模块基址
 */
inline void *get_module_base_addr(pid_t pid, const char *ModuleName)
{
    FILE *fp = NULL;
    long ModuleBaseAddr = 0;
    char szFileName[50] = {0};
    char szMapFileLine[1024] = {0};

    // 读取"/proc/pid/maps"可以获得该进程加载的模块
    if (pid < 0)
    {
        //  枚举自身进程模块
        snprintf(szFileName, sizeof(szFileName), "/proc/self/maps");
    }
    else
    {
        snprintf(szFileName, sizeof(szFileName), "/proc/%d/maps", pid);
    }

    fp = fopen(szFileName, "r");

    if (fp != NULL)
    {
        while (fgets(szMapFileLine, sizeof(szMapFileLine), fp))
        {
            if (strstr(szMapFileLine, ModuleName))
            {
                char *Addr = strtok(szMapFileLine, "-");
                ModuleBaseAddr = strtoul(Addr, NULL, 16);

                if (ModuleBaseAddr == 0x8000)
                    ModuleBaseAddr = 0;

                break;
            }
        }

        fclose(fp);
    }

    return (void *)ModuleBaseAddr;
}
void *get_module_base_addr(pid_t pid, const char *ModuleName, uint64_t addr)
{
    FILE *fp = NULL;
    long ModuleBaseAddr = 0;
    char szFileName[50] = {0};
    char szMapFileLine[1024] = {0};

    // 读取"/proc/pid/maps"可以获得该进程加载的模块
    if (pid < 0)
    {
        //  枚举自身进程模块
        snprintf(szFileName, sizeof(szFileName), "/proc/self/maps");
    }
    else
    {
        snprintf(szFileName, sizeof(szFileName), "/proc/%d/maps", pid);
    }

    fp = fopen(szFileName, "r");

    if (fp != NULL)
    {
        while (fgets(szMapFileLine, sizeof(szMapFileLine), fp))
        {
            // 检查行中是否包含模块名
            if (strstr(szMapFileLine, ModuleName))
            {
                char *Addr = strtok(szMapFileLine, "-");
                ModuleBaseAddr = strtoul(Addr, nullptr, 16);

                // 增加条件：如果传入了 addr，并且当前模块基址等于它，则跳过
                if (addr != 0 && ModuleBaseAddr == addr)
                {
                    continue; // 跳到下一行继续查找
                }

                // 找到不等于 addr 的模块，直接返回
                if (ModuleBaseAddr == 0x8000)
                {
                    ModuleBaseAddr = 0;
                }

                fclose(fp);
                return (void *)ModuleBaseAddr;
            }
        }
        fclose(fp);
    }

    return nullptr; // 没找到合适的模块，返回空指针
}
struct vma *find_vma(pid_t pid, uint64_t addr)
{
    FILE *file;
    char path[0xff], buffer[1024];
    struct vma *remote_vma = nullptr;

    sprintf(path, "/proc/%d/maps", pid);
    file = fopen(path, "r");
    if (!file)
    {
        return nullptr;
    }

    remote_vma = (struct vma *)malloc(sizeof(struct vma));
    if (!remote_vma)
    {
        fclose(file);
        return nullptr;
    }
    memset(remote_vma, 0, sizeof(struct vma));

    while (fgets(buffer, 1024, file) != nullptr)
    {
        uint64_t saddr, eaddr;
        char r, w, x, p;

        if (sscanf(buffer, "%lx-%lx %c%c%c%c", &saddr, &eaddr, &r, &w, &x, &p) != 6)
        {
            continue;
        }

        if (addr >= saddr && addr < eaddr)
        {
            remote_vma->saddr = saddr;
            remote_vma->eaddr = eaddr;
            remote_vma->size = eaddr - saddr;
            remote_vma->r = (r == 'r') ? 1 : 0;
            remote_vma->w = (w == 'w') ? 1 : 0;
            remote_vma->x = (x == 'x') ? 1 : 0;
            remote_vma->p = (p == 'p') ? 1 : 0;

            fclose(file);
            return remote_vma;
        }
    }

    free(remote_vma);
    fclose(file);
    return nullptr;
}
void *fake_libc_addr(pid_t pid, void *RemoteModuleAddr, const char *ModuleName)
{
    // pr_fuck("%s", ModuleName);
    auto vma = find_vma(pid, (uint64_t)RemoteModuleAddr);
    // r: 1, w: 0, x: 0, p: 0 异常
    // r: 1, w: 0, x: 0, p: 1 正常
    // pr_fuck("one r: %d, w: %d, x: %d, p: %d\n", vma->r, vma->w, vma->x, vma->p);
    if (vma->r == 1 && vma->w == 0 && vma->x == 0 && vma->p == 0)
    {
        // pr_fuck("错误的头部地址: %p\n", RemoteModuleAddr);
        RemoteModuleAddr = get_module_base_addr(pid, ModuleName, (uint64_t)RemoteModuleAddr);
        vma = find_vma(pid, (uint64_t)RemoteModuleAddr);
        // pr_fuck("tw r: %d, w: %d, x: %d, p: %d\n", vma->r, vma->w, vma->x, vma->p);
        if (vma->r == 1 && vma->w == 0 && vma->x == 0 && vma->p == 1)
        {
            // pr_fuck("正确的头部地址: %p\n", RemoteModuleAddr);
        }
    }
    free(vma);
    return RemoteModuleAddr;
}
// Search the base address of the module in the process
static void *getModuleBaseAddr1(pid_t pid, const char *moduleName)
{
    long moduleBaseAddr = 0;
    char mapsPath[50] = {0};
    char szMapFileLine[1024] = {0};

    snprintf(mapsPath, sizeof(mapsPath), "/proc/%d/maps", pid);
    FILE *fp = fopen(mapsPath, "r");

    if (fp != nullptr)
    {
        while (fgets(szMapFileLine, sizeof(szMapFileLine), fp))
        {
            if (strstr(szMapFileLine, moduleName))
            {
                char *address = strtok(szMapFileLine, "-");
                moduleBaseAddr = strtoul(address, nullptr, 16);

                if (moduleBaseAddr == 0x8000)
                {
                    moduleBaseAddr = 0;
                }

                break;
            }
        }
        fclose(fp);
    }

    return (void *)moduleBaseAddr;
}

// Get the address of the function in the module loaded by the remote process and this process
/**
 * @brief 获取远程进程与本进程都加载的模块中函数的地址
 *
 * @param pid pid表示远程进程的ID
 * @param ModuleName ModuleName表示模块名称
 * @param LocalFuncAddr LocalFuncAddr表示本地进程中该函数的地址
 * @return void* 返回远程进程中对应函数的地址
 */
inline void *getRemoteFuncAddr(pid_t pid, const char *ModuleName, void *LocalFuncAddr)
{
    void *LocalModuleAddr, *RemoteModuleAddr, *RemoteFuncAddr;
    // 获取本地某个模块的起始地址
    LocalModuleAddr = get_module_base_addr(-1, ModuleName);
    // 获取远程pid的某个模块的起始地址
    RemoteModuleAddr = get_module_base_addr(pid, ModuleName);

    RemoteModuleAddr = fake_libc_addr(pid, RemoteModuleAddr, ModuleName);

    RemoteFuncAddr = (void *)((uintptr_t)LocalFuncAddr - (uintptr_t)LocalModuleAddr +
                              (uintptr_t)RemoteModuleAddr);

    return RemoteFuncAddr;
}
