#pragma once
#include "ue4math.h"
#include "vector.h"
#include "quat.h"
//本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险
struct FMatrix_class;

// ScriptStruct CoreUObject.Transform
// 0x0030
struct alignas(16) FTransform_class {
public:
	struct FQuat                                       Rotation;                                                 // 0x0000(0x0010) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
	struct FVector_class                                     Translation;                                              // 0x0010(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
private:	unsigned char                              UnknownData00[0x4];                                       // 0x001C(0x0004) MISSED OFFSET
public:		struct FVector_class                             Scale3D;                                                  // 0x0020(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)

	FTransform_class();
	FTransform_class(const FQuat& Rotation, const FVector_class& Translation, const FVector_class& Scale3D);
	static bool AnyHasNegativeScale(const FVector_class& InScale3D, const FVector_class& InOtherScale3D);
	static void Multiply(FTransform_class* OutTransform, const FTransform_class* A, const FTransform_class* B);

	static void MultiplyUsingMatrixWithScale(FTransform_class* OutTransform, const FTransform_class* A, const FTransform_class* B);
	static void ConstructTransformFromMatrixWithDesiredScale(const FMatrix_class& AMatrix, const FMatrix_class& BMatrix, const FVector_class& DesiredScale, FTransform_class& OutTransform);

	FMatrix_class ToMatrixWithScale() const;

	FTransform_class operator*(const FTransform_class& A);

	static FVector_class GetSafeScaleReciprocal(const FVector_class& InScale, float Tolerance = SMALL_NUMBER);
	FTransform_class GetRelativeTransform(const FTransform_class& Other) const;

	static void GetRelativeTransformUsingMatrixWithScale(FTransform_class* OutTransform, const FTransform_class* Base, const FTransform_class* Relative);
};

static_assert(sizeof(FTransform_class) == 0x30, "FTransform_class");
static_assert(offsetof(FTransform_class, Scale3D) == 0x20, "FTransform_class");