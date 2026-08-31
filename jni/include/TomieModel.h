#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>

// ========== 类型定义（如果你的项目中已有这些类型，请注释掉或删除以下部分） ==========
/*
struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    Vec3 operator+(const Vec3 &v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vec3 operator-(const Vec3 &v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vec3 operator*(float v) const { return {x * v, y * v, z * v}; }
    Vec3 operator/(float v) const { return {x / v, y / v, z / v}; }
    bool operator==(const Vec3 &v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vec3 &v) const { return !(*this == v); }
    static Vec3 Zero() { return {0.0f, 0.0f, 0.0f}; }
    static float Dot(const Vec3 &lhs, const Vec3 &rhs) { return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z; }
};

struct Rotator {
    float Pitch, Yaw, Roll;
    Rotator(float p=0, float y=0, float r=0) : Pitch(p), Yaw(y), Roll(r) {}
};

struct D3DVector {
    float X, Y, Z;
    D3DVector(float x=0, float y=0, float z=0) : X(x), Y(y), Z(z) {}
};
*/

// ========== 函数声明 ==========
// 如果使用上面注释的类型，请确保取消注释后编译
void InitPhysX(std::string license_key, uintptr_t libUE4,
               bool (*readv)(uint64_t, void*, size_t), int game);
bool LinePosition(const D3DVector& origin, const D3DVector& target);
std::vector<std::array<Vec3, 3>> PhysXMesh(const Vec3& cameraPos,
                                            const Rotator& cameraRot,
                                            float fov,
                                            int screenWidth,
                                            int screenHeight);
