#pragma once
#include "ue4math.h"

struct FVector_class;
struct FRotator_class;
struct FTransform;

struct FMatrix_class {
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

    FMatrix_class() {
        //Identity matrix
        _11 = 1.0f; _12 = 0.0f; _13 = 0.0f; _14 = 0.0f;
        _21 = 0.0f; _22 = 1.0f; _23 = 0.0f; _24 = 0.0f;
        _31 = 0.0f; _32 = 0.0f; _33 = 1.0f; _34 = 0.0f;
        _41 = 0.0f; _42 = 0.0f; _43 = 0.0f; _44 = 1.0f;
    }

    FMatrix_class MatrixMultiply(const FMatrix_class& M2) const {
        const FMatrix_class& M1 = *this;
        FMatrix_class mResult;
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

    FMatrix_class operator * (const FMatrix_class& v) const { return MatrixMultiply(v); }

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

    float Determinant() const
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

    FMatrix_class Inverse() const;

    void SetAxis0(const FVector_class& Axis);
    void SetAxis1(const FVector_class& Axis);
    void SetAxis2(const FVector_class& Axis);

    FVector_class GetOrigin() const;
    FVector_class GetScaledAxisX() const;
    FVector_class GetScaledAxisY() const;
    FVector_class GetScaledAxisZ() const;
    FRotator_class GetRotator() const;

    //Convert to FTransform
    FMatrix_class& operator=(const FTransform& t);
    FMatrix_class(const FTransform& t);
};