#pragma once
//============================================================================
//  PhysX Pro 模型库 —— 对接单头文件（给客户的唯一头文件）
//  只需本头 + libPhysXPro.a 即可对接，无需任何内部头/源码。
//
//  三个函数：
//    InitPhysX  —— 初始化（第一个参数就是卡密；并传入 libUE4 基址 + 读内存回调）
//    LinePosition —— 射线遮挡判定（true=有遮挡/不可见）
//    PhysXMesh  —— 取准星命中模型的三角形（世界坐标，自行投影绘制）
//
//  注意：若你的工程已定义 Vec3 / Rotator / D3DVector，请在包含本头之前
//        #define VEC3_DEFINED / ROTATOR_DEFINED / D3DVECTOR_DEFINED 以避免重复定义。
//============================================================================
#include <cstdint>
#include <string>
#include <vector>
#include <array>

#ifndef VEC3_DEFINED
#define VEC3_DEFINED
struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};
#endif

#ifndef ROTATOR_DEFINED
#define ROTATOR_DEFINED
struct Rotator {
    float Pitch, Yaw, Roll;
    Rotator() : Pitch(0), Yaw(0), Roll(0) {}
    Rotator(float p, float y, float r) : Pitch(p), Yaw(y), Roll(r) {}
};
#endif

#ifndef D3DVECTOR_DEFINED
#define D3DVECTOR_DEFINED
struct D3DVector {
    float X, Y, Z;
    D3DVector() : X(0), Y(0), Z(0) {}
    D3DVector(float x_, float y_, float z_) : X(x_), Y(y_), Z(z_) {}
};
#endif

//==================== 函数声明 ====================

// 初始化模型库。
//   license_key : 卡密（直接填你的卡密字符串；传空或 "y" 用上次保存的 /storage/emulated/0/YU模型2）
//   libUE4      : 游戏 libUE4.so 基址
//   readv       : 你的内存读取回调，签名必须为 bool(uint64_t addr, void* buf, size_t size)
//   game        : 游戏选择（0=和平精英，按需）
// 卡密验证不过 → 模型不工作（不影响你程序其它功能）；成功/失败原因自动打日志。
void InitPhysX(std::string license_key, uintptr_t libUE4,
               bool (*readv)(uint64_t, void*, size_t), int game);

// 射线遮挡判定：origin 到 target 是否被挡。
//   返回 true  = 有遮挡（不可见）
//   返回 false = 无遮挡（可见）
// 用法：可见性 = !LinePosition(相机坐标, 骨骼坐标);
bool LinePosition(const D3DVector& origin, const D3DVector& target);

// 取准星（相机正前方）命中的那一个模型的三角形（世界坐标），自行投影绘制。
//   cameraPos  : 相机世界坐标
//   cameraRot  : 相机朝向（度）—— 用准星 Pitch/Yaw/Roll
//   fov        : 视野角
//   screenW/H  : 屏幕宽高（保留参数，可传真实分辨率）
std::vector<std::array<Vec3, 3>> PhysXMesh(const Vec3& cameraPos,
                                           const Rotator& cameraRot,
                                           float fov,
                                           int screenWidth,
                                           int screenHeight);
