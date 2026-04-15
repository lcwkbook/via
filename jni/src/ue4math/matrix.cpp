#include "matrix.h"
#include "vector.h"
#include "rotator.h"
#include "transform.h"

FVector_class FMatrix_class::GetScaledAxisX() const { return FVector_class(M[0][0], M[0][1], M[0][2]); }
FVector_class FMatrix_class::GetScaledAxisY() const { return FVector_class(M[1][0], M[1][1], M[1][2]); }
FVector_class FMatrix_class::GetScaledAxisZ() const { return FVector_class(M[2][0], M[2][1], M[2][2]); }

FVector_class FMatrix_class::GetOrigin() const { return FVector_class(_41, _42, _43); }

FRotator_class FMatrix_class::GetRotator() const {
    const FVector_class XAxis = GetScaledAxisX();
    const FVector_class YAxis = GetScaledAxisY();
    const FVector_class ZAxis = GetScaledAxisZ();

    FRotator_class r = FRotator_class(
        atan2f(XAxis.Z, sqrtf(XAxis.X * XAxis.X + XAxis.Y * XAxis.Y)) * 180.0f / PI,
        atan2f(XAxis.Y, XAxis.X) * 180.0f / PI,
        0
    );

    const FVector_class SYAxis = GetScaledAxisY();

    r.Roll = atan2f(ZAxis | SYAxis, YAxis | SYAxis) * 180.0f / PI;

    return r;
}

//FMatrix_class& FMatrix_class::operator=(const FTransform_class& t) { return *this = FTransform_class(t).ToMatrixWithScale(); }
//FMatrix_class::FMatrix_class(const FTransform_class& t) { operator=(t); }

void FMatrix_class::SetAxis0(const FVector_class& Axis)
{
    M[0][0] = Axis.X;
    M[0][1] = Axis.Y;
    M[0][2] = Axis.Z;
}

void FMatrix_class::SetAxis1(const FVector_class& Axis)
{
    M[1][0] = Axis.X;
    M[1][1] = Axis.Y;
    M[1][2] = Axis.Z;
}

void FMatrix_class::SetAxis2(const FVector_class& Axis)
{
    M[2][0] = Axis.X;
    M[2][1] = Axis.Y;
    M[2][2] = Axis.Z;
}

FMatrix_class FMatrix_class::Inverse() const
{
    FMatrix_class Result;

    // Check for zero scale matrix to invert
    if (GetScaledAxisX().IsNearlyZero(SMALL_NUMBER) &&
        GetScaledAxisY().IsNearlyZero(SMALL_NUMBER) &&
        GetScaledAxisZ().IsNearlyZero(SMALL_NUMBER))
    {
        // just set to zero - avoids unsafe inverse of zero and duplicates what QNANs were resulting in before (scaling away all children)
        Result = FMatrix_class();
    }
    else
    {
        const float	Det = Determinant();

        if (Det == 0.0f)
        {
            Result = FMatrix_class();
        }
        else
        {
            VectorMatrixInverse(&Result, this);
        }
    }

    return Result;
}