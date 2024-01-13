#include "Vector.h"
namespace core::math
{
	// OP
	auto operator + (const Vector2& a, const Vector2& b) -> Vector2
	{
		return Vector2{ a.x + b.x, a.y + b.y };
	}

	auto operator + (const Vector3& a, const Vector3& b) -> Vector3
	{
		return Vector3{ a.x + b.x, a.y + b.y, a.z + b.z };
	}

	auto operator + (const Vector4& a, const Vector4& b) -> Vector4
	{
		return Vector4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
	}

	auto operator - (const Vector2& a, const Vector2& b) -> Vector2
	{
		return Vector2{ a.x - b.x, a.y - b.y };
	}

	auto operator - (const Vector3& a, const Vector3& b) -> Vector3
	{
		return Vector3{ a.x - b.x, a.y - b.y, a.z - b.z };
	}

	auto operator - (const Vector4& a, const Vector4& b) -> Vector4
	{
		return Vector4{ a.x - b.x, a.y - b.y, a.z - b.z , a.w - b.w };
	}

	auto operator * (const Vector2& a, float b) -> Vector2
	{
		return Vector2{ a.x * b, a.y * b };
	}

	auto operator * (const Vector3& a, float b) -> Vector3
	{
		return Vector3{ a.x * b, a.y * b, a.z * b };
	}

	auto operator * (const Vector4& a, float b) -> Vector4
	{
		return Vector4{ a.x * b, a.y * b, a.z * b, a.w * b };
	}
}