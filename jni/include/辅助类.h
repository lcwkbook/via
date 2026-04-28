#ifndef 辅助类_H
#define 辅助类_H

#include <iostream>
#include <pthread.h>
#include <dirent.h>
#include <driver.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include <map>
#include <cstdio>
#include <stdio.h>
#include "timer.h"
#include "obfuscate.h"
#define PI 3.141592653589793238
#include <iostream>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/stat.h>
#include <cstdlib> // C++标准库
#include <unordered_map>
#include <imgui.h>
#include <mutex>
#define PI 3.141592653589793238
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "结构体.h"
#include "LineOfSightToAPI.h"


#pragma once

#include <arpa/inet.h>
#include <errno.h> // For errno
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h> // for access
#include <sys/types.h>
#include <unistd.h>

// --- 结构体和 IOCTL 定义 ---
struct paradise_read_physical_memory_cmd
{
    pid_t pid;          /* Input: Process ID owning the virtual address */
    uintptr_t src_va;   /* Input: Virtual address to access */
    uintptr_t dst_va;   /* Input: Virtual address to write */
    size_t size;        /* Input: Size of memory to read */
    uintptr_t phy_addr; /* Output: Physical address of the source virtual address */
};

struct paradise_write_physical_memory_cmd
{
    pid_t pid;          /* Input: Process ID owning the virtual address */
    uintptr_t src_va;   /* Input: Virtual address to access */
    uintptr_t dst_va;   /* Input: Virtual address to write */
    size_t size;        /* Input: Size of memory to read */
    uintptr_t phy_addr; /* Output: Physical address of the source virtual address */
};

struct paradise_get_module_base_cmd
{
    pid_t pid;      /* Input: Process ID */
    char name[256]; /* Input: Module name */
    uintptr_t base; /* Output: Base address of the module */
    int vm_flag;    /* Input: VM flag to filter (e.g., VM_EXEC) */
};

struct paradise_find_proc_cmd
{
    pid_t pid;      /* Output: Process ID */
    char name[256]; /* Input: Process name */
};

struct paradise_hide_proc_cmd
{
    pid_t pid; /* Input: Process ID */
    int hide;  /* Input: 1 to hide, 0 to unhide */
};

struct paradise_read_physical_memory_ioremap_cmd
{
    pid_t pid;          /* Input: Process ID owning the virtual address */
    uintptr_t src_va;   /* Input: Virtual address to access */
    uintptr_t dst_va;   /* Input: Virtual address to write */
    size_t size;        /* Input: Size of memory to read */
    uintptr_t phy_addr; /* Output: Physical address of the source virtual address */
    int prot;           /* Input: Memory protection type (use MT_*) */
};

struct paradise_write_physical_memory_ioremap_cmd
{
    pid_t pid;          /* Input: Process ID owning the virtual address */
    uintptr_t src_va;   /* Input: Virtual address to access */
    uintptr_t dst_va;   /* Input: Virtual address to write */
    size_t size;        /* Input: Size of memory to read */
    uintptr_t phy_addr; /* Output: Physical address of the source virtual address */
    int prot;           /* Input: Memory protection type (use MT_*) */
};




// 新增的结构体定义
struct paradise_is_proc_alive_cmd
{
    pid_t pid;  /* Input: Process ID */
    int alive;  /* Output: 1 if alive, 0 if not */
};

struct paradise_give_root_cmd
{
    int result; /* Output: Result of the operation */
};

struct paradise_addr_translate_cmd
{
    uintptr_t phy_addr; /* Output: Physical address */
    pid_t pid;          /* Input: Process ID */
    uintptr_t va;       /* Input: Virtual address */
};
// --- IOCTL 命令 ---
#ifndef __NR_reboot
#if defined(__aarch64__) || defined(__arm__)
#define __NR_reboot 142
#elif defined(__x86_64__)
#define __NR_reboot 169
#elif defined(__i386__)
#define __NR_reboot 88
#elif defined(SYS_reboot)
#define __NR_reboot SYS_reboot
#else
#error "__NR_reboot not defined and cannot be determined for this architecture"
#endif
#endif
// Magic numbers for reboot syscall to install fd
#define PARADISE_INSTALL_MAGIC1 0xDEADBEEF
#define PARADISE_INSTALL_MAGIC2 0xCAFEBABE
#define PARADISE_IOCTL_TOUCH_DOWN _IOWR('W', 22, struct paradise_touch_down_cmd)
#define PARADISE_IOCTL_TOUCH_MOVE _IOWR('W', 23, struct paradise_touch_move_cmd)
#define PARADISE_IOCTL_TOUCH_UP _IOWR('W', 24, struct paradise_touch_up_cmd)
#define PARADISE_IOCTL_HW_BREAKPOINT_CTL _IOWR('W', 25, struct paradise_hw_breakpoint_ctl_cmd)
#define PARADISE_IOCTL_HW_BREAKPOINT_GET_HITS _IOWR('W', 26, struct paradise_hw_breakpoint_get_hits_cmd)
#define PARADISE_IOCTL_READ_MEMORY _IOWR('W', 9, struct paradise_read_physical_memory_cmd)
#define PARADISE_IOCTL_GET_MODULE_BASE _IOWR('W', 10, struct paradise_get_module_base_cmd)
#define PARADISE_IOCTL_FIND_PROCESS _IOWR('W', 11, struct paradise_find_proc_cmd)
#define PARADISE_IOCTL_WRITE_MEMORY _IOWR('W', 12, struct paradise_write_physical_memory_cmd)
#define PARADISE_IOCTL_HIDE_PROCESS _IOWR('W', 14, struct paradise_hide_proc_cmd)
#define PARADISE_IOCTL_READ_MEMORY_IOREMAP _IOWR('W', 16, struct paradise_read_physical_memory_ioremap_cmd)
#define PARADISE_IOCTL_WRITE_MEMORY_IOREMAP _IOWR('W', 17, struct paradise_write_physical_memory_ioremap_cmd)
#define PARADISE_IOCTL_IS_PROCESS_ALIVE _IOWR('W', 15, struct paradise_is_proc_alive_cmd)
#define PARADISE_IOCTL_GIVE_ROOT _IOWR('W', 18, struct paradise_give_root_cmd)
#define PARADISE_IOCTL_ADDR_TRANSLATE _IOWR('W', 19, struct paradise_addr_translate_cmd)
#define WMT_NORMAL 0
#define WMT_NORMAL_TAGGED 1
#define WMT_NORMAL_NC 2
#define WMT_NORMAL_WT 3
#define WMT_DEVICE_nGnRnE 4
#define WMT_DEVICE_nGnRE 5
#define WMT_DEVICE_GRE 6
#define WMT_NORMAL_iNC_oWB 7

// --- 单例 c_driver 类 ---

class Kernel
{
private:
    int has_upper = 0;
    int has_lower = 0;
    int has_symbol = 0;
    int has_digit = 0;
    pid_t pid;
    long int OP_INIT_KEY = 0x6660;
    long int OP_READ_MEM = 0x6661;
    long int OP_WRITE_MEM = 0x6662;
    long int OP_MODULE_BASE = 0x6663;
    long int OP_GET_IDZT = 0x6664;

public:
    选择配置 选择配置;
    int fd;
    Driver* kpm_driver = nullptr;  // KPM驱动对象
    Kernel();
    ~Kernel();
    uintptr_t get_Module_On();
    void 初始化读写(int pid);
    bool readv(uintptr_t addr, void *buffer, size_t size);
    bool writev(uintptr_t addr, void *buffer, size_t size);
    template <typename T>
    T Read(uintptr_t address);
    uintptr_t get_module_base(char *name);
    uintptr_t get_module_base2(char *name);
    uintptr_t getPtr64(uintptr_t addr);
    uintptr_t getPtr32(uintptr_t addr);
    float getFloat(uintptr_t addr);
    int getDword(uintptr_t addr);
    int WriteDword(long int addr, int value);
    float WriteFloat(long int addr, float value);
    void writefloat(unsigned long addr, float data);
    void writeptr(unsigned long addr, uintptr_t data);
    void writedword(unsigned long addr, int data);
    int getPID(const char *packageName);
    void getUTF8(char *buf, unsigned long namepy);
    bool init_key(char *key);
    char getByte(unsigned long addr);
    bool reopen_dev();
};



#include "Draw.h"
#include "结构体.h"
#include "骨骼.hpp"

struct smokeObject
{
    float screenDistance;
    Vec3 worldPos;
    Vec2 screenPos[8];
};
extern std::vector<smokeObject> smokeObjects;
extern std::vector<smokeObject> aimSmokeObjects;

class StringFloatMap
{
private:
    map<string, vector<float>> data;

public:
    void add(string key, float value1, float value2)
    {
        vector<float> values;
        values.push_back(value1);
        values.push_back(value2);
        data[key] = values;
    }

    void remove(string key)
    {
        data.erase(key);
    }

    bool exists(string key)
    {
        return data.find(key) != data.end();
    }

    string calculateKey(float 坐标X, float 坐标Y)
    {
        int maps = 999;
        string 键名;
        for (const auto &pair : data)
        {
            const vector<float> &values = pair.second;
            int result = (int)sqrt(pow(坐标X - values[0], 2) + pow(坐标Y - values[1], 2)) * 0.01;
            if (result < maps)
            {
                maps = result;
                键名 = pair.first;
            }
        }
        if (maps < 20)
        {
            return 键名;
        }
        return ""; // 如果没有满足条件的键名，则返回空字符串
    }
};

class Timer
{
private:
    std::map<std::string, int> timers;

public:
    void addTimer(const std::string &name, int seconds)
    {
        if (timers.find(name) == timers.end())
        {
            timers[name] = seconds;
        }
    }

    void updateTimers()
    {
        for (auto &timer : timers)
        {
            timer.second++;
        }
    }

    void checkAndRemoveTimers()
    {
        for (auto it = timers.begin(); it != timers.end();)
        {
            if (it->second == 1500)
            {
                it = timers.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    int getTimerSeconds(const std::string &name)
    {
        auto it = timers.find(name);
        if (it != timers.end())
        {
            return it->second;
        }
        return -1;
    }

    void removeTimer(const std::string &name)
    {
        timers.erase(name);
    }

    bool hasTimer(const std::string &name)
    { // 判断是否存在
        return timers.find(name) != timers.end();
    }

    void renameTimer(const std::string &oldName, const std::string &newName)
    {
        if (timers.find(oldName) != timers.end())
        {
            timers[newName] = timers[oldName];
            timers.erase(oldName);
        }
    }
};

class 计算
{
public:
    int 计算距离(FVector_class 自身坐标, FVector_class 对方坐标);
    D4DVector 计算屏幕坐标(float 矩阵[16], D3DVector 人物坐标, float px, float py);
    D2DVector 计算屏幕坐标2(float 矩阵[16], D3DVector 人物坐标, float px, float py);
    float 计算屏幕距离(D2DVector &坐标, float px, float py);
    骨骼数据 计算骨骼(FVector_class 自身相机, FVector_class (&骨骼)[17], float px, float py);
    void GetDistance(D3DVector Object, D3DVector Self, float *Distance); // 新
    D2DVector rotateCoord(float angle, float objRadar_x, float objRadar_y);
    D2DVector rotateCoord(FVector_class Enemy, FVector_class RealPerson);
};
// 绘图
class 绘图
{
    struct 颜色
    {
        ImColor 红色 = ImColor(255, 0, 0, 255);
        ImColor 白色 = ImColor(255, 255, 255, 255);
        ImColor 蓝色 = ImColor(0, 0, 255, 255);
        ImColor 绿色 = ImColor(0, 255, 0, 255);
        ImColor 黄色 = ImColor(255, 255, 0, 255);
        ImColor 黑色 = ImColor(0, 0, 0, 255);
    };

public:
    struct VecTor2
    {
        float x;
        float y;
        VecTor2()
        {
            this->x = 0;
            this->y = 0;
        }
        VecTor2(float x, float y)
        {
            this->x = x;
            this->y = y;
        }
        bool operator!=(const VecTor2 &Pos)
        {
            if (this->x != Pos.x || this->y != Pos.y)
            {
                return true;
            }
            return false;
        }
        VecTor2 operator+(float v) const
        {
            return VecTor2(x + v, y + v);
        }
        VecTor2 operator-(float v) const
        {
            return VecTor2(x - v, y - v);
        }
        VecTor2 operator*(float v) const
        {
            return VecTor2(x * v, y * v);
        }
        VecTor2 operator/(float v) const
        {
            return VecTor2(x / v, y / v);
        }
        VecTor2 &operator+=(float v)
        {
            x += v;
            y += v;
            return *this;
        }
        VecTor2 &operator-=(float v)
        {
            x -= v;
            y -= v;
            return *this;
        }
        VecTor2 &operator*=(float v)
        {
            x *= v;
            y *= v;
            return *this;
        }
        VecTor2 &operator/=(float v)
        {
            x /= v;
            y /= v;
            return *this;
        }
        VecTor2 operator+(const VecTor2 &v) const
        {
            return VecTor2(x + v.x, y + v.y);
        }
        VecTor2 operator-(const VecTor2 &v) const
        {
            return VecTor2(x - v.x, y - v.y);
        }
        VecTor2 operator*(const VecTor2 &v) const
        {
            return VecTor2(x * v.x, y * v.y);
        }
        VecTor2 operator/(const VecTor2 &v) const
        {
            return VecTor2(x / v.x, y / v.y);
        }
        VecTor2 &operator+=(const VecTor2 &v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }
        VecTor2 &operator-=(const VecTor2 &v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }
        VecTor2 &operator*=(const VecTor2 &v)
        {
            x *= v.x;
            y *= v.y;
            return *this;
        }
        VecTor2 &operator/=(const VecTor2 &v)
        {
            x /= v.x;
            y /= v.y;
            return *this;
        }
    };
    struct VecTor3
    {
        float x;
        float y;
        float z;
        VecTor3()
        {
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }
        VecTor3(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }
        bool operator!=(const VecTor3 &Pos)
        {
            if (this->x != Pos.x || this->y != Pos.y || this->z != Pos.z)
            {
                return true;
            }
            return false;
        }
        VecTor3 operator+(float v) const
        {
            return VecTor3(x + v, y + v, z + v);
        }
        VecTor3 operator-(float v) const
        {
            return VecTor3(x - v, y - v, z - v);
        }
        VecTor3 operator*(float v) const
        {
            return VecTor3(x * v, y * v, z * v);
        }
        VecTor3 operator/(float v) const
        {
            return VecTor3(x / v, y / v, z / v);
        }
        VecTor3 &operator+=(float v)
        {
            x += v;
            y += v;
            z += v;
            return *this;
        }
        VecTor3 &operator-=(float v)
        {
            x -= v;
            y -= v;
            z -= v;
            return *this;
        }
        VecTor3 &operator*=(float v)
        {
            x *= v;
            y *= v;
            z *= v;
            return *this;
        }
        VecTor3 &operator/=(float v)
        {
            x /= v;
            y /= v;
            z /= v;
            return *this;
        }
        VecTor3 operator+(const VecTor3 &v) const
        {
            return VecTor3(x + v.x, y + v.y, z + v.z);
        }
        VecTor3 operator-(const VecTor3 &v) const
        {
            return VecTor3(x - v.x, y - v.y, z - v.z);
        }
        VecTor3 operator*(const VecTor3 &v) const
        {
            return VecTor3(x * v.x, y * v.y, z * v.z);
        }
        VecTor3 operator/(const VecTor3 &v) const
        {
            return VecTor3(x / v.x, y / v.y, z / v.z);
        }
        VecTor3 &operator+=(const VecTor3 &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }
        VecTor3 &operator-=(const VecTor3 &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }
        VecTor3 &operator*=(const VecTor3 &v)
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            return *this;
        }
        VecTor3 &operator/=(const VecTor3 &v)
        {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            return *this;
        }
    };

    struct VecTor4
    {
        float x;
        float y;
        float z;
        float w;
        VecTor4()
        {
            this->x = 0;
            this->y = 0;
            this->z = 0;
            this->w = 0;
        }
        VecTor4(float x, float y, float z, float w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }
        bool operator!=(const VecTor4 &Pos)
        {
            if (this->x != Pos.x || this->y != Pos.y || this->z != Pos.z || this->w != Pos.w)
            {
                return true;
            }
            return false;
        }
        VecTor4 operator+(float v) const
        {
            return VecTor4(x + v, y + v, z + v, w + v);
        }
        VecTor4 operator-(float v) const
        {
            return VecTor4(x - v, y - v, z - v, w - v);
        }
        VecTor4 operator*(float v) const
        {
            return VecTor4(x * v, y * v, z * v, w * v);
        }
        VecTor4 operator/(float v) const
        {
            return VecTor4(x / v, y / v, z / v, w / v);
        }
        VecTor4 &operator+=(float v)
        {
            x += v;
            y += v;
            z += v;
            w += v;
            return *this;
        }
        VecTor4 &operator-=(float v)
        {
            x -= v;
            y -= v;
            z -= v;
            w -= v;
            return *this;
        }
        VecTor4 &operator*=(float v)
        {
            x *= v;
            y *= v;
            z *= v;
            w *= v;
            return *this;
        }
        VecTor4 &operator/=(float v)
        {
            x /= v;
            y /= v;
            z /= v;
            w /= v;
            return *this;
        }
        VecTor4 operator+(const VecTor4 &v) const
        {
            return VecTor4(x + v.x, y + v.y, z + v.z, w + v.w);
        }
        VecTor4 operator-(const VecTor4 &v) const
        {
            return VecTor4(x - v.x, y - v.y, z - v.z, w - v.w);
        }
        VecTor4 operator*(const VecTor4 &v) const
        {
            return VecTor4(x * v.x, y * v.y, z * v.z, w * v.w);
        }
        VecTor4 operator/(const VecTor4 &v) const
        {
            return VecTor4(x / v.x, y / v.y, z / v.z, w / v.w);
        }
        VecTor4 &operator+=(const VecTor4 &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }
        VecTor4 &operator-=(const VecTor4 &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }
        VecTor4 &operator*=(const VecTor4 &v)
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            w *= v.w;
            return *this;
        }
        VecTor4 &operator/=(const VecTor4 &v)
        {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            w /= v.w;
            return *this;
        }
    };

    颜色 颜色; // 颜色的类
    float PX, PY;
    float 黑点大小 = 1.0;
    float MIDDLE, BOTTOM, TOP;
    float left, right, top, top1, bottom;
    bool isAiming;
    void 初始化绘图(int X, int Y);
    void 初始化坐标(D4DVector &屏幕坐标, 骨骼数据 &骨骼);
    void 绘制方框(bool 是否可见,bool isboot);
    void 绘制人数(int 人机, int 真人, uintptr_t 自身);
    void 绘制距离(int 距离, int 队伍);
    void 绘制射线(bool 是否可见,骨骼数据 &骨骼);
    void 漏手模式();
    void 绘制血量(float 最大血量, float 当前血量, bool isbot);
    void 绘制名字(string 名字, bool isboot, float 计时, bool 是否掐雷, char *类名, int 阵营, int Bonecount, bool 是否自救,int 高级人机);
    void 绘制骨骼(骨骼数据 &骨骼, D4DVector &屏幕坐标, bool LineOfSightTo[15], int 距离, int Bonecount);
    void 绘制手持(int 手持, int 状态, int 子弹, int 最大子弹);
    void 绘制动作(int 状态);
    void 绘制连点位置(float 触摸范围X, float 触摸范围Y);
    void 绘制监听区域(float 触摸范围, float 触摸范围X, float 触摸范围Y);
    void 绘制车辆(D4DVector 屏幕坐标, int 距离, int CarrierID);
    void 绘制自瞄触摸范围(float 触摸范围, float 触摸范围X, float 触摸范围Y);
    void 绘制加粗字体(float size, float x, float y, ImColor color, ImColor color1, const char *str);
    void 绘制字体描边(float size, int x, int y, ImVec4 color, const char *str);
   // void RenderRadarScan(ImDrawList *draw_list, ImVec2 center, float radius, int numSegments, float &rotationAngle, float lineLength);
    void RenderRadarScan(ImDrawList* draw_list, ImVec2 center, float radius, int numSegments, float& rotationAngle, float lineLength, float yawRotation);
    void 绘制瞄准信息();
    void 绘制自救(float 自救倒计时);
    void 绘制头甲包(int id);
    void 瞬爆雷预测(int 距离, float 计时, bool 是否掐雷);
    bool WorldTurnScreen(VecTor2 &Screen, VecTor3 World, float Matrix[]);
    void ExplosionRange(D3DVector Obj, ImColor color, float Range, float thickn, float Matrix[]);
    void ExplosionRange1(D3DVector Obj, ImColor color, float Range, float thickn, float Matrix[]);
    void ExplosionRange2(D3DVector Obj, ImColor color, float Range, float thickn, float Matrix[]);
};

// 绘制
class 绘制
{

    int 驱动路线 = 0;
    struct ColorTable
    {
        float 方框颜色[4] = {0.0, 1.0, 0.0, 1.0};       // 掩体前改为绿色
        float 方框掩体颜色[4] = {1.0, 0, 0, 1.0};       // 掩体后保持红色
        float 射线颜色[4] = {0.0, 1.0, 0.0, 1.0};       // 掩体前改为绿色
        float 射线掩体颜色[4] = {1.0, 0, 0, 1.0};       // 掩体后保持红色
        float 骨骼颜色[4] = {0.0, 1.0, 0.0, 1.0};        // 掩体前改为绿色
        float 骨骼掩体颜色[4] = {1.0, 0.0, 0.0, 1.0};   // 掩体后保持红色
        float 血量颜色[4] = {0.0, 1.0, 0.0, 1.0};
        float 阵营颜色[4] = {1.0, 1.0, 0.0, 1.0};
        float 距离颜色[4] = {1.0, 1.0, 1.0, 1.0};
        float 名称颜色[4] = {1.0, 1.0, 1.0, 1.0};
    };
    struct map_node
    {
        long int start_addr;   // 起始地址
        long int end_addr;     // 结束地址
        struct map_node *next; // 下一个节点
    };

    struct 压枪
    {
        float m416;
        float scar_l;
        float aug;
        float 狗杂;
        float famas;
        float g36c;
        float m249;
        float akm;
        float m762;
        float 蜜獾;
        float pkm;
        float mg3;
        float mg_36;
        float p90;
        float uzi;
        float ump45;
        float vector;
        float 汤姆逊;
        float 野牛;
        float mini14;
        float sks;
        float m417;
        float mk20_h;
        float mk12;
        float vss;
        float 扫车 = 1.2f;
    };

public:
    std::mutex touchMutex;
    std::unordered_map<int, string> 武器名字 = {
        // ========== 突击步枪 ==========
        {101008, "M762突击步枪"},
        {101001, "AKM突击步枪"},
        {101004, "M416突击步枪"},
        {101003, "SCAR-L突击步枪"},
        {101002, "M16A4突击步枪"},
        {101009, "Mk47突击步枪"},
        {101006, "AUG突击步枪"},
        {101005, "Groza突击步枪"},
        {101010, "G36C突击步枪"},
        {101007, "QBZ突击步枪"},
        {101011, "AC-VAL突击步枪"},
        {101014, "ACE-32突击步枪"},
        {101012, "蜜獾突击步枪"},

        // ========== 射手步枪 ==========
        {103009, "SLR射手步枪"},
        {103005, "VSS射手步枪"},
        {103006, "Mini14射手步枪"},
        {103010, "QBU射手步枪"},
        {103004, "SKS射手步枪"},
        {103007, "MK14射手步枪"},
        {103014, "MK20-H射手步枪"},
        {103100, "MK12射手步枪"},
        {103013, "M417射手步枪"},

        // ========== 狙击枪 ==========
        {103012, "AMR狙击枪"},
        {103003, "AWM狙击枪"},
        {103002, "M24狙击枪"},
        {103016, "SVD狙击枪"},
        {103011, "莫甘娜辛狙击枪"},
        {103001, "Kar98K狙击枪"},
        {103008, "Win94狙击枪"},

        // ========== 轻机枪 ==========
        {105001, "M249轻机枪"},
        {105002, "DP-28轻机枪"},
        {105010, "MG3轻机枪"},

        // ========== 冲锋枪 ==========
        {102001, "UZI冲锋枪"},
        {102003, "Vector冲锋枪"},
        {100103, "PP-19冲锋枪"},
        {102007, "MP5K冲锋枪"},
        {102002, "UMP-45冲锋枪"},
        {102008, "AKS-74U冲锋枪"},
        {102009, "JS9冲锋枪"},
        {102004, "汤姆逊冲锋枪"},
        {102105, "P90冲锋枪"},
        {102005, "野牛冲锋枪"},

        // ========== 霰弹枪 ==========
        {104001, "S686霰弹枪"},
        {104002, "S1897霰弹枪"},
        {104003, "S12K霰弹枪"},
        {104004, "DBS霰弹枪"},
        {104100, "SPAS-12霰弹枪"},
    };
    static std::vector<ConfigItem> configItems;
    static std::vector<ConfigItem> boolConfigItems;

    static std::unordered_map<uintptr_t, int> lastBoneIndices;                // 记录每个敌人最后锁定的骨骼索引
    static std::unordered_map<uintptr_t, std::vector<int>> recentBoneIndices; // 记录最近锁定的骨骼索引
    string My_Gworld;
    string My_Matrix;
    string My_Matrix_Tol;
    string My_STExtraBaseCharacter;
    string My_libue4;
    string My_Mesh;
    uintptr_t 转十六进制(string value);
    static std::unordered_map<int, 武器触发条件> 武器触发配置;
    static std::unordered_map<int, 武器参数> 武器参数配置;
    int 距离字体大小 = 18;
    int 名称字体大小 = 18;
    int 手持字体大小 = 18;
    int 动作字体大小 = 18;
    int 物资字体大小 = 25;
    float 射线颜色[4] = {1.0, 1.0, 1.0, 1.0};
    float mk20, m417 = 1.0f;
    float 轻型压枪力度 = 1.f;
    float 拇指压枪力度 = 0.2f;
    float 垂直压枪力度 = 0.2f;
    float 直角压枪力度 = 0.2f;
    int 世界数量;
    
    float 运行负载 = 0.0f;
    int 网络延迟 = 0;
    std::chrono::steady_clock::time_point 启动时间;    
    // 如果使用方案3，添加这个方法
    
    // 新增圆角配置变量
    float UI圆角 = 5.0f;      // UI窗口圆角
    float 按键圆角 = 3.0f;    // 按钮圆角
        
    std::string updateshow;      // 更新内容
    long 卡密到期时间戳 = 0;     // 卡密到期时间戳
    bool 已登录 = false;         // 登录状态标志
    
    // 添加获取剩余时间的函数
    long 获取剩余天数() {
        if (!已登录 || 卡密到期时间戳 == 0) return 0;
        
        auto current_time = std::chrono::system_clock::now();
        auto current_timestamp = std::chrono::duration_cast<std::chrono::seconds>(current_time.time_since_epoch()).count();
        long 剩余秒数 = 卡密到期时间戳 - current_timestamp;
        
        if (剩余秒数 <= 0) {
            已登录 = false; // 标记为过期
            return 0;
        }
        
        return 剩余秒数 / (24 * 60 * 60);
    }
    
    long 获取剩余小时数() {
        if (!已登录 || 卡密到期时间戳 == 0) return 0;
        
        auto current_time = std::chrono::system_clock::now();
        auto current_timestamp = std::chrono::duration_cast<std::chrono::seconds>(current_time.time_since_epoch()).count();
        long 剩余秒数 = 卡密到期时间戳 - current_timestamp;
        
        if (剩余秒数 <= 0) {
            已登录 = false; // 标记为过期
            return 0;
        }
        
        return (剩余秒数 % (24 * 60 * 60)) / 3600;
    }
    
    long 获取剩余分钟数() {
        if (!已登录 || 卡密到期时间戳 == 0) return 0;
        
        auto current_time = std::chrono::system_clock::now();
        auto current_timestamp = std::chrono::duration_cast<std::chrono::seconds>(current_time.time_since_epoch()).count();
        long 剩余秒数 = 卡密到期时间戳 - current_timestamp;
        
        if (剩余秒数 <= 0) {
            已登录 = false; // 标记为过期
            return 0;
        }
        
        return (剩余秒数 % 3600) / 60;
    }
    
    float 骨骼距离限制 = 300;
    int 自瞄模式=999;
    int 防录屏=999;
    int 无后台开关=999;
    char 卡密[250];
    bool 漏打开关;
    bool Winorlose = false;
    ImVec2 Pos;
    int winWidth = 684;
    int winHeith = 896;
    bool 是否开启自瞄页面 = false;
    压枪 压枪力;
    压枪 预判度;
    float 握把[100];
    bool isView = true;
//    uintptr_t 解密数组;
    long int 解密模式 = 0x4000;
    int 被瞄准对象数量 = 0;
    int 头甲包文本高度 = 0;
    FILE *numSave = nullptr;
    char 悬浮窗标题[200];
    ColorTable Colorset[2]; // 颜色配置
    int pid;
    bool 线程 = 0; // 用于判断更新数据显示是否加载
    float PX, PY;  // 绘制用的分辨率
    float 真实PX, 真实PY;
    bool Validate;
    string 包名;
    string openzhao;
    bool pojie = false;
    float 物资颜色[4] = {0.5, 1.0, 0.0, 1.0}; // 改为黄绿色
    float 车辆颜色[4] = {0.0, 1.0, 0.0, 1.0};
    float 手持颜色[4] = {1.0, 1.0, 1.0, 1.0};
    StringFloatMap 手雷类;
    StringFloatMap 自救类;
    Timer 计时器; // 计时器
    char AimName[32];
    bool Shelter[14];
    ImFont *font_24 = nullptr; // 24像素字体
    Kernel 读写;
    地址 地址;    
    int 掩体刷新时间=10;
    uintptr_t 真人数量;
    开关 按钮;
    计算 计算;
    骨骼 *骨骼;
    绘图 绘图;
    自瞄 自瞄;
    连点配置 连点;
    备份 备份;
    std::mutex mtx;
    自瞄信息 自瞄函数[100];
    瞄准信息 被瞄信息[100];
    自身数据 自身数据; // 创建自身数据结构体
    对象地址 对象地址; // 创建对象地址结构体
    对象信息 对象信息; // 创建敌人信息结构体数组
    void 初始化绘制(string 包名, int 真实X, int 真实Y);
    float 倍镜判断(float Fov);
    float 倍镜压枪(int id);
    void 自瞄主线程();
    void 贝塞尔自瞄主线程();
    void 欧拉角自瞄主线程();
    void 连点主线程();
    void 无后座主线程();
    void 驱动自瞄主线程();
    int findminat();
    void 更新地址数据();
    void 多线程更新地址();
    void 更新对象地址();
    void 更新对象数据();
    void 绘制载具信息();
    void 运行绘制();
    ImColor floatArrToImColor(float arr[4]);
    void hide_process();
    string getBoxName(int id);
    string getBoxName1(int id);
    void OffScreen(ImDrawList *ImDraw, D4DVector Obj, float camear, ImU32 color, float Radius, float 距离);
    void GetTouch();       
    void 保存配置();
    void 读取配置();
    void 重置配置();
    void 读取用户选择配置();
    bool 自瞄触发(float 距离);
    
    
    
    void 无目标压枪();
    
    
    void 停止陀螺仪();
    void 重置陀螺仪();
    
    float 陀螺仪灵敏度补偿(float Fov);
    // 新增的FOV相关函数
    float 计算FovFactor(float currentFov);
    float 动态Fov范围调整(float baseRange, float currentFov);
       
    uintptr_t 解密数组;

    bool 已启用解密;
    uintptr_t 特征地址;
    std::vector<uintptr_t> 解密地址列表;
    
    // 解密函数声明
    void 查找解密地址();
    void 设置解密功能(bool 启用);
    bool 获取解密状态();
    void 重新扫描解密地址();
    void 显示解密数组选择窗口();
    void 选择解密数组(uintptr_t 数组地址);
    bool 解密数组选择窗口开启;   
    bool 连点触发(float 距离);
    const char *getMaterialName(char *name);
    int Cloudcheck();
    const char *Level(char *name);
    void InitShoot();
    FVector2D WorldToScreen(const FVector_class & WorldLocation);
    D2DVector WorldToScreen2(const FVector_class & WorldLocation);
    void SetTouchPositionFor连点();
   
};

class 布局
{
public:
    string 到期时间;
    void 读取配置();
    void 保存配置();
    void 开启悬浮窗();
    void 绘制悬浮窗();
    int 初始化程序();
    int MonitorVolumeKeys();
};
#endif
