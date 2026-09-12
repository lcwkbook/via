//
// Created by Wang on 2025/7/3.
//
#ifndef STRUCT_H_
#define STRUCT_H_
#include <string>
#include <cstring>
#include "VectorStruct.h"

#define SMALL_NUMBER		(1.e-8f)
#define PI 3.141592653589793238


static float ConvertToRadians(float Degrees) { return Degrees * (PI / 180.0f); }
static float ConvertToDegrees(float Radians) { return Radians * (180.0f / PI); }

static float InvSqrt(float F)
{
    return 1.0f / sqrtf(F);
}

static float Select(float Comparand, float ValueGEZero, float ValueLTZero)
{
    return Comparand >= 0.0 ? ValueGEZero : ValueLTZero;
}
struct FVector
{
public:
    float                                              X;                                                        // 0x0000(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
    float                                              Y;                                                        // 0x0004(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
    float                                              Z;                                                        // 0x0008(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)

    FVector() : X(0.0), Y(0.0), Z(0.0) {}
    FVector(float X, float Y, float Z) :X(X), Y(Y), Z(Z) {}

    [[nodiscard]] float DotProduct(const FVector& v) const {
        return (X * v.X) + (Y * v.Y) + (Z * v.Z);
    }

    [[nodiscard]] FVector CrossProduct(const FVector& v) const {
        FVector output;
        output.X = (Y * v.Z) - (Z * v.Y);
        output.Y = (Z * v.X) - (X * v.Z);
        output.Z = (X * v.Y) - (Y * v.X);
        return output;
    }

    [[nodiscard]] FVector Min(const FVector& v) const {
        FVector output;
        output.X = X < v.X ? X : v.X;
        output.Y = Y < v.Y ? Y : v.Y;
        output.Z = Z < v.Z ? Z : v.Z;
        return output;
    }

    [[nodiscard]] FVector Max(const FVector& v) const {
        FVector output;
        output.X = X > v.X ? X : v.X;
        output.Y = Y > v.Y ? Y : v.Y;
        output.Z = Z > v.Z ? Z : v.Z;
        return output;
    }

    bool operator == (const FVector& v) const {
        return X == v.X && Y == v.Y && Z == v.Z;
    }

    bool operator != (const FVector& v) const {
        return !(*this == v);
    }

    FVector& operator*=(float s) {
        X *= s;
        Y *= s;
        Z *= s;
        return *this;
    }

    FVector operator - () const {
        return {-X, -Y, -Z};
    }

    FVector operator + (const FVector& v) const {
        return {X + v.X, Y + v.Y, Z + v.Z};
    }

    FVector operator - (const FVector& v) const {
        return {X - v.X, Y - v.Y, Z - v.Z};
    }

    FVector operator * (const FVector& v) const {
        return {X * v.X, Y * v.Y, Z * v.Z};
    }

    FVector operator * (float Value) const {
        return {X * Value, Y * Value, Z * Value};
    }

    [[nodiscard]] FVector GetNormalizedVector() const {
        return operator*(1.0f / sqrtf(X * X + Y * Y + Z * Z));
    }

    void Normalize() {
        *this = GetNormalizedVector();
    }

    [[nodiscard]] float Length() const {
        return sqrtf(X * X + Y * Y + Z * Z);
    }

    [[nodiscard]] float Distance(const FVector& v) const {
        return (v - *this).Length();
    }

    FVector operator ^ (const FVector& v) const {
        return CrossProduct(v);
    }

    float operator | (const FVector& v) const {
        return DotProduct(v);
    }

    [[nodiscard]] FVector GetSignVector() const
    {
        return {
                        Select(X, 1.0, -1.0),
                        Select(Y, 1.0, -1.0),
                        Select(Z, 1.0, -1.0)
                };
    }
};

static FVector operator * (float Value, const FVector& v) {
    return v.operator*(Value);
}

struct Vector3;

struct FVector2D {
    float X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
    float Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

    inline FVector2D()
            : X(0), Y(0) {}

    inline FVector2D(float x, float y)
            : X(x),
              Y(y) {}

    static float Distance(FVector2D a, FVector2D PE_Result) {
        return sqrt(pow(PE_Result.X - a.X, 2) + pow(PE_Result.Y - a.Y, 2));
    }

};







struct FMatrix {
public:
    union
    {
        struct
        {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float M[4][4];
    };

    [[nodiscard]] FMatrix() {
        //Identity matrix
        _11 = 1.0f; _12 = 0.0f; _13 = 0.0f; _14 = 0.0f;
        _21 = 0.0f; _22 = 1.0f; _23 = 0.0f; _24 = 0.0f;
        _31 = 0.0f; _32 = 0.0f; _33 = 1.0f; _34 = 0.0f;
        _41 = 0.0f; _42 = 0.0f; _43 = 0.0f; _44 = 1.0f;
    }

    [[nodiscard]] FMatrix MatrixMultiply(const FMatrix& M2) const {
        const FMatrix& M1 = *this;
        FMatrix mResult;
        float x = M1.M[0][0];
        float y = M1.M[0][1];
        float z = M1.M[0][2];
        float w = M1.M[0][3];
        mResult.M[0][0] = (M2.M[0][0] * x) + (M2.M[1][0] * y) + (M2.M[2][0] * z) + (M2.M[3][0] * w);
        mResult.M[0][1] = (M2.M[0][1] * x) + (M2.M[1][1] * y) + (M2.M[2][1] * z) + (M2.M[3][1] * w);
        mResult.M[0][2] = (M2.M[0][2] * x) + (M2.M[1][2] * y) + (M2.M[2][2] * z) + (M2.M[3][2] * w);
        mResult.M[0][3] = (M2.M[0][3] * x) + (M2.M[1][3] * y) + (M2.M[2][3] * z) + (M2.M[3][3] * w);
        x = M1.M[1][0];
        y = M1.M[1][1];
        z = M1.M[1][2];
        w = M1.M[1][3];
        mResult.M[1][0] = (M2.M[0][0] * x) + (M2.M[1][0] * y) + (M2.M[2][0] * z) + (M2.M[3][0] * w);
        mResult.M[1][1] = (M2.M[0][1] * x) + (M2.M[1][1] * y) + (M2.M[2][1] * z) + (M2.M[3][1] * w);
        mResult.M[1][2] = (M2.M[0][2] * x) + (M2.M[1][2] * y) + (M2.M[2][2] * z) + (M2.M[3][2] * w);
        mResult.M[1][3] = (M2.M[0][3] * x) + (M2.M[1][3] * y) + (M2.M[2][3] * z) + (M2.M[3][3] * w);
        x = M1.M[2][0];
        y = M1.M[2][1];
        z = M1.M[2][2];
        w = M1.M[2][3];
        mResult.M[2][0] = (M2.M[0][0] * x) + (M2.M[1][0] * y) + (M2.M[2][0] * z) + (M2.M[3][0] * w);
        mResult.M[2][1] = (M2.M[0][1] * x) + (M2.M[1][1] * y) + (M2.M[2][1] * z) + (M2.M[3][1] * w);
        mResult.M[2][2] = (M2.M[0][2] * x) + (M2.M[1][2] * y) + (M2.M[2][2] * z) + (M2.M[3][2] * w);
        mResult.M[2][3] = (M2.M[0][3] * x) + (M2.M[1][3] * y) + (M2.M[2][3] * z) + (M2.M[3][3] * w);
        x = M1.M[3][0];
        y = M1.M[3][1];
        z = M1.M[3][2];
        w = M1.M[3][3];
        mResult.M[3][0] = (M2.M[0][0] * x) + (M2.M[1][0] * y) + (M2.M[2][0] * z) + (M2.M[3][0] * w);
        mResult.M[3][1] = (M2.M[0][1] * x) + (M2.M[1][1] * y) + (M2.M[2][1] * z) + (M2.M[3][1] * w);
        mResult.M[3][2] = (M2.M[0][2] * x) + (M2.M[1][2] * y) + (M2.M[2][2] * z) + (M2.M[3][2] * w);
        mResult.M[3][3] = (M2.M[0][3] * x) + (M2.M[1][3] * y) + (M2.M[2][3] * z) + (M2.M[3][3] * w);
        return mResult;
    }

    FMatrix operator * (const FMatrix& v) const { return MatrixMultiply(v); }

    void RemoveScaling(float Tolerance = SMALL_NUMBER)
    {
        // For each row, find magnitude, and if its non-zero re-scale so its unit length.
        const float SquareSum0 = (M[0][0] * M[0][0]) + (M[0][1] * M[0][1]) + (M[0][2] * M[0][2]);
        const float SquareSum1 = (M[1][0] * M[1][0]) + (M[1][1] * M[1][1]) + (M[1][2] * M[1][2]);
        const float SquareSum2 = (M[2][0] * M[2][0]) + (M[2][1] * M[2][1]) + (M[2][2] * M[2][2]);
        const float Scale0 = Select(SquareSum0 - Tolerance, InvSqrt(SquareSum0), 1.0);
        const float Scale1 = Select(SquareSum1 - Tolerance, InvSqrt(SquareSum1), 1.0);
        const float Scale2 = Select(SquareSum2 - Tolerance, InvSqrt(SquareSum2), 1.0);
        M[0][0] *= Scale0;
        M[0][1] *= Scale0;
        M[0][2] *= Scale0;
        M[1][0] *= Scale1;
        M[1][1] *= Scale1;
        M[1][2] *= Scale1;
        M[2][0] *= Scale2;
        M[2][1] *= Scale2;
        M[2][2] *= Scale2;
    }

    [[nodiscard]] float Determinant() const
    {
        return
                M[0][0] * (
                        M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
                        M[2][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) +
                        M[3][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])
                ) -
                M[1][0] * (
                        M[0][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
                        M[2][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
                        M[3][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2])
                ) +
                M[2][0] * (
                        M[0][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) -
                        M[1][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
                        M[3][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
                ) -
                M[3][0] * (
                        M[0][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) -
                        M[1][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2]) +
                        M[2][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
                );
    }

    FVector GetScaledAxisX();
    FVector GetScaledAxisY();
    FVector GetScaledAxisZ();


};




struct FRotator {
    float Pitch;                                                    // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
    float Yaw;                                                      // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
    float Roll;                                                     // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

    FRotator() : Pitch(0.0), Yaw(0.0), Roll(0.0) {}
    FRotator(float pitch, float yaw, float roll) : Pitch(pitch), Yaw(yaw), Roll(roll) {}

    [[nodiscard]] FMatrix GetMatrix(FVector origin = { 0, 0, 0 }) const;
};




struct MinimalViewInfo {
    FVector Location;//[Offset: 0x0, Size: 0xc]
    FVector LocationLocalSpace;//[Offset: 0xc, Size: 0xc]
    FRotator Rotation;//[Offset: 0x18, Size: 0xc]
    char mPad[0xC]{};
    float FOV{};
};


struct FGameData {
    uint64_t UWorld;
    uint64_t UE4;
    uint64_t PhysxInstancePtr;
    FVector WorldOriginLocation;
    FVector CameraLocation;
};



extern FGameData GameData;


#endif // STRUCT_H_