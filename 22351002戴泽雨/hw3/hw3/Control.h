#pragma once
#pragma once
#include <glm/glm.hpp>
namespace core::control
{
	using glm::vec2;
	using glm::vec3;
	using glm::vec4;

	struct Ray
	{
		vec3 direction;
		vec3 origin;
		Ray(const vec3& in_origin, const vec3& in_direction):
			origin{in_origin},
			direction{in_direction}
		{}
	};

	struct Bounding_Box
	{
		vec3 min;
		vec3 max;
		Bounding_Box(const vec3& a, const vec3& b)
		{
			min.x = std::min(a.x, b.x);
			max.x = std::max(a.x, b.x);

			min.y = std::min(a.y, b.y);
			max.y = std::max(a.y, b.y);

			min.z = std::min(a.z, b.z);
			max.z = std::max(a.z, b.z);
		}
		auto intersect(const Ray& ray) -> bool;
	};

	auto test_intersect_with_triangle(const Ray& ray, const vec3& a, const vec3& b, const vec3& c) -> bool;
}