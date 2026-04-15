#include "PhysxPublic.h"
#include "libPhysxShared.h"
#include <cstring>

#include <sys/mman.h>
#include <sys/types.h>
#include <linux/memfd.h>
#include <linux/unistd.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>

int shm_open_anon(void)
{
    return syscall(__NR_memfd_create, "shm_anon", (unsigned int)(MFD_CLOEXEC));
}

void *fdlopen(int fd, int mode) {
    char BUF[PATH_MAX];
    if (isatty(fd)) {
        errno = EBADFD;
        return NULL;
    }
    if (access("/proc", F_OK) < 0) {
        errno = ENOSYS;
        return NULL;
    }
    snprintf(BUF, PATH_MAX, "/proc/self/fd/%d", fd);
    if (access(BUF, F_OK) < 0) {
        errno = EBADF;
        return NULL;
    } else if (access(BUF, R_OK) < 0) {
        errno = EACCES;
        return NULL;
    }
    struct stat st;
    stat(BUF, &st);
    if (S_ISDIR(st.st_mode)) {
        errno = EISDIR;
        return NULL;
    }
    return dlopen(BUF, mode);
}

void *dlblobb(const void *blob, size_t len){
    int fd = shm_open_anon();
    ftruncate(fd, len);
    void *mem = mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd, 0);
    memcpy(mem, blob, len);
    munmap(mem, len);
    void *so = fdlopen(fd, RTLD_LAZY);
    close(fd);
    return so;
}


//鸭子掩体渲染动态库加载
PhysxVerifyFunc PhysxVerify = nullptr;
StartLoadModelFunc StartLoadModel = nullptr;
LineTraceSingleFunc LineTraceSingle = nullptr;
RaycastTrianglesFunc RaycastTriangles = nullptr;
void* g_physx_handle = nullptr;

int loadDK() {
    // 从内存中加载得到句柄
    g_physx_handle = dlblobb(&libPhysxShared, sizeof(libPhysxShared));
    if (!g_physx_handle) {
        std::cerr << "动态库加载失败：" << dlerror() << std::endl;
        return 0;
    }

    PhysxVerify = (PhysxVerifyFunc)dlsym(g_physx_handle, "PhysxVerify");
    if (!PhysxVerify) {
        std::cerr << "获取 PhysxVerify 失败：" << dlerror() << std::endl;
        dlclose(g_physx_handle);
        g_physx_handle = nullptr;
        return 0;
    }
    
    StartLoadModel = (StartLoadModelFunc)dlsym(g_physx_handle, "StartLoadModel");
    if (!StartLoadModel) {
        std::cerr << "获取 StartLoadModel 失败：" << dlerror() << std::endl;
        dlclose(g_physx_handle);
        g_physx_handle = nullptr;
        return 0;
    }

    LineTraceSingle = (LineTraceSingleFunc)dlsym(g_physx_handle, "LineTraceSingle");
    if (!LineTraceSingle) {
        std::cerr << "获取 LineTraceSingle 失败：" << dlerror() << std::endl;
        dlclose(g_physx_handle);
        g_physx_handle = nullptr;
        return 0;
    }

    void* factoryPtr = dlsym(g_physx_handle, "RaycastTriangles");
    if (factoryPtr) {
        auto factory = reinterpret_cast<void*(*)()>(factoryPtr);
        RaycastTriangles = reinterpret_cast<RaycastTrianglesFunc>(factory());
    } else {
        std::cerr << "获取 RaycastTrianglesFactory 失败：" << dlerror() << std::endl;
    }
    
    if (!RaycastTriangles) {
        std::cerr << "获取 RaycastTriangles 失败" << std::endl;
        dlclose(g_physx_handle);
        g_physx_handle = nullptr;
        return 0;
    }
    
    return 1;
}

void unloadDK() {
    if (g_physx_handle) {
        dlclose(g_physx_handle);
        g_physx_handle = nullptr;
        PhysxVerify = nullptr;
        StartLoadModel = nullptr;
        LineTraceSingle = nullptr;
        RaycastTriangles = nullptr;
    }
}
