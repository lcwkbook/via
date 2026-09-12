#pragma once
#include "struct.h"

#include <cmath>

class VectorHelper
{
public:


	static FVector2D WorldToScreen(const FVector& WorldLocation, MinimalViewInfo POV);
    static bool IsInScreen(FVector2D WorldToScreen);
};