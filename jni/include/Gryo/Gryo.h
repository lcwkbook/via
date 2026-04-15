#ifndef GRYO_H
#define GRYO_H
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "../SysRead.h"
#include "Gryo_Module.h"
#include "../Ptrace/PtraceUtils.h"
struct sGyro
{
	bool enable;
	float x;
	float y;
};

class FGyro
{
private:
	SysRead Sys;
	uint64_t GryoAddr = NULL;
	sGyro gyro;
	static inline FGyro *instance = nullptr;

	uint64_t find_maps_vm(pid_t pid, const char *vm_name)
	{
		char maps_path[64];

		// 构建 maps 文件路径
		snprintf(maps_path, sizeof(maps_path), "/proc/%d/maps", pid);

		FILE *fp = fopen(maps_path, "r");
		if (!fp)
		{
			return 0;
		}

		char line[512];
		uint64_t start_addr = 0;
		int found = 0;

		// 逐行读取 maps 文件
		while (fgets(line, sizeof(line), fp))
		{
			// 查找包含目标名称的行
			if (strstr(line, vm_name))
			{
				// 解析地址范围: "start_addr-end_addr permissions ... [vm_name]"
				if (sscanf(line, "%lx-", &start_addr) == 1)
				{
					found = 1;
					break;
				}
			}
		}

		fclose(fp);

		return found ? start_addr : 0;
	}
	bool write_and_inject()
	{
		unsigned int Gryo_Len = sizeof(Gryo_Module); // 根据实际数组长度调整
		const char *file_path = "/data/app/libaa.so";

		// LOGI("开始写入脚本文件: %s", file_path);

		// 使用更底层的文件操作
		int fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0755);
		if (fd < 0)
		{
			// LOGE("无法创建文件: %s", file_path);
			return false;
		}

		ssize_t written = write(fd, Gryo_Module, Gryo_Len);
		close(fd);

		if (written != (ssize_t)Gryo_Len)
		{
			// LOGE("文件写入不完整: %zd/%u", written, SJZ_len);
			return false;
		}

		// LOGI("文件写入成功");

		// 确保权限正确
		chmod(file_path, 0777);

		int result = PtraceInject("system_server", "/data/app/libaa.so");

		// 删除文件
		if (unlink(file_path) == 0)
		{
			// LOGI("文件删除成功");
		}
		else
		{
			// LOGE("文件删除失败");
			return false;
		}

		return true;
	}

public:
	bool Init()
	{
	Gryo_Start:
		pid_t pid = get_pid_by_name("system_server");
		Sys.SetPid(pid); // 设置pid
		if (pid == 0 || pid < 0)
			return false;
		GryoAddr = find_maps_vm(pid, "FuckMem"); // 获取地址
		if (GryoAddr)
		{
			Unlock_Event();
			gyro = Sys.read<sGyro>(GryoAddr);
			// printf("GryoAddr=%lx enable=%d x=%f y=%f\n", GryoAddr, gyro.enable, gyro.x, gyro.y);
			return true;
		}
		if (!GryoAddr)
		{
			if (write_and_inject())
				goto Gryo_Start;
		}

		return false;
	}

	void Send_Event(float x, float y)
	{
		if (GryoAddr)
		{
			gyro = {true, x, y};
			Sys.writeMemory(GryoAddr, &gyro, sizeof(sGyro));
			// printf("enable=%d x=%f y=%f\n", gyro.enable, gyro.x, gyro.y);
		}
	}

	void Unlock_Event()
	{
		if (GryoAddr)
		{
			gyro = {false, 0, 0};
			Sys.writeMemory(GryoAddr, &gyro, sizeof(sGyro));
		}
	}

	sGyro GetGyroInfo() const
	{
		return gyro;
	}

	static FGyro *GetInstance()
	{
		if (!instance)
		{
			instance = new FGyro();
		}
		return instance;
	}
};

void GTA(uint64_t arg1, uint64_t arg2, float *out_result);
void GTB(float arg1, float arg2, float *out_result);

#endif // GRYO_H
