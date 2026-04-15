#ifndef PHYSX_PUBLIC_H
#define PHYSX_PUBLIC_H
#include <cstdint>
#include <stdint.h>
#include <iostream>
#include <cmath>
#include <dlfcn.h>

namespace PhysxNS { // 命名空间
struct Rotator {
    float Pitch;
    float Yaw;
    float Roll;
    Rotator() : Pitch(0), Yaw(0), Roll(0) {}
    Rotator(float pitch, float yaw, float roll) : Pitch(pitch), Yaw(yaw), Roll(roll) {}

};

struct Vector2D {
    float X, Y;
    Vector2D() : X(0), Y(0) {}
    Vector2D(float x, float y) : X(x), Y(y) {}
};

struct Vector3D {
    float X, Y, Z;

    Vector3D() : X(0), Y(0), Z(0) {}
    Vector3D(float x, float y, float z) : X(x), Y(y), Z(z) {}
    bool operator!=(const Vector3D &Pos) const {
        return X != Pos.X || Y != Pos.Y || Z != Pos.Z;
    }
    Vector3D operator+(float v) const { return {X + v, Y + v, Z + v}; }
    Vector3D operator-(float v) const { return {X - v, Y - v, Z - v}; }
    Vector3D operator*(float v) const { return {X * v, Y * v, Z * v}; }
    Vector3D operator/(float v) const { return {X / v, Y / v, Z / v}; }
    Vector3D& operator=(float v) { X = Y = Z = v; return *this; }
    Vector3D& operator+=(float v) { X += v; Y += v; Z += v; return *this; }
    Vector3D& operator-=(float v) { X -= v; Y -= v; Z -= v; return *this; }
    Vector3D& operator*=(float v) { X *= v; Y *= v; Z *= v; return *this; }
    Vector3D& operator/=(float v) { X /= v; Y /= v; Z /= v; return *this; }
    Vector3D operator+(const Vector3D& v) const { return {X + v.X, Y + v.Y, Z + v.Z}; }
    Vector3D operator-(const Vector3D& v) const { return {X - v.X, Y - v.Y, Z - v.Z}; }
    Vector3D operator*(const Vector3D& v) const { return {X * v.X, Y * v.Y, Z * v.Z}; }
    Vector3D operator/(const Vector3D& v) const { return {X / v.X, Y / v.Y, Z / v.Z}; }
    Vector3D& operator+=(const Vector3D& v) { X += v.X; Y += v.Y; Z += v.Z; return *this; }
    Vector3D& operator-=(const Vector3D& v) { X -= v.X; Y -= v.Y; Z -= v.Z; return *this; }
    Vector3D& operator*=(const Vector3D& v) { X *= v.X; Y *= v.Y; Z *= v.Z; return *this; }
    Vector3D& operator/=(const Vector3D& v) { X /= v.X; Y /= v.Y; Z /= v.Z; return *this; }
    [[nodiscard]] float Length() const { return sqrt(X * X + Y * Y + Z * Z); }
    float LengthSquared() const { return X * X + Y * Y + Z * Z; }
    Vector3D Normalized() const {
        float len = Length();
        return len > 0.0001f ? Vector3D(X / len, Y / len, Z / len) : Vector3D(0, 0, 0);
    }
    float Dot(const Vector3D& other) const { return X * other.X + Y * other.Y + Z * other.Z; }
};
}

// 函数指针类型声明
typedef bool (*PhysxVerifyFunc)(const std::string&, const std::string&);
typedef void (*StartLoadModelFunc)(int, uint64_t);
typedef bool (*LineTraceSingleFunc)(const PhysxNS::Vector3D&, const PhysxNS::Vector3D&);
typedef std::vector<std::array<PhysxNS::Vector2D, 3>> (*RaycastTrianglesFunc)(const PhysxNS::Vector3D, const PhysxNS::Rotator, float, float, float);

// 全局函数指针和句柄的
extern void* g_physx_handle;
extern PhysxVerifyFunc PhysxVerify;
extern StartLoadModelFunc StartLoadModel;
extern LineTraceSingleFunc LineTraceSingle;
extern RaycastTrianglesFunc RaycastTriangles;

int loadDK();
void unloadDK();

#endif // PHYSX_PUBLIC_H
