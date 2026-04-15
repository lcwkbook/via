#include "辅助类.h"
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

bool Kernel::init_key(char *key)
{
    char buf[0x100];
    strcpy(buf, key);
    if (ioctl(fd, OP_INIT_KEY, buf) != 0)
    {
        return false;
    }
    return true;
}

Kernel::Kernel()
{
    fd = open("/dev/niuto01", O_RDWR);
}

Kernel::~Kernel()
{
    if (fd > 0)
    {
        close(fd);
    }
}

void Kernel::初始化读写(int pid)
{
    this->pid = pid;
    if (this->pid <= 0)
    {
        cout << "[-] 选定进程失败\n";
    }
    else
    {
        cout << "[-] 读写初始化成功\n";
    }
}

bool Kernel::readv(uintptr_t addr, void *buffer, size_t size)  
{
    if (addr < 0x10000000 || addr > 0xFFFFFFFFFF || addr <= 0xfff || addr == 0 || addr % 4 != 0)
    {
        return false;
    }
    struct
    {
        pid_t pid_value;
        uintptr_t addr_value;
        void *buffer_value;
        size_t size_value;
    } cm;
    cm.pid_value = this->pid;
    cm.addr_value = addr;
    cm.buffer_value = buffer;
    cm.size_value = size;
    ioctl(fd, OP_READ_MEM, &cm);
    return true;
}

bool Kernel::writev(uintptr_t addr, void *buffer, size_t size)
{
    if (addr < 0x10000000 || addr > 0xFFFFFFFFFF || addr <= 0xfff || addr == 0 || addr % 4 != 0)
    {
        return false;
    }
    struct
    {
        pid_t pid_value;
        uintptr_t addr_value;
        void *buffer_value;
        size_t size_value;
    } cm;
    cm.pid_value = this->pid;
    cm.addr_value = addr;
    cm.buffer_value = buffer;
    cm.size_value = size;
    ioctl(fd, OP_WRITE_MEM, &cm);
    return true;
}

uintptr_t Kernel::get_module_base(char *name)
{
    struct
    {
        pid_t pid_value;
        char *name_value;
        uintptr_t base_value;
    } mb;
    char buf[0x100];
    strcpy(buf, name);
    mb.pid_value = this->pid;
    mb.name_value = buf;
    ioctl(fd, OP_MODULE_BASE, &mb);
    return mb.base_value;
}

int Kernel::WriteDword(long int addr, int value)
{
    writev(addr, &value, 4);
    return 0;
}

float Kernel::WriteFloat(long int addr, float value)
{
    writev(addr, &value, 4);
    return 0;
}

template <typename T>
T Kernel::Read(uintptr_t addr)
{
    T res{};
    if (this->readv(addr, &res, sizeof(T)))
        return res;
    return {};
}

float Kernel::getFloat(uintptr_t addr)
{
    float var = 0;
    readv(addr, &var, 4);
    return var;
}

int Kernel::getDword(uintptr_t addr)
{
    int var = 0;
    readv(addr, &var, 4);
    return var;
}

uintptr_t Kernel::getPtr32(uintptr_t addr)
{
    unsigned int var = 0;
    readv(addr & 0xFFFFFFFFFF, &var, 4);
    return (var & 0xFFFFFFFFFF);
}

uintptr_t Kernel::getPtr64(uintptr_t addr)
{
    unsigned long var = 0;
    readv(addr, &var, 8);
    return (var);
}

void Kernel::writefloat(unsigned long addr, float data)
{
    writev(addr, &data, 4);
}

uintptr_t Kernel::get_module_base2(char *module_name)
{
    FILE *fp;
    char cmd[0x100] = "";
    uintptr_t ret = 0;
    snprintf(cmd, sizeof(cmd), "ls -l /proc/%d/map_files/ | grep '%s'", pid, module_name);
    fp = popen(cmd, "r");
    if (!fp)
    {
        std::cerr << "Failed to run command" << std::endl;
        return 0;
    }
    fscanf(fp, "%*s %*d %*s %*s %*d %*s %*s %lx-%*lx", &ret);
    pclose(fp);
    return ret;
}

uintptr_t Kernel::get_Module_On()
{
    struct
    {
        pid_t pid_value;
        char *name_value;
        uintptr_t base_value;
    } mb;
    ioctl(fd, OP_GET_IDZT, &mb);
    if (mb.base_value != 10086)
    {
        return 0;
    }
    return mb.base_value;
}

int Kernel::getPID(const char *packageName)
{
    int id = -1;
    DIR *dir;
    FILE *fp;
    char filename[64];
    char cmdline[64];
    struct dirent *entry;
    dir = opendir("/proc");
    while ((entry = readdir(dir)) != NULL)
    {
        id = atoi(entry->d_name);
        if (id != 0)
        {
            sprintf(filename, "/proc/%d/cmdline", id);
            fp = fopen(filename, "r");
            if (fp)
            {
                fgets(cmdline, sizeof(cmdline), fp);
                fclose(fp);
                if (strcmp(packageName, cmdline) == 0)
                {
                    return id;
                }
            }
        }
    }
    closedir(dir);
    return -1;
}

void Kernel::getUTF8(char *buf, unsigned long namepy)
{
    unsigned short buf16[16] = {0};
    readv(namepy, buf16, 28);
    unsigned short *pTempUTF16 = buf16;
    char *pTempUTF8 = buf;
    char *pUTF8End = pTempUTF8 + 32;
    while (pTempUTF16 < pTempUTF16 + 28)
    {
        if (*pTempUTF16 <= 0x007F && pTempUTF8 + 1 < pUTF8End)
        {
            *pTempUTF8++ = (char)*pTempUTF16;
        }
        else if (*pTempUTF16 >= 0x0080 && *pTempUTF16 <= 0x07FF && pTempUTF8 + 2 < pUTF8End)
        {
            *pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        }
        else if (*pTempUTF16 >= 0x0800 && *pTempUTF16 <= 0xFFFF && pTempUTF8 + 3 < pUTF8End)
        {
            *pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
            *pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        }
        else
        {
            break;
        }
        pTempUTF16++;
    }
}

char Kernel::getByte(unsigned long addr)
{
    char var = 0;
    readv(addr, &var, 1);
    return var;
}

bool Kernel::reopen_dev()
{
    if (fd > 0)
    {
        close(fd);
    }
    fd = open("/dev/niuto01", O_RDWR);
    return fd > 0;
}
