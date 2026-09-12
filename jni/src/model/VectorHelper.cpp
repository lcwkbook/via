#define NOMINMAX
#include "VectorHelper.h"
#include "Log.h"
#include "掩体配置.h"

bool VectorHelper::IsInScreen(const FVector2D WorldToScreen) {
    if ((WorldToScreen.X < -100 || WorldToScreen.X > (float)掩体配置::屏幕宽 + 100 || WorldToScreen.Y < -100 || WorldToScreen.Y > (float)掩体配置::屏幕高 + 100))
    {
        return false;
    }
    return true;
}

FVector2D VectorHelper::WorldToScreen(const FVector& WorldLocation, MinimalViewInfo POV) {
    FVector2D ScreenLocation;

    FMatrix RotationMatrix = POV.Rotation.GetMatrix();

    FVector AxisX = RotationMatrix.GetScaledAxisX();
    FVector AxisY = RotationMatrix.GetScaledAxisY();
    FVector AxisZ = RotationMatrix.GetScaledAxisZ();

    FVector vDelta(WorldLocation - POV.Location);
    FVector vTransformed(vDelta | AxisY, vDelta | AxisZ, vDelta | AxisX);

    if (vTransformed.Z == 0.0f)
        vTransformed.Z = -0.001f;

    auto VieW = vTransformed.Z;

    if (vTransformed.Z < 0.0f)
        vTransformed.Z = -vTransformed.Z;

    float ScreenCenterX = (float)掩体配置::屏幕宽 / 2.0f;
    float ScreenCenterY = (float)掩体配置::屏幕高 / 2.0f;
    float TangentFOV = tanf(ConvertToRadians(POV.FOV / 2.0f));

    ScreenLocation.X = (ScreenCenterX + vTransformed.X * (ScreenCenterX / TangentFOV) / vTransformed.Z);
    ScreenLocation.Y = (ScreenCenterY - vTransformed.Y * (ScreenCenterX / TangentFOV) / vTransformed.Z);

    if (VieW != INFINITY && VieW > 0.0f)
        return ScreenLocation;
    return {INFINITY, INFINITY};
}
