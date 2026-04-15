#include "vector.h"
#include "rotator.h"

FRotator_class FVector_class::GetDirectionRotator() const {
	FRotator_class r;
	r.Pitch = ConvertToDegrees(atan2f(Z, sqrtf(X * X + Y * Y)));
	r.Yaw = ConvertToDegrees(atan2f(Y, X));
	r.Roll = 0.0;
	return r;
}