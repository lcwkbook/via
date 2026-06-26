#ifndef PARADISE_API_H
#define PARADISE_API_H

#include <stdint.h>
#include <sys/types.h>
#include <stddef.h>

#define PARADISE_GYRO_MASK_GYRO (1u << 0)
#define PARADISE_GYRO_MASK_UNCAL (1u << 1)
#define PARADISE_GYRO_MASK_ALL (PARADISE_GYRO_MASK_GYRO | PARADISE_GYRO_MASK_UNCAL)

/* Hardware breakpoint definitions */
#define HWBP_MAX_POINTS 16
#define HWBP_MAX_RECORDS 0x100

enum hwbp_type {
    HWBP_BREAKPOINT_EMPTY = 0,
    HWBP_BREAKPOINT_R = 1,
    HWBP_BREAKPOINT_W = 2,
    HWBP_BREAKPOINT_RW = HWBP_BREAKPOINT_R | HWBP_BREAKPOINT_W,
    HWBP_BREAKPOINT_X = 4,
};

enum hwbp_len {
    HWBP_BREAKPOINT_LEN_1 = 1,
    HWBP_BREAKPOINT_LEN_2 = 2,
    HWBP_BREAKPOINT_LEN_3 = 3,
    HWBP_BREAKPOINT_LEN_4 = 4,
    HWBP_BREAKPOINT_LEN_5 = 5,
    HWBP_BREAKPOINT_LEN_6 = 6,
    HWBP_BREAKPOINT_LEN_7 = 7,
    HWBP_BREAKPOINT_LEN_8 = 8,
};

enum hwbp_scope {
    SCOPE_MAIN_THREAD = 0,
    SCOPE_OTHER_THREADS = 1,
    SCOPE_ALL_THREADS = 2
};

struct hwbp_record {
    uint8_t mask[18];
    uint64_t hit_count;
    uint64_t pc;
    uint64_t lr;
    uint64_t sp;
    uint64_t orig_x0;
    uint64_t syscallno;
    uint64_t pstate;
    uint64_t x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
    uint64_t x10, x11, x12, x13, x14, x15, x16, x17, x18, x19;
    uint64_t x20, x21, x22, x23, x24, x25, x26, x27, x28, x29;
    uint32_t fpsr;
    uint32_t fpcr;
    __uint128_t q0, q1, q2, q3, q4, q5, q6, q7, q8, q9;
    __uint128_t q10, q11, q12, q13, q14, q15, q16, q17, q18, q19;
    __uint128_t q20, q21, q22, q23, q24, q25, q26, q27, q28, q29;
    __uint128_t q30, q31;
};

struct hwbp_point_config {
    enum hwbp_type bt;
    enum hwbp_len bl;
    enum hwbp_scope bs;
    uint64_t hit_addr;
};

class paradise_driver {
private:
    pid_t pid;
    int fd;

    int install_driver_fd();
    void ensure_connected();

public:
    // 构造时连接 Paradise 驱动
    paradise_driver();
    
    // 构析方法
    ~paradise_driver();

    // 初始化目标 pid，读写前务必调用一次
    void initialize(pid_t target_pid);
    
    // 获取进程pid，传入进程名称，从内核层安全获取pid
    pid_t get_pid(const char *name);
    
    // 获取模块基址，传入模块名，从内核层安全获取模块基址
    uintptr_t get_module_base(const char *name);

    // 获取模块映射范围 [base, end)，end 为最高一段 VMA 的 vm_end，便于一次覆盖整个 so，如 libc 多段
    bool get_module_range(const char *name, uintptr_t *base_out, uintptr_t *end_out);

    // 获取模块结束地址，传入模块名，从内核层安全获取模块结束地址
    uintptr_t get_module_end(const char *name);

    /*
    用法示例：
        uintptr_t lo, hi;
        if (get_module_range("libc.so", &lo, &hi)) {
            // 映射包络为 [lo, hi)，按需分段读取
        }
        // 或仅获取结束地址：
        uintptr_t end = get_module_end("libc.so");
    */
    
    // 更新陀螺仪数据
    // bool gyro_update(float x, float y, uint32_t type_mask = PARADISE_GYRO_MASK_ALL, bool enable = true);
    
    // 检查进程是否存活 (alive_out: 1为存活，0为未存活)
    bool is_process_alive(pid_t check_pid, int *alive_out);
    
    // 隐藏或取消隐藏指定进程
    bool hide_process(pid_t target_pid, bool hide);
    
    // 隐藏或取消隐藏指定路径
    bool hide_path(const char *path, bool hide);
    
    // 获取进程列表位图
    bool list_processes(uint8_t *bitmap, size_t bitmap_size, size_t *process_count_out);
    
    // 硬件层读取数据，传入地址、接收指针、类型大小
    bool read(uintptr_t addr, void *buffer, size_t size);
    
    // 硬件层修改数据，传入地址、数据指针、类型大小
    bool write(uintptr_t addr, void *buffer, size_t size);

    // 内核层映射读取数据，传入地址、接收指针、类型大小
    bool read_fast(uintptr_t addr, void *buffer, size_t size);

    // 内核层映射修改数据，传入地址、数据指针、类型大小
    bool write_fast(uintptr_t addr, void *buffer, size_t size);

    // // 初始化触摸注入，传入用户屏幕分辨率用于坐标映射
    // bool touch_init(int screen_width, int screen_height);

    // // 手指按下
    // bool touch_down(int slot, int x, int y);

    // // 手指移动
    // bool touch_move(int slot, int x, int y);

    // // 手指抬起
    // bool touch_up(int slot);

    // // 销毁触摸注入
    // bool touch_destroy();

    // // 获取硬件断点/观察点槽位数量
    // bool hwbp_get_info(uint64_t *num_brps, uint64_t *num_wrps);

    // // 设置硬件断点，points 数组最多 HWBP_MAX_POINTS 个，hit_addr=0 的条目被忽略
    // bool hwbp_set(pid_t target_pid, struct hwbp_point_config *points, int count);

    // // 移除指定进程的所有硬件断点
    // bool hwbp_remove(pid_t target_pid);

    // 读取断点命中记录，point_index 指定哪个观测点 (0-15)
    // records_out 为输出缓冲区，max_records 为最多读取的记录数
    // 返回实际读取的记录数，-1 表示错误
    // int hwbp_read_records(pid_t target_pid, int point_index,
    //                       struct hwbp_record *records_out, int max_records,
    //                       uint64_t *hit_addr_out, int *total_records_out);

    // 模板方法，传入地址，返回地址上的值
    template <typename T>
    T read(uintptr_t addr)
    {
        T res{};
        if (this->read(addr, &res, sizeof(T)))
            return res;
        return {};
    }

    // 模板方法，传入地址，修改后的值
    template <typename T>
    bool write(uintptr_t addr, T value)
    {
        return this->write(addr, &value, sizeof(T));
    }

    // 基于 vmap 的模板读取
    template <typename T>
    T read_fast(uintptr_t addr)
    {
        T res{};
        if (this->read_fast(addr, &res, sizeof(T)))
            return res;
        return {};
    }

    // 基于 vmap 的模板写入
    template <typename T>
    bool write_fast(uintptr_t addr, T value)
    {
        return this->write_fast(addr, &value, sizeof(T));
    }
};

#endif
