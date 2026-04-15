#include "transform.h"
#include "vector.h"
#include "quat.h"
#include "matrix.h"

FTransform_class::FTransform_class(const FQuat& Rotation, const FVector_class& Translation, const FVector_class& Scale3D) :Rotation(Rotation), Translation(Translation), Scale3D(Scale3D) {}
FTransform_class::FTransform_class() : Rotation(FQuat(0.f, 0.f, 0.f, 1.f)), Translation(FVector_class(0.0, 0.0, 0.0)), Scale3D(FVector_class(1.0f, 1.0f, 1.0f)) {}

bool FTransform_class::AnyHasNegativeScale(const FVector_class& InScale3D, const FVector_class& InOtherScale3D)
{
	return  (InScale3D.X < 0.f || InScale3D.Y < 0.f || InScale3D.Z < 0.f
		|| InOtherScale3D.X < 0.f || InOtherScale3D.Y < 0.f || InOtherScale3D.Z < 0.f);
}

/**
* Convert this Transform to a transformation matrix with scaling.
*/
FMatrix_class FTransform_class::ToMatrixWithScale() const
{
	FMatrix_class OutMatrix;
	OutMatrix.M[3][0] = Translation.X;
	OutMatrix.M[3][1] = Translation.Y;
	OutMatrix.M[3][2] = Translation.Z;

	const float x2 = Rotation.X + Rotation.X;
	const float y2 = Rotation.Y + Rotation.Y;
	const float z2 = Rotation.Z + Rotation.Z;
	{
		const float xx2 = Rotation.X * x2;
		const float yy2 = Rotation.Y * y2;
		const float zz2 = Rotation.Z * z2;

		OutMatrix.M[0][0] = (1.0f - (yy2 + zz2)) * Scale3D.X;
		OutMatrix.M[1][1] = (1.0f - (xx2 + zz2)) * Scale3D.Y;
		OutMatrix.M[2][2] = (1.0f - (xx2 + yy2)) * Scale3D.Z;
	}
	{
		const float yz2 = Rotation.Y * z2;
		const float wx2 = Rotation.W * x2;

		OutMatrix.M[2][1] = (yz2 - wx2) * Scale3D.Z;
		OutMatrix.M[1][2] = (yz2 + wx2) * Scale3D.Y;
	}
	{
		const float xy2 = Rotation.X * y2;
		const float wz2 = Rotation.W * z2;

		OutMatrix.M[1][0] = (xy2 - wz2) * Scale3D.Y;
		OutMatrix.M[0][1] = (xy2 + wz2) * Scale3D.X;
	}
	{
		const float xz2 = Rotation.X * z2;
		const float wy2 = Rotation.W * y2;

		OutMatrix.M[2][0] = (xz2 + wy2) * Scale3D.Z;
		OutMatrix.M[0][2] = (xz2 - wy2) * Scale3D.X;
	}

	OutMatrix.M[0][3] = 0.0f;
	OutMatrix.M[1][3] = 0.0f;
	OutMatrix.M[2][3] = 0.0f;
	OutMatrix.M[3][3] = 1.0f;

	return OutMatrix;
}

void FTransform_class::MultiplyUsingMatrixWithScale(FTransform_class* OutTransform, const FTransform_class* A, const FTransform_class* B)
{
	// the goal of using M is to get the correct orientation
	// but for translation, we still need scale
	ConstructTransformFromMatrixWithDesiredScale(A->ToMatrixWithScale(), B->ToMatrixWithScale(), A->Scale3D * B->Scale3D, *OutTransform);
}

void FTransform_class::ConstructTransformFromMatrixWithDesiredScale(const FMatrix_class& AMatrix, const FMatrix_class& BMatrix, const FVector_class& DesiredScale, FTransform_class& OutTransform)
{
	// the goal of using M is to get the correct orientation
	// but for translation, we still need scale
	FMatrix_class M = AMatrix * BMatrix;
	M.RemoveScaling();

	// apply negative scale back to axes
	FVector_class SignedScale = DesiredScale.GetSignVector();

	M.SetAxis0(SignedScale.X * M.GetScaledAxisX());
	M.SetAxis1(SignedScale.Y * M.GetScaledAxisY());
	M.SetAxis2(SignedScale.Z * M.GetScaledAxisZ());

	// @note: if you have negative with 0 scale, this will return rotation that is identity
	// since matrix loses that axes
	FQuat Rotation = FQuat(M);
	Rotation.Normalize();

	// set values back to output
	OutTransform.Scale3D = DesiredScale;
	OutTransform.Rotation = Rotation;

	// technically I could calculate this using FTransform_class but then it does more quat multiplication 
	// instead of using Scale in matrix multiplication
	// it's a question of between RemoveScaling vs using FTransform_class to move translation
	OutTransform.Translation = M.GetOrigin();
}

/** Returns Multiplied Transform of 2 FTransforms **/
void FTransform_class::Multiply(FTransform_class* OutTransform, const FTransform_class* A, const FTransform_class* B)
{
	if (AnyHasNegativeScale(A->Scale3D, B->Scale3D))
	{
		// @note, if you have 0 scale with negative, you're going to lose rotation as it can't convert back to quat
		MultiplyUsingMatrixWithScale(OutTransform, A, B);
	}
	else
	{
		OutTransform->Rotation = B->Rotation * A->Rotation;
		OutTransform->Scale3D = A->Scale3D * B->Scale3D;
		OutTransform->Translation = B->Rotation * (B->Scale3D * A->Translation) + B->Translation;
	}

	// we do not support matrix transform when non-uniform
	// that was removed at rev 21 with UE4
}

// FTransform_class FTransform_class::operator*(const FTransform_class& A) {
// 	FTransform_class OutTransform;
// 	Multiply(&OutTransform, this, &A);
// 	return OutTransform;
// }

// mathematically if you have 0 scale, it should be infinite, 
// however, in practice if you have 0 scale, and relative transform doesn't make much sense 
// anymore because you should be instead of showing gigantic infinite mesh
// also returning BIG_NUMBER causes sequential NaN issues by multiplying 
// so we hardcode as 0
FVector_class FTransform_class::GetSafeScaleReciprocal(const FVector_class& InScale, float Tolerance)
{
	FVector_class SafeReciprocalScale;
	if (fabs(InScale.X) <= Tolerance)
	{
		SafeReciprocalScale.X = 0.f;
	}
	else
	{
		SafeReciprocalScale.X = 1 / InScale.X;
	}

	if (fabs(InScale.Y) <= Tolerance)
	{
		SafeReciprocalScale.Y = 0.f;
	}
	else
	{
		SafeReciprocalScale.Y = 1 / InScale.Y;
	}

	if (fabs(InScale.Z) <= Tolerance)
	{
		SafeReciprocalScale.Z = 0.f;
	}
	else
	{
		SafeReciprocalScale.Z = 1 / InScale.Z;
	}

	return SafeReciprocalScale;
}

void FTransform_class::GetRelativeTransformUsingMatrixWithScale(FTransform_class* OutTransform, const FTransform_class* Base, const FTransform_class* Relative)
{
	// the goal of using M is to get the correct orientation
	// but for translation, we still need scale
	FMatrix_class AM = Base->ToMatrixWithScale();
	FMatrix_class BM = Relative->ToMatrixWithScale();
	// get combined scale
	FVector_class SafeRecipScale3D = GetSafeScaleReciprocal(Relative->Scale3D, SMALL_NUMBER);
	FVector_class DesiredScale3D = Base->Scale3D * SafeRecipScale3D;
	ConstructTransformFromMatrixWithDesiredScale(AM, BM.Inverse(), DesiredScale3D, *OutTransform);
}

FTransform_class FTransform_class::GetRelativeTransform(const FTransform_class& Other) const
{
	// A * B(-1) = VQS(B)(-1) (VQS (A))
	// 
	// Scale = S(A)/S(B)
	// Rotation = Q(B)(-1) * Q(A)
	// Translation = 1/S(B) *[Q(B)(-1)*(T(A)-T(B))*Q(B)]
	// where A = this, B = Other
	FTransform_class Result;

	if (AnyHasNegativeScale(Scale3D, Other.Scale3D))
	{
		// @note, if you have 0 scale with negative, you're going to lose rotation as it can't convert back to quat
		GetRelativeTransformUsingMatrixWithScale(&Result, this, &Other);
	}
	else
	{
		FVector_class SafeRecipScale3D = GetSafeScaleReciprocal(Other.Scale3D, SMALL_NUMBER);
		Result.Scale3D = Scale3D * SafeRecipScale3D;

		if (Other.Rotation.IsNormalized() == false)
		{
			return FTransform_class();
		}

		FQuat Inverse = Other.Rotation.Inverse();
		Result.Rotation = Inverse * Rotation;

		Result.Translation = (Inverse * (Translation - Other.Translation)) * (SafeRecipScale3D);
	}

	return Result;
}