#pragma once
#include "ue4math.h"
#include "vector.h"

struct FQuat;
struct FMatrix_class;

// ScriptStruct CoreUObject.Rotator
// 0x000C
struct FRotator_class {
public:
	float                                              Pitch;                                                    // 0x0000(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Yaw;                                                      // 0x0004(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Roll;                                                     // 0x0008(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)

	FRotator_class() : Pitch(0.0), Yaw(0.0), Roll(0.0) {}
	FRotator_class(float pitch, float yaw, float roll) : Pitch(pitch), Yaw(yaw), Roll(roll) {}

	static float NormalizeAxis(float Angle) {
		//-180 ~ 180
		if (Angle > 180.0)
			Angle -= 360.0;
		if (Angle < -180.0)
			Angle += 360.0;
		return Angle;
	}

	void Clamp() {
		Pitch = std::clamp(NormalizeAxis(Pitch), -75.0f, 75.0f);
		Yaw = NormalizeAxis(Yaw);
		Roll = NormalizeAxis(Roll);
	}

	float InnerProduct(const FRotator_class& v) const {
		return (Pitch * v.Pitch) + (Yaw * v.Yaw) + (Roll * v.Roll);
	}

	FRotator_class OuterProduct(const FRotator_class& v) const {
		FRotator_class output;
		output.Pitch = (Yaw * v.Roll) - (Roll * v.Yaw);
		output.Yaw = (Roll * v.Pitch) - (Pitch * v.Roll);
		output.Roll = (Pitch * v.Yaw) - (Yaw * v.Pitch);
		return output;
	}

	bool operator == (const FRotator_class& v) const {
		return Pitch == v.Pitch && Yaw == v.Yaw && Roll == v.Roll;
	}

	bool operator != (const FRotator_class& v) const {
		return !(*this == v);
	}

	FRotator_class operator - () const {
		return FRotator_class(-Pitch, -Yaw, -Roll);
	}

	FRotator_class operator + (const FRotator_class& v) const {
		return FRotator_class(Pitch + v.Pitch, Yaw + v.Yaw, Roll + v.Roll);
	}

	FRotator_class operator - (const FRotator_class& v) const {
		return FRotator_class(Pitch - v.Pitch, Yaw - v.Yaw, Roll - v.Roll);
	}

	FRotator_class operator * (float Value) const {
		return FRotator_class(Pitch * Value, Yaw * Value, Roll * Value);
	}

	float Length() const {
		return sqrtf(Pitch * Pitch + Yaw * Yaw + Roll * Roll);
	}

	float Distance(const FRotator_class& v) const {
		return (v - *this).Length();
	}

	FRotator_class operator ^ (const FRotator_class& v) const {
		return OuterProduct(v);
	}

	float operator * (const FRotator_class& v) const {
		return InnerProduct(v);
	}

	FQuat GetQuaternion() const;
	FRotator_class(const FQuat& q);
	operator FQuat() const;

	FVector_class GetUnitVector() const;
	FMatrix_class GetMatrix(FVector_class origin = { 0, 0, 0 }) const;
};

static_assert(sizeof(FRotator_class) == 0xC, "FRotator_class");