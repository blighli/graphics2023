#pragma once
#include "Shader.h"
#include <glm/glm.hpp>
#include <string>

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
	struct Light_Manager
	{
		std::vector<Point_Light> point_lights;
		Direct_Light direct_light;
		auto setup_lights(core::shader::Shader& shader) const noexcept -> void
		{
			std::string pre = "point_lights";
			int nr = 0;
			for (auto& light : point_lights) {
				auto pre_s = pre + '[' + std::to_string(nr) + "].";
				auto str_position = pre_s + "position";
				auto str_color = pre_s + "color";
				shader.set_vec3(str_position, light.position);
				shader.set_vec3(str_color, light.color);
				nr++;
			}
		}
	};
}