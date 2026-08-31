#include "HookRW.h"

HookRW::ReadMode HookRW::readMode = HookRW::ReadMode::No_cache;

    typedef struct _COPY_MEMORY {
		pid_t pid;
		uintptr_t addr;
		void *buffer;
		size_t size;
		int flags;
	} COPY_MEMORY, *PCOPY_MEMORY;
	
    typedef struct _GET_BASE {
        pid_t pid;
        const char *name;
        uintptr_t base;
    } GET_BASE, *PGET_BASE;
    
    typedef struct _GET_PID {
        const char *name;
        pid_t pid;
    } GET_PID, *PGET_PID;
    
    typedef struct _HIDE_PROCESS {
        pid_t pid;
    } HIDE_PROCESS, *PHIDE_PROCESS;
    
    enum OPERATIONS {
        OP_READ_MEM = 8001,
        OP_WRITE_MEM = 8002,
        OP_GET_BASE = 8003,
        OP_GET_PID = 8004,
        OP_PROCESS_HIDE = 8005,
        OP_PROCESS_RESTORE = 8006,
        OP_PROCESS_HIDE_KGSL = 8007,
        OP_PROCESS_HIDE_CTS = 8008
    };
    
    HookRW::HookRW() {
        fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd == -1) {
            perror("[-] 打开失败");
            _exit(EXIT_FAILURE);
        }
    }
    
    HookRW::~HookRW() {
        if(fd > 0) close(fd);
    }
    
    void HookRW::initialize(pid_t pid) {
		this->pid = pid;
	}
	
    bool HookRW::readv(uintptr_t addr, void *buffer, size_t size) {
        COPY_MEMORY cm;
        cm.pid = this->pid;
        cm.addr = addr;
        cm.buffer = buffer;
        cm.size = size;
        if (HookRW::readMode == HookRW::ReadMode::No_cache)
            cm.flags = 1;
        else if (HookRW::readMode == HookRW::ReadMode::Cache)
            cm.flags = 0;
        else
            return false;
        if (ioctl(fd, OP_READ_MEM, &cm) != 0) {
            return true;
        }
        return false;
    }
    
    bool HookRW::writev(uintptr_t addr, void *buffer, size_t size) {
        COPY_MEMORY cm;
        cm.pid = this->pid;
        cm.addr = addr;
        cm.buffer = buffer;
        cm.size = size;
        if (HookRW::readMode == HookRW::ReadMode::No_cache)
            cm.flags = 1;
        else if (HookRW::readMode == HookRW::ReadMode::Cache)
            cm.flags = 0;
        else
            return false;
        if (ioctl(fd, OP_WRITE_MEM, &cm) != 0) {
            return true;
        }
        return false;
    }
    
    uintptr_t HookRW::get_module_base(const char *name) {
        GET_BASE gb;
        gb.pid = this->pid;
        gb.name = name;
        if (ioctl(fd, OP_GET_BASE, &gb) != 0) {
            return gb.base;
        }
        return 0;
    }
    
    pid_t HookRW::get_process_pid(const char *name) {
        GET_PID gp;
        gp.name = name;
        if (ioctl(fd, OP_GET_PID, &gp) != 0) {
            return gp.pid;
        }
        return 0;
    }
    
    void HookRW::hide_process(pid_t selfpid) {
        HIDE_PROCESS hp;
        hp.pid = selfpid;
        ioctl(fd, OP_PROCESS_HIDE, &hp);
    }
    
    void HookRW::restore_process() {
        ioctl(fd, OP_PROCESS_RESTORE, NULL);
    }
    
    void HookRW::hide_kgsl(pid_t selfpid) {
        HIDE_PROCESS hp;
        hp.pid = selfpid;
        ioctl(fd, OP_PROCESS_HIDE_KGSL, &hp);
    }
    
    void HookRW::hide_cts(pid_t selfpid) {
        HIDE_PROCESS hp;
        hp.pid = selfpid;
        ioctl(fd, OP_PROCESS_HIDE_CTS, &hp);
    }
    
    void HookRW::setReadMode(ReadMode mode){
    	HookRW::readMode = mode;
    }
    
    HookRW::ReadMode HookRW::getReadMode(){
    	return HookRW::readMode;
    }
    
    float HookRW::getFloat(uintptr_t addr) {
    	float var = 0;
    	readv(addr,&var,4);
    	return(var);
    }
    
    int HookRW::getDword(uintptr_t addr) {
    	int var = 0;
    	readv(addr,&var,4);
    	return(var);
    }
    
    int HookRW::getWord(uintptr_t addr) {
    	int var = 0;
    	readv(addr,&var,2);
    	return(var);
    }
    
    bool HookRW::getBool(uintptr_t addr) {
    	bool var = false;
    	readv(addr,&var,1);
    	return(var);
    }
    
    uint8_t HookRW::getuint8_t(uintptr_t addr) {
    	uint8_t var = 0;
    	readv(addr,&var,1);
    	return(var);
    }
    
    uint64_t HookRW::getuint64_t(uintptr_t addr) {
    	uint64_t var = 0;
    	readv(addr,&var,8);
    	return(var);
    }
    
    uintptr_t HookRW::getPtr32(uintptr_t addr) {
    	unsigned int var = 0;
    	readv(addr&0xFFFFFFFFFF,&var,4);
    	return(var&0xFFFFFFFFFF);
    }
    
    uintptr_t HookRW::getPtr64(uintptr_t addr) {
    	uintptr_t var = 0;
    	readv(addr&0xFFFFFFFFFF,&var,8);
    	return(var&0xFFFFFFFFFF);
    }
    
    bool HookRW::writeFloat(uintptr_t addr,float data) {
    	return writev(addr,&data,4);
    }
    
    char *HookRW::getUTF8(uintptr_t addr) {
    	static char buf[64];
    	//不支持多次读写
    	unsigned short buf16[16] = { 0 };
    	readv(addr, buf16, 28);
    	unsigned short *pTempUTF16 = buf16;
    	char *pTempUTF8 = buf;
    	char *pUTF8End = pTempUTF8 + 32;
    	while (pTempUTF16 < pTempUTF16 + 28) {
    		if (*pTempUTF16 <= 0x007F && pTempUTF8 + 1 < pUTF8End) {
    			*pTempUTF8++ = (char) *pTempUTF16;
    		} else if (*pTempUTF16 >= 0x0080 && *pTempUTF16 <= 0x07FF && pTempUTF8 + 2 < pUTF8End) {
    			*pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
    			*pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
    		} else if (*pTempUTF16 >= 0x0800 && *pTempUTF16 <= 0xFFFF && pTempUTF8 + 3 < pUTF8End) {
    			*pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
    			*pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
    			*pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
    		} else {
    			break;
    		}
    		pTempUTF16++;
    	}
    	return buf;
    }
    
    std::vector<HookRW::MemSeg> HookRW::getMemSeg() {
        std::vector<HookRW::MemSeg> memSeg;
        
        struct MemPage {
    	    uintptr_t start;
        	uintptr_t end;
        	char flags[8];
        	char name[128];
        };
        
        char path[128];
        snprintf(path, sizeof(path), "/proc/%d/maps", this->pid);
        
        FILE *fp = fopen(path, "r");
        if (!fp) {
            printf("maps打开失败\n");
            return memSeg;
        }
        
        char* line = nullptr;
        size_t n = 0;
        MemPage mp;
        
        while (getline(&line, &n, fp) > 0) {
            // 初始化结构体
            memset(&mp, 0, sizeof(mp));
            
            sscanf(line, "%lx-%lx %s %*p %*p:%*p %*p %[^\n]", &mp.start, &mp.end, mp.flags, mp.name);
            
            if (strstr(mp.name, "[anon:scudo:secondary]")) {
                memSeg.push_back({mp.start, mp.end});
            }
        }
        
        free(line);
        fclose(fp);
        
        return memSeg;
    }
    
    static uintptr_t page = 0;
    
    uintptr_t HookRW::getpage(uintptr_t addr) {
        for (auto &mem : getMemSeg()) {
            uintptr_t memStart = mem.start;
            uintptr_t memEnd   = mem.end;
            for (uintptr_t pageStart = memStart; pageStart < memEnd; pageStart += 0x1000) {
                uintptr_t pageEnd = pageStart + 0x1000;
                if (pageEnd > memEnd) pageEnd = memEnd;
                for (uintptr_t p = pageStart; p + 0x8 <= pageEnd; p += 0x8) {
                    if (getPtr64(p) == addr) {
                        page = pageStart;
                        return p;
                    }
                }
            }
        }
        
        return 0;
    }
    
    uintptr_t HookRW::getPtr(uintptr_t addr) {
        if (!page) return 0;
        
        for (uintptr_t p = page; p <= page + 0x1000; p += 0x8) {
            if (getPtr64(p) == addr)
                return p;
        }
        
        return 0;
    }