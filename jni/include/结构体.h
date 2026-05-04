#ifndef 结构体_H
#define 结构体_H

#include <cstring>
#include <iostream>
using namespace std;
#include "matrix.h"
#include "rotator.h"
#include "vector.h"
#include "transform.h"

// 本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险
struct Rotator
{
    float Pitch;
    float Yaw;
    float Roll;
    Rotator() {}
    Rotator(float _P, float _Y, float _R) : Pitch(_P), Yaw(_Y), Roll(_R) {}
};

class FRotator
{
public:
    FRotator()
        : Pitch(0.f), Yaw(0.f), Roll(0.f)
    {
    }
    FRotator(float _Pitch, float _Yaw, float _Roll)
        : Pitch(_Pitch), Yaw(_Yaw), Roll(_Roll)
    {
    }
    ~FRotator()
    {
    }
    float Pitch;
    float Yaw;
    float Roll;
    inline FRotator Clamp()
    {
        if (Pitch > 180)
        {
            Pitch -= 360;
        }
        else
        {
            if (Pitch < -180)
            {
                Pitch += 360;
            }
        }
        if (Yaw > 180)
        {
            Yaw -= 360;
        }
        else
        {
            if (Yaw < -180)
            {
                Yaw += 360;
            }
        }
        if (Pitch > 89)
        {
            Pitch = 89;
        }
        if (Pitch < -89)
        {
            Pitch = -89;
        }
        while (Yaw < 180)
        {
            Yaw += 360;
        }
        while (Yaw > 180)
        {
            Yaw -= 360;
        }
        Roll = 0;
        return FRotator(Pitch, Yaw, Roll);
    }
    inline float Length()
    {
        return sqrtf(Pitch * Pitch + Yaw * Yaw + Roll * Roll);
    }
    FRotator operator+(FRotator v)
    {
        return FRotator(Pitch + v.Pitch, Yaw + v.Yaw, Roll + v.Roll);
    }
    FRotator operator-(FRotator v)
    {
        return FRotator(Pitch - v.Pitch, Yaw - v.Yaw, Roll - v.Roll);
    }
};

struct ConfigItem
{
    const char *name;
    void *value;
    enum
    {
        INT,
        FLOAT,
        BOOL
    } type;
};

struct 开关
{
    bool 绘制 = false;
    bool 超体职业;
    /*  float 第一人称 = 125;
      float 加速数值 =1.1;
      float 第三人称 = 100;
      float 开镜广角 = 10;*/
    bool hide_process = false;
    bool 烟雾倒计时 = false;
    bool 方框 = false;
    bool 漏手模式;
    bool 射线 = false;
    bool 名字 = false;
    bool 距离 = false;
    bool 视角追踪 = false;
    bool 忽略人机 = false;
    bool 血量 = false;
    int 血条绘图 = 0;
    int 手雷样式 = 0;

    bool 瞬爆雷预测;
    bool 自救倒计时;
    bool 人数 = true;
    bool 调试 = false;
    bool 骨骼 = false;
    bool 手持 = false;
    bool 手持2 = false;
    int 手持绘图 = 0;
    bool 车辆 = false;
    bool 载具血量 = false;
    bool 载具油量 = false;
    bool 盒子 = false;
    bool 手雷预警 = false;
    bool 被瞄预警 = false;
    bool 背敌预警 = false;
    bool 全图人数 = false;
    bool 雷达 = false;
    //    bool 人物加速 = false;
    bool 隐藏方框背景 = false;
    bool 动作 = false;
    bool Debug = false;
    int Debug模式 = 0;
    int 当前配置 = 0;
    bool 加速;
    bool 物资总开关 = false;
    int 绘制最大距离 = 500;
    bool 刷新灵敏度 = true;
    bool 其他物资 = false;
    bool 显示步枪 = false;
    bool 冲锋枪械 = false;
    bool 狙击枪械 = false;
    bool 散弹枪械 = false;
    bool 显示子弹 = false;
    bool 显示556子弹 = false;
    bool 显示762子弹 = false;
    bool 显示9mm子弹 = false;
    bool 显示45mm子弹 = false;
    bool 显示霰弹 = false;
    bool 显示信号弹 = false;
    bool 显示箭矢 = false;
    bool 显示倍镜 = false;
    bool 显示扩容 = false;
    bool 显示配件 = false;
    bool 显示子弹袋 = false;
    bool 显示箭袋 = false;
    bool 显示激光瞄准器 = false;
    bool 显示轻型握把 = false;
    bool 显示半截握把 = false;
    bool 显示UZI枪托 = false;
    bool 显示狙击枪托 = false;
    bool 显示步枪枪托 = false;
    bool 显示狙击枪补偿器 = false;
    bool 显示狙击枪消焰器 = false;
    bool 显示狙击枪消音器 = false;
    bool 显示步枪消音器 = false;
    bool 显示步枪补偿器 = false;
    bool 显示步枪消焰器 = false;
    bool 显示冲锋枪消音器 = false;
    bool 显示冲锋枪消焰器 = false;
    bool 显示拇指握把 = false;
    bool 显示垂直握把 = false;
    bool 显示直角握把 = false;
    bool 显示撞火枪托 = false;
    bool 显示霰弹快速 = false;
    bool 显示鸭嘴枪口 = false;
    bool 显示霰弹收束 = false;
    bool 显示药品 = false;
    bool 显示类名;
    bool 不想吃鸡;
    bool 延伸;

    // 新添加
    //  刷刀功能相关变量
    // bool 人物聚点;
    // bool 人物无后;
    // bool 人物防抖;
    // bool 人物射速;
    // bool 人物瞬击;
    // bool 人物加速;
    // bool 广角设置;
    // float 速度值 = 1.0f;
    // float 第三人称 = 105.0f;
    // float 第一人称 = 100.0f;
    // bool 自动开火;
    // bool 内透;
    bool 解密模式;

    bool 解密;

    bool 精英勋章;
    bool 显示医疗箱 = false;
    bool 显示急救包 = false;
    bool 显示绷带 = false;
    bool 显示可乐 = false;
    bool 显示肾上腺素 = false;
    bool 显示止痛药 = false;
    bool 显示防具 = false;
    bool 显示三级头 = false;
    bool 显示三级甲 = false;
    bool 显示三级包 = false;
    bool 投掷物品 = false;
    bool 超级物资箱 = false;
    bool 爆炸猎弓 = false;
    bool 绘制信号枪 = false;
    // 新添加
    bool 隐藏古墓已开启 = false; // 新增
    bool 显示古墓篮子 = false;
    bool 显示古墓首饰盒 = false;
    bool 显示古墓宝箱 = false;
    bool 显示古墓精致宝箱 = false;
    bool 显示古墓华贵宝箱;
    // 在你的开关结构体中添加这两行
    bool 显示古墓树木 = false;
    bool 显示自救器 = false;
    bool 显示飞索 = false;
    bool 显示黑色物资箱 = false;
    bool 自定义物资开关 = false;
    float 物资字体大小 = 14.0f; // 默认值可以根据你的需求调整

    bool 绘制金插 = false;
    bool 绘制宝箱 = false;
    bool 绘制药箱 = false;
    bool 绘制武器箱 = false;
    bool 绘制空投 = false;
    bool 头甲包显示 = false; // 耐久
    bool 盒子物资 = false;
    bool 背包容量 = false;
    bool 头甲包显示2 = false;
    bool 隐藏已开启 = false;

    bool 显示对局信息 = false;
    //   float 速度值 = 1.15;
    float 雷达X = 300;
    float 雷达Y = 400;
    float rotationAngle = 0.0f;
    float 方框粗细 = 1.0f;
    float 射线粗细 = 0.1f;
    float 骨骼粗细 = 2.0f;
    bool 自瞄选项 = false;
    int 帧率选项 = 90;
    int 当前帧率 = 90;
    int VehicleID;
    float 测试数值 = 0.0f;
};

struct 地址
{
    uintptr_t libdi;
    uintptr_t libue4;
    uintptr_t 世界地址;
    uintptr_t 类地址;
    uintptr_t 解密地址;
    uintptr_t 自身地址;
    uintptr_t 矩阵地址;
    uintptr_t 矩阵地址_Tol;
    uintptr_t 数组地址;
    uintptr_t 全图人数;
    uintptr_t 队伍数;
    uintptr_t 真实玩家;
    uintptr_t 相机;
};

struct FMatrix
{
    float M[4][4];
};

struct D2DVector
{
    float X;
    float Y;
    D2DVector()
    {
        this->X = 0;
        this->Y = 0;
    }
    D2DVector(float x, float y)
    {
        this->X = x;
        this->Y = y;
    }
};

struct D3DVector
{
    float X;
    float Y;
    float Z;
    D3DVector()
    {
        this->X = 0;
        this->Y = 0;
        this->Z = 0;
    }
    D3DVector(float x, float y, float z)
    {
        this->X = x;
        this->Y = y;
        this->Z = z;
    }
};

struct D4DVector
{
    float X;
    float Y;
    float Z;
    float W;
    D4DVector()
    {
        this->X = 0;
        this->Y = 0;
        this->Z = 0;
        this->W = 0;
    }
    D4DVector(float x, float y, float z, float w)
    {
        this->X = x;
        this->Y = y;
        this->Z = z;
        this->W = w;
    }
};

struct 自瞄信息
{
    FVector_class 瞄准坐标;
    D3DVector 人物向量;
    float 准心距离 = 10000;
    float 距离 = 10000;
    D2DVector 对象骨骼;
    long int Bone;
    long int Human;
    int 血量;
    bool isview = true;
    string 名字;
    float 头;
    float 甲;
    int 阵营;
    long 头甲包地址;
    D3DVector 骨骼坐标[17];
    int 掩体部位;
};

struct BoneStruct
{
    D3DVector Pos;       // 世界坐标
    D2DVector ScreenPos; // 屏幕坐标
    bool CanSee = true;  // 可见判断
};

struct 瞄准信息
{
    float 距离;
    string 名字;
    string 瞄准武器;
};

struct FTransform
{
    D4DVector Rotation;
    D3DVector Translation;
    float chunk;
    D3DVector Scale3D;
};

struct 备份
{
    float 自瞄速度 = 20.f; //&绘制.自瞄.自瞄速度
    float 压枪力度 = 0.f;  //&绘制.自瞄.压枪力度
    float mk20压枪 = 0.f;  //&绘制.mk20
};

struct 连点配置
{
    float 触摸范围X = 300.0f;
    float 触摸范围Y = 300.0f;
    float 监听位置X = 300.0f;
    float 监听位置Y = 300.0f;
    float 监听边长 = 50.0f;
    float 触摸范围 = 50.0f; // 圆的半径大小
    int 连点速度 = 100;
    bool 启用连点 = false;
    bool 启用狙击连点 = false;
    int 充电口方向 = 0; // 添加方向参数
    bool 连点位置 = false;
    bool 监听位置 = false;
    bool 开火判断 = false;
    bool 监听区域内 = false;
    bool 跟随自瞄连点 = false;
};

struct Rect
{
    // 成员变量（直接存储起点和终点坐标）
    int startX; // 起点X
    int startY; // 起点Y
    int endX;   // 终点X
    int endY;   // 终点Y

    // ------------- 构造函数 -------------
    // 默认构造（全0初始化）
    Rect() {}
    void 初始化(int X1, int X2, int Y1, int Y2)
    {
        this->startX = X1;
        this->startY = Y1;
        this->endX = X2;
        this->endY = Y2;
    }
    // 判断点是否在矩形内（包含边界）
    bool contains(float x, float y) const
    {
        float rectX = std::min(startX, endX);
        float rectY = std::min(startY, endY);
        float rectWidth = std::abs(endX - startX);
        float rectHeight = std::abs(endY - startY);
        return x >= rectX && x < rectX + rectWidth &&
               y >= rectY && y < rectY + rectHeight;
    }
};

struct 自瞄
{

    // bool 无目标压枪开关 = false;

    bool 初始化 = false;
    bool 触摸位置 = false;

    bool 无目标压枪开关 = false;

    bool 动态自瞄 = false;
    bool 准星射线 = false;
    bool 倒地不瞄 = false;
    bool 掉血自瞄 = false;
    bool 自瞄控件 = false;
    bool 喷子自瞄 = false;
    bool 狙击自瞄 = false;
    bool 人机不瞄 = false;
    bool 框内自瞄 = false;
    bool 软锁自瞄 = false;
    bool 持续自瞄中 = false;
    bool 隐藏自瞄圈 = false;
    bool 烟雾不瞄 = false;
    bool 指哪打哪 = false;
    bool 随机触摸点 = false;
    bool 持续锁定 = false;
    bool 扫车不瞄 = false;
    bool 定速巡航 = false;
    int 定速巡航速率 = 0;
    int 自瞄条件 = 0;
    int 充电口方向 = 0;
    int 瞄准优先 = 0;
    int 瞄准部位 = 0;
    int 喷子自瞄条件 = 0;
    int lastBoneIndex = -1;
    float 当前自瞄范围 = 500.0f;
    float 喷子自瞄范围 = 500.0f;
    float 自瞄范围 = 500.0f;
    float 开镜自瞄范围 = 500.0f;
    float 动态范围 = 200.0f;
    float 触摸范围 = 200.0f;
    float 压枪力度 = 2.15f;
    float 自瞄速度 = 8.f;     // 默认自瞄速度
    float 腰射自瞄速度 = 2.f; // 腰射自瞄速度（更灵敏）
    float 开镜自瞄速度 = 3.f; // 开镜自瞄速度（更平滑）
    float 预判力度 = 1.8f;
    float 扫车预判 = 0.f;
    int 瞄准目标 = -1;
    float 触摸范围X = 1500.0f;
    float 触摸范围Y = 650.0f;

    // 新添加

    float 地形适应系数 = 0.5f; // 根据地形高度差调整瞄准精度

    int 瞄准对象数量 = 0;
    int 瞄准总数量 = 0;

    float 掉血自瞄数率 = 8.0f;

    float 喷子距离限制 = 5.0f;
    float 趴下位置调节 = 1.0f;
    float 触摸采样率 = 800;

    float 腰射距离限制 = 25.0f;
    float 自瞄距离限制 = 300.0f;
    float 连点速度 = 100.0f;
    bool 开启喷子连点;
    bool 开启单发狙连点;
    float 三倍压枪 = 0.f;
    float 四倍压枪 = 0.f;
    float 六倍压枪 = 0.f;

    bool 自动适应灵敏度 = false;
    bool 自适应腰射灵敏度 = false; // 新增：是否启用自适应腰射灵敏度
    int 适应系数 = 21;
    Rect 连点位置;
};

struct 骨骼数据
{
    D2DVector Head;
    D2DVector Chest;
    D2DVector Pelvis;
    D2DVector Left_Shoulder;
    D2DVector Right_Shoulder;
    D2DVector Left_Elbow;
    D2DVector Right_Elbow;
    D2DVector Left_Wrist;
    D2DVector Right_Wrist;
    D2DVector Left_Thigh;
    D2DVector Right_Thigh;
    D2DVector Left_Knee;
    D2DVector Right_Knee;
    D2DVector Left_Ankle;
    D2DVector Right_Ankle;

    // 返回成员引用的指针数组（用于遍历）
    std::vector<D2DVector *> 获取所有骨骼指针()
    {
        return {
            &Head, &Chest, &Pelvis,
            &Left_Shoulder, &Right_Shoulder,
            &Left_Elbow, &Right_Elbow,
            &Left_Wrist, &Right_Wrist,
            &Left_Thigh, &Right_Thigh,
            &Left_Knee, &Right_Knee,
            &Left_Ankle, &Right_Ankle};
    }
};

struct 武器触发条件
{
    bool 独立调节 = false;
    bool 独立压枪 = false;
    bool 独立预判 = false;
    bool 独立距离限制 = false;
    bool 启用自瞄 = true; // 是否启用该武器自瞄
    float 腰射距离限制 = 50.0f;
    float 自瞄距离限制 = 100.0f; // 最大生效距离
    int 自瞄条件 = 0;            // 0=开火触发 1=开镜触发 2=任意条件
};

struct 武器参数
{
    float 压枪力度 = 2.15f;
    float 预判力度 = 1.45f;
    float 自瞄速度 = 20.f;
};

struct Vec2
{
    float x;
    float y;
    Vec2()
    {
        this->x = 0;
        this->y = 0;
    }
    Vec2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }
    Vec2 operator+(float v) const
    {
        return Vec2(x + v, y + v);
    }
    Vec2 operator-(float v) const
    {
        return Vec2(x - v, y - v);
    }
    Vec2 operator*(float v) const
    {
        return Vec2(x * v, y * v);
    }
    Vec2 operator/(float v) const
    {
        return Vec2(x / v, y / v);
    }
    Vec2 &operator+=(float v)
    {
        x += v;
        y += v;
        return *this;
    }
    Vec2 &operator-=(float v)
    {
        x -= v;
        y -= v;
        return *this;
    }
    Vec2 &operator*=(float v)
    {
        x *= v;
        y *= v;
        return *this;
    }
    Vec2 &operator/=(float v)
    {
        x /= v;
        y /= v;
        return *this;
    }
    Vec2 operator+(const Vec2 &v) const
    {
        return Vec2(x + v.x, y + v.y);
    }
    Vec2 operator-(const Vec2 &v) const
    {
        return Vec2(x - v.x, y - v.y);
    }
    Vec2 operator*(const Vec2 &v) const
    {
        return Vec2(x * v.x, y * v.y);
    }
    Vec2 operator/(const Vec2 &v) const
    {
        return Vec2(x / v.x, y / v.y);
    }
    Vec2 &operator+=(const Vec2 &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }
    Vec2 &operator-=(const Vec2 &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    Vec2 &operator*=(const Vec2 &v)
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }
    Vec2 &operator/=(const Vec2 &v)
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }
};

struct Vec3
{
    float x;
    float y;
    float z;

    Vec3()
    {
        x = y = z = 0.0f;
    }

    Vec3(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    Vec3 operator+(const Vec3 &v) const
    {
        return {x + v.x, y + v.y, z + v.z};
    }

    Vec3 operator-(const Vec3 &v) const
    {
        return {x - v.x, y - v.y, z - v.z};
    }

    bool operator==(const Vec3 &v)
    {
        return x == v.x && y == v.y && z == v.z;
    }

    bool operator!=(const Vec3 &v)
    {
        return !(x == v.x && y == v.y && z == v.z);
    }

    static Vec3 Zero()
    {
        return {0.0f, 0.0f, 0.0f};
    }

    static float Dot(Vec3 lhs, Vec3 rhs)
    {
        return (((lhs.x * rhs.x) + (lhs.y * rhs.y)) + (lhs.z * rhs.z));
    }

    static float Distance(Vec3 a, Vec3 b)
    {
        Vec3 vector = Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
        return sqrt(((vector.x * vector.x) + (vector.y * vector.y)) + (vector.z * vector.z));
    }
};

struct 自身数据
{
    FVector_class 坐标;
    FVector_class 相机坐标;
    FRotator_class 准星;
    D3DVector 视角;
    float 矩阵[16];
    float 水平后坐力;
    float 垂直后坐力;
    int 自身队伍;
    int 自身状态;
    int 射出子弹数量;
    int 真人数量;
    int 总人数;
    int 人机数量;
    int 队伍数量;
    int 开火;
    int 狙击开镜;
    int 射击类型;
    int 倍镜id;
    int 开镜;
    int 手持;
    int 手持id;
    int 手持握把;
    float Fov;
    float 子弹速度;
    uintptr_t 驱动自瞄地址;
    float 后坐力数据;
    float 准星Y;
    float 人物高度;
    D2DVector 自身角度;
    float 头;
    float 甲;
    Rotator NowRot;
    D3DVector CameraLocation;
    D3DVector Firearms;
    float 陀螺仪灵敏度第三人称;
    float 陀螺仪灵敏度第一人称;
    float 陀螺仪灵敏度红点;
    float 陀螺仪灵敏度二倍;
    float 陀螺仪灵敏度三倍;
    float 陀螺仪灵敏度四倍;
    float 陀螺仪灵敏度六倍;
    float 陀螺仪灵敏度八倍;
};

// 地址结构体
struct 对象地址
{
    long 敌人地址;
    long 物品地址[1000];
    long 车辆地址[50];
};

struct 敌人信息
{
    FVector_class 坐标;
    FVector_class 相机坐标;
    FRotator_class 准星;
    D3DVector 向量;
    FVector_class 骨骼坐标[17];
    D2DVector 雷达;
    D2DVector 对象骨骼;
    bool 烟雾中 = true;
    bool isboot;
    int 队伍;
    int 状态;
    int 距离;
    int 手持;
    float 当前血量;
    float 最大血量;
    bool 乘坐载具;
    string 名字;
    float Rotator;
    float 头;
    float 甲;
    uint64_t 实体地址;
    uint64_t 角色实体;
    uint64_t 实体列表地址;
    int 实体数量;
    bool isView = true;
    int 头甲包id;
    long 头甲包地址;
    int 子弹数量, 子弹最大数量;
    int 高级人机;
};
struct 物品信息
{
    D3DVector 坐标;
    int 物品;
    int 距离;
};

struct 车辆信息
{
    D3DVector 坐标;
    int 车辆;
    int 距离;
};

// 数据结构体
struct 对象信息
{
    int isCanRead;
    int 敌人数量;
    int 物品数量;
    int 车辆数量;
    敌人信息 敌人信息;
    物品信息 物品信息[1000];
    车辆信息 车辆信息[50];
};

struct 选择配置
{
    int 自瞄模式 = 999;
    int 无后台开关 = 999;
    int 防录屏 = 999;
    int 驱动选择 = 999;
};

struct Result
{
    float value1;
    float value2;
};

struct D3DXMATRIX
{
    float _11;
    float _12;
    float _13;
    float _14;
    float _21;
    float _22;
    float _23;
    float _24;
    float _31;
    float _32;
    float _33;
    float _34;
    float _41;
    float _42;
    float _43;
    float _44;
};

struct D3DXVECTOR4
{
    float X;
    float Y;
    float Z;
    float W;
};

struct FTransform1
{
    D3DXVECTOR4 Rotation;
    D3DVector Translation;
    D3DVector Scale3D;
};

#endif
