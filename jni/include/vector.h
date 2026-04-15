#pragma once
#include "ue4math.h"

struct FRotator_class;

//本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险


// ScriptStruct CoreUObject.Vector
// 0x000C
struct FVector_class
{
public:
	float                                              X;                                                        // 0x0000(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Z;                                                        // 0x0008(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)

	FVector_class() : X(0.0), Y(0.0), Z(0.0) {}
	FVector_class(float X, float Y, float Z) :X(X), Y(Y), Z(Z) {}

	float DotProduct(const FVector_class& v) const {
		return (X * v.X) + (Y * v.Y) + (Z * v.Z);
	}

	FVector_class CrossProduct(const FVector_class& v) const {
		FVector_class output;
		output.X = (Y * v.Z) - (Z * v.Y);
		output.Y = (Z * v.X) - (X * v.Z);
		output.Z = (X * v.Y) - (Y * v.X);
		return output;
	}

	FVector_class Min(const FVector_class& v) const {
		FVector_class output;
		output.X = X < v.X ? X : v.X;
		output.Y = Y < v.Y ? Y : v.Y;
		output.Z = Z < v.Z ? Z : v.Z;
		return output;
	}

	FVector_class Max(const FVector_class& v) const {
		FVector_class output;
		output.X = X > v.X ? X : v.X;
		output.Y = Y > v.Y ? Y : v.Y;
		output.Z = Z > v.Z ? Z : v.Z;
		return output;
	}

	bool operator == (const FVector_class& v) const {
		return X == v.X && Y == v.Y && Z == v.Z;
	}

	bool operator != (const FVector_class& v) const {
		return !(*this == v);
	}
	
	FVector_class operator - () const {
		return FVector_class(-X, -Y, -Z);
	}

	FVector_class operator + (const FVector_class& v) const {
		return FVector_class(X + v.X, Y + v.Y, Z + v.Z);
	}

	FVector_class operator - (const FVector_class& v) const {
		return FVector_class(X - v.X, Y - v.Y, Z - v.Z);
	}

	FVector_class operator * (const FVector_class& v) const {
		return FVector_class(X * v.X, Y * v.Y, Z * v.Z);
	}

	FVector_class operator * (float Value) const {
		return FVector_class(X * Value, Y * Value, Z * Value);
	}

	FVector_class GetNormalizedVector() const {
		return operator*(1.0f / sqrtf(X * X + Y * Y + Z * Z));
	}

	void Normalize() {
		*this = GetNormalizedVector();
	}

	float Length() const {
		return sqrtf(X * X + Y * Y + Z * Z);
	}

	float Distance(const FVector_class& v) const {
		return (v - *this).Length();
	}

	FVector_class operator ^ (const FVector_class& v) const {
		return CrossProduct(v);
	}

	float operator | (const FVector_class& v) const {
		return DotProduct(v);
	}

	FVector_class GetSignVector() const
	{
		return FVector_class
		(
			Select(X, 1.0, -1.0),
			Select(Y, 1.0, -1.0),
			Select(Z, 1.0, -1.0)
		);
	}

	bool IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const {
		return fabs(X) <= Tolerance && fabs(Y) <= Tolerance && fabs(Z) <= Tolerance;
	}

	FRotator_class GetDirectionRotator() const;
};

static FVector_class operator * (float Value, const FVector_class& v) {
	return v.operator*(Value);
}

static_assert(sizeof(FVector_class) == 0xC, "FVector_class");
class FVector2D
{
public:

	float                                              X;                                                         // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                              Y;                                                         // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)

	inline FVector2D() : X(0), Y(0) {}

	inline FVector2D(float x, float y) : X(x), Y(y) {}
	inline bool Zero() const
	{
		return (X > -0.1f && X < 0.1f && Y > -0.1f && Y < 0.1f);
	}
	inline FVector2D operator + (const FVector2D& other) const { return FVector2D(X + other.X, Y + other.Y); }

	inline FVector2D operator - (const FVector2D& other) const { return FVector2D(X - other.X, Y - other.Y); }

	inline FVector2D operator * (float scalar) const { return FVector2D(X * scalar, Y * scalar); }

	inline FVector2D operator * (const FVector2D& other) const { return FVector2D(X * other.X, Y * other.Y); }

	inline FVector2D operator / (float scalar) const { return FVector2D(X / scalar, Y / scalar); }

	inline FVector2D operator / (const FVector2D& other) const { return FVector2D(X / other.X, Y / other.Y); }

	inline FVector2D& operator=  (const FVector2D& other) { X = other.X; Y = other.Y; return *this; }

	inline FVector2D& operator+= (const FVector2D& other) { X += other.X; Y += other.Y; return *this; }

	inline FVector2D& operator-= (const FVector2D& other) { X -= other.X; Y -= other.Y; return *this; }

	inline FVector2D& operator*= (const float other) { X *= other; Y *= other; return *this; }

	bool IsValid_S() const {
		return -65535.f < X && X < 65535.f && -65535.f < Y && Y < 65535.f;

	}
	inline float Distance(const FVector2D& other) const {
		float a = (X - other.X);
		float b = (Y - other.Y);
		return sqrt(a * a + b * b);

	}
};