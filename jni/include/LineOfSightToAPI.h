#ifndef LineOfSightTo_H
#define LineOfSightTo_H

struct FVector
{
	float X, Y, Z;

	FVector() : X(0), Y(0), Z(0){}
	FVector(float x, float y, float z) : X(x), Y(y), Z(z){}
	FVector operator+(const FVector& V) const
	{
		return FVector(X + V.X, Y + V.Y, Z + V.Z);
	}
	bool operator==(FVector& V) const
	{
		return X == V.X && Y == V.Y && Z == V.Z;
	}

	FVector operator*(float scalar) const
	{
		return FVector(X * scalar, Y * scalar, Z * scalar);
	}

	float operator[](int i) const
	{
		if (i >= 3) return 0.f;
		return ((float*)this)[i];
	}

	FVector operator-(const FVector& Other) const
	{
		return FVector(X - Other.X, Y - Other.Y, Z - Other.Z);
	}

	FVector operator/(const float Other) const
	{
		return FVector(X / Other, Y / Other, Z / Other);
	}

	FVector operator^(const FVector& V) const
	{
		return FVector
		(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X
		);
	}

	float operator|(const FVector& V) const
	{
		return X * V.X + Y * V.Y + Z * V.Z;
	}

	float Dot(const FVector& V) const
	{
		return X * V.X + Y * V.Y + Z * V.Z;
	}

	float Dist(const FVector& V) const
	{
		FVector Diff = V - *this;
		return sqrtf(Diff.X * Diff.X + Diff.Y * Diff.Y + Diff.Z * Diff.Z);
	}

	FVector Cross(const FVector& V) const
	{
		return FVector(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X
		);
	}
};

struct FVector2
{
	float X, Y;

	FVector2() : X(0), Y(0) {};

	FVector2(float x, float y) : X(x), Y(y) {};
};

typedef bool (*ReadAddr)(pid_t pid,uint64_t address, void *buffer, size_t size);
//内存读取函数指针
typedef void (*ImGuiAddr)(FVector2 a1, FVector2 a2);
//IMGUI绘制函数指针

void CreatePhysicsEngine(ReadAddr resolver, ImGuiAddr renderCallback, pid_t processId, uintptr_t moduleBase);
//Physx线程初始化函数

bool PerformRaycast(FVector origin, FVector direction);
//射线检测函数

void RenderPhysicsScene(const float viewProj[16], int screenWidth, int screenHeight);
//三角形网格模型绘制函数 参数 矩阵[16],分辨率X,分辨率Y

void InitPhysxMain(std::string AuthorizationCode);
//Main初始化函数 初始化生成核心数据 传入授权码


/*
掩体静态库使用教程

//IMGUI绘制函数
void DrawLine2D(FVector2 a1, FVector2 a2)
{
  ImGui::GetForegroundDrawList()->AddLine(ImVec2(a1.X, a1.Y), ImVec2(a2.X, a2.Y), ImColor(255,255,255,255), 1.0f);
}

//读取内存函数:当前以雪花内核驱动为例
//驱动读取需要支持多线程 不支持的情况下需要加入互斥锁
//std::mutex readMutex;//全局互斥锁
//注意:加了互斥锁需要你主线程读取内存函数也需要加上
bool Read(pid_t pid, uint64_t addr, void *buffer, size_t size) {
  //std::lock_guard<std::mutex> lock(readMutex); //使用不支持多线程驱动的开启互斥
  COPY_MEMORY cm;
  cm.pid = pid;
  cm.addr = addr;
  cm.buffer = buffer;
  cm.size = size;
  if (ioctl(fd, 0x801, &cm) != 0) {
    return false;
  }
  return true;
}

//参数1:读取内存函数指针
//参数2:IMGUI绘制函数指针
//参数3:游戏进程PID
//参数4:游戏模块UE4模块入口
CreatePhysicsEngine(&Read,&DrawLine2D,PID,UE4入口);


//main初始化函数-放在main.cpp程序入口初始化
InitPhysxMain("你的授权码");

//三角形网格绘制函数 放在绘制主线程
RenderPhysicsScene(矩阵[16], 分辨率X, 分辨率Y);

//射线检测函数 传入数据内容
//参数1:相机XYZ
//参数2:骨骼部位坐标
bool PerformRaycast(FVector origin, FVector direction);
*/


#endif