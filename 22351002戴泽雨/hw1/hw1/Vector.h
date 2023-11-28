#pragma once
namespace core::math
{
	struct Vector2
	{
		Vector2(const float& a = 0.0f, const float& b = 0.0f) : x{ a }, y{ b } 
		{}
		Vector2(const float& a) : x{ a }, y{ a }
		{}
		Vector2(const Vector2& a) : x{ a.x }, y{ a.y }
		{}
		auto operator += (const Vector2& adder) -> void
		{
			x += adder.x;
			y += adder.y;
		}
		auto operator -= (const Vector2& minner) -> void
		{
			x += minner.x;
			y -= minner.y;
		}
		auto operator *= (const float& a) -> void
		{
			x *= a;
			y *= a;
		}
		float x, y;
	};

	struct Vector3
	{
		Vector3(const float& a = 0.0f, const float& b = 0.0f, const float& c = 0.0f)
			: x{ a }, y{ b }, z{ c }
		{}
		Vector3(const float& a)
			: x{ a }, y{ a }, z{ a }
		{}
		Vector3(const Vector3& a) : x{ a.x }, y{ a.y }, z{ a.z }
		{}
		auto operator += (const Vector3& adder) -> void
		{
			x += adder.x;
			y += adder.y;
			z += adder.z;
		}
		auto operator -= (const Vector3& minner) -> void
		{
			x += minner.x;
			y -= minner.y;
			z -= minner.z;
		}
		auto operator *= (const float& a) -> void
		{
			x *= a;
			y *= a;
			z *= a;
		}
		float x, y, z;
	};
	struct Vector4
	{
		Vector4(const float& a = 0.0f, const float& b = 0.0f, const float& c = 0.0f, const float& d = 0.0f)
			: x{ a }, y{ b }, z{ c }, w{ d }
		{}
		Vector4(const float& a)
			: x{ a }, y{ a }, z{ a }, w{ a }
		{}
		Vector4(const Vector4& a) : x{ a.x }, y{ a.y }, z{ a.z }, w{ a.w }
		{}
		auto operator += (const Vector4& adder) -> void
		{
			x += adder.x;
			y += adder.y;
			z += adder.z;
			w += adder.w;
		}
		auto operator -= (const Vector4& minner) -> void
		{
			x += minner.x;
			y -= minner.y;
			z -= minner.z;
			w -= minner.w;
		}
		auto operator *= (const float& a) -> void
		{
			x *= a;
			y *= a;
			z *= a;
			w *= a;
		}
		float x, y, z, w;
	};

	// OP
	auto operator + (const Vector2& a, const Vector2& b) -> Vector2;

	auto operator + (const Vector3& a, const Vector3& b) -> Vector3;

	auto operator + (const Vector4& a, const Vector4& b) -> Vector4;

	auto operator - (const Vector2& a, const Vector2& b) -> Vector2;

	auto operator - (const Vector3& a, const Vector3& b) -> Vector3;

	auto operator - (const Vector4& a, const Vector4& b) -> Vector4;

	auto operator * (const Vector2& a, float b) -> Vector2;

	auto operator * (const Vector3& a, float b) -> Vector3;

	auto operator * (const Vector4& a, float b) -> Vector4;

}