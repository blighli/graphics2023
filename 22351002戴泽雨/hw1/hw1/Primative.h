#pragma once
#include "Vector.h"
#include <vector>
namespace core::primative
{
	struct Triangle
	{
		const core::math::Vector3 a, b, c;
		const core::math::Vector4 color_a, color_b, color_c;
		Triangle(
			const core::math::Vector3& _a,
			const core::math::Vector3& _b,
			const core::math::Vector3& _c,
			const core::math::Vector4& _color_a,
			const core::math::Vector4& _color_b,
			const core::math::Vector4& _color_c
		);
		auto draw() -> void;
	
	private:
		std::vector<float> vertex_array;
		unsigned int VAO;
	};

}