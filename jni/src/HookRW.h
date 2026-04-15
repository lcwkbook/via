#ifndef HookRW_HPP
#define HookRW_HPP

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

class HookRW {
	protected:
	int fd;
	pid_t pid;
	
    public:
    enum class ReadMode {
        No_cache, // 无缓存读取
        Cache // 有缓存读取
    };
    struct MemSeg {
	    uintptr_t start; // 地址开始
	    uintptr_t end; // 地址结尾
    };
    
    private:
    static ReadMode readMode;

    public:
    HookRW();
    ~HookRW();
    void initialize(pid_t pid);
    bool readv(uintptr_t address, void *buffer, size_t size);
    bool writev(uintptr_t address, void *buffer, size_t size);
    uintptr_t get_module_base(const char *name);
    pid_t get_process_pid(const char *name);
    void hide_process(pid_t selfpid);
    void restore_process();
    void hide_kgsl(pid_t selfpid);
    void hide_cts(pid_t selfpid);

    static void setReadMode(ReadMode mode);
    static ReadMode getReadMode();
    
    float getFloat(uintptr_t addr);
    int getDword(uintptr_t addr);
    int getWord(uintptr_t addr);
    bool getBool(uintptr_t addr);
    char *getUTF8(uintptr_t addr);
    uint8_t getuint8_t(uintptr_t addr);
    uint64_t getuint64_t(uintptr_t addr);
    uintptr_t getPtr64(uintptr_t addr);
    uintptr_t getPtr32(uintptr_t addr);
    bool writeFloat(uintptr_t addr,float data);
    
    std::vector<MemSeg> getMemSeg();
    uintptr_t getpage(uintptr_t addr);
    uintptr_t getPtr(uintptr_t addr);

    template <typename... Args>//必须放在头文件
    uintptr_t jumpPoint(uintptr_t addr, Args... args) {
        // 静态断言：确保所有后续参数都是 int 类型
        static_assert(
            (std::is_same_v<Args, int> && ...),  // C++17 折叠表达式
            "链条类型错误!"
        );
    	uintptr_t result = addr;
        // 处理后续
        if constexpr (sizeof...(args) > 0) {
            // C++17 的 if constexpr
            (..., (readv((result + args) & 0xFFFFFFFFFF, &result, 8)));// 折叠表达式输出所有参数
        }
        return result;
    }
};

extern HookRW *rw;

#endif