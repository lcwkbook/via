#include <sys/uio.h>
#include <unistd.h>
#include <sys/syscall.h>

class SysRead {
private:
    int target_pid;
    // 系统调用号定义
#if defined(__arm__)
    static const int process_vm_readv_syscall = 376;
    static const int process_vm_writev_syscall = 377;
#elif defined(__aarch64__)
    static const int process_vm_readv_syscall = 270;
    static const int process_vm_writev_syscall = 271;
#elif defined(__i386__)
    static const int process_vm_readv_syscall = 347;
    static const int process_vm_writev_syscall = 348;
#else
    static const int process_vm_readv_syscall = 310;
    static const int process_vm_writev_syscall = 311;
#endif

    // 内联汇编系统调用
#define __asm_syscall(...)                                               \
    do                                                                   \
    {                                                                    \
        __asm__ __volatile__("svc 0"                                     \
                             : "=r"(x0) : __VA_ARGS__ : "memory", "cc"); \
        return x0;                                                       \
    } while (0)

    __attribute__((always_inline)) static inline long
    __syscall6(long n, long a, long b, long c, long d, long e, long f) {
        register long x8 __asm__("x8") = n;
        register long x0 __asm__("x0") = a;
        register long x1 __asm__("x1") = b;
        register long x2 __asm__("x2") = c;
        register long x3 __asm__("x3") = d;
        register long x4 __asm__("x4") = e;
        register long x5 __asm__("x5") = f;
        __asm_syscall("r"(x8), "0"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5));
    }

    // 读取核心函数
    int pvm(void *address, void *buffer, size_t size) const {
        struct iovec local[1];
        struct iovec remote[1];

        local[0].iov_base = buffer;
        local[0].iov_len = size;
        remote[0].iov_base = address;
        remote[0].iov_len = size;

        return (int) __syscall6(__NR_process_vm_readv, (long) target_pid, (long) local, (long) 1, (long) remote,
                                (long) 1, (long) 0);
    }

    // 写入核心函数
    int pvm2(void *address, void *buffer, size_t size) const {
        struct iovec local[1];
        struct iovec remote[1];

        local[0].iov_base = buffer;
        local[0].iov_len = size;
        remote[0].iov_base = address;
        remote[0].iov_len = size;

        return (int) syscall(process_vm_writev_syscall, target_pid, local, 1, remote, 1, 0);
    }

    // 内存读取
    bool vm_readv(uintptr_t address, void *buffer, size_t size) const {
        return pvm(reinterpret_cast<void *>(address), buffer, size) > 0;
    }

    // 内存写入
    bool vm_writev(uintptr_t address, void *buffer, size_t size) const {
        return pvm2(reinterpret_cast<void *>(address), buffer, size) > 0;
    }

public:
    // 构造函数
    SysRead(int pid = 0) : target_pid(pid) {
    }

    // 设置目标PID
    void SetPid(int pid) {
        target_pid = pid;
    }

    // 获取当前目标PID
    int GetPid() const {
        return target_pid;
    }

    // 读取内存模板函数
    template<typename T>
    T read(uintptr_t addr) const {
        T res{};
        if (target_pid > 0) {
            vm_readv(addr, &res, sizeof(T));
        }
        return res;
    }

    template<typename T>
    bool read(uint64_t address, T *buffer, size_t size) {
        return vm_readv(address, reinterpret_cast<void *>(buffer), size);
    }

    template<typename T>
    bool read(uintptr_t addr, T *buffer) {
        return vm_readv(addr, buffer, sizeof(T));
    }

    template<typename T>
    void AddScatterReadRequest(uint64_t address, T *buffer) {
        vm_readv(address, reinterpret_cast<void *>(buffer), sizeof(T));
    }

    template<typename T>
    void AddScatterReadRequest(uint64_t address, T *buffer, size_t size) {
        vm_readv(address, reinterpret_cast<void *>(buffer), size);
    }

    // 写入内存模板函数
    template<typename T>
    bool write(uintptr_t addr, const T &value) const {
        if (target_pid <= 0)
            return false;
        return vm_writev(addr, const_cast<T *>(&value), sizeof(T));
    }

    // 读取原始内存数据
    bool readMemory(uintptr_t addr, void *buffer, size_t size) const {
        if (target_pid <= 0)
            return false;
        return vm_readv(addr, buffer, size);
    }


    // 写入原始内存数据
    bool writeMemory(uintptr_t addr, const void *buffer, size_t size) const {
        if (target_pid <= 0)
            return false;
        return vm_writev(addr, const_cast<void *>(buffer), size);
    }

    bool InjectShellCode(uintptr_t address, uint8_t *shellcode, size_t shellcode_size) {
        struct iovec iov = {
            .iov_base = shellcode,
            .iov_len = shellcode_size // 直接使用传入的字节长度
        };
        if (target_pid <= 0)
            return false;
        return vm_writev(address, iov.iov_base, iov.iov_len);
    }

    // 读取字符串
    std::string readString(uintptr_t addr, size_t max_length = 256) const {
        if (target_pid <= 0)
            return "";

        std::string result;
        char buffer[128];
        size_t read_count = 0;

        while (read_count < max_length) {
            size_t to_read = std::min(sizeof(buffer), max_length - read_count);
            if (!readMemory(addr + read_count, buffer, to_read)) {
                break;
            }

            // 查找null终止符
            for (size_t i = 0; i < to_read; ++i) {
                if (buffer[i] == '\0') {
                    result.append(buffer, i);
                    return result;
                }
            }

            result.append(buffer, to_read);
            read_count += to_read;
        }

        return result;
    }
};
