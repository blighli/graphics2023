#pragma once
#include <glm/glm.hpp>
namespace core::light
{
	using glm::vec2;
	using glm::vec3;
	using glm::vec4;
	struct Point_Light
	{
		//Vector3 direction;
		vec3 color;
		vec3 position;
		Point_Light(const vec3& in_color, const vec3& in_position):
			color{in_color},
			position{in_position}
		{}
		float intensity;
	};
	struct Direct_Light
	{
		vec3 direction;
		vec3 color;
		float intensity;
	};
}