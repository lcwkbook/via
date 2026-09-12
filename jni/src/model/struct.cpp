//
// Created by 王 on 2025/12/7.
//
#include "struct.h"


FVector FMatrix::GetScaledAxisX() { return {M[0][0], M[0][1], M[0][2]}; }
FVector FMatrix::GetScaledAxisY() { return {M[1][0], M[1][1], M[1][2]}; }
FVector FMatrix::GetScaledAxisZ() { return {M[2][0], M[2][1], M[2][2]}; }


FMatrix FRotator::GetMatrix(FVector origin) const {
    float radPitch = ConvertToRadians(Pitch);
    float radYaw = ConvertToRadians(Yaw);
    float radRoll = ConvertToRadians(Roll);

    float SP = sinf(radPitch);
    float CP = cosf(radPitch);
    float SY = sinf(radYaw);
    float CY = cosf(radYaw);
    float SR = sinf(radRoll);
    float CR = cosf(radRoll);

    FMatrix matriX;
    matriX.M[0][0] = CP * CY;
    matriX.M[0][1] = CP * SY;
    matriX.M[0][2] = SP;
    matriX.M[0][3] = 0.0;

    matriX.M[1][0] = SR * SP * CY - CR * SY;
    matriX.M[1][1] = SR * SP * SY + CR * CY;
    matriX.M[1][2] = -SR * CP;
    matriX.M[1][3] = 0.0;

    matriX.M[2][0] = -(CR * SP * CY + SR * SY);
    matriX.M[2][1] = CY * SR - CR * SP * SY;
    matriX.M[2][2] = CR * CP;
    matriX.M[2][3] = 0.0;

    matriX.M[3][0] = origin.X;
    matriX.M[3][1] = origin.Y;
    matriX.M[3][2] = origin.Z;
    matriX.M[3][3] = 1.0;

    return matriX;
}