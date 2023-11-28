#include "Primative.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
namespace core::primative
{
	Triangle::Triangle(
		const core::math::Vector3& _a,
		const core::math::Vector3& _b,
		const core::math::Vector3& _c,
		const core::math::Vector4& _color_a,
		const core::math::Vector4& _color_b,
		const core::math::Vector4& _color_c
	) :
		a{ _a },
		b{ _b },
		c{ _c },
		color_a{ _color_a },
		color_b{ _color_b },
		color_c{ _color_c },
		VAO{},
		vertex_array{}
	{
		vertex_array.push_back(a.x); vertex_array.push_back(a.y); vertex_array.push_back(a.z);
		vertex_array.push_back(color_a.x); vertex_array.push_back(color_a.y); vertex_array.push_back(color_a.z); vertex_array.push_back(color_a.w);
		vertex_array.push_back(b.x); vertex_array.push_back(b.y); vertex_array.push_back(b.z);
		vertex_array.push_back(color_b.x); vertex_array.push_back(color_b.y); vertex_array.push_back(color_b.z); vertex_array.push_back(color_b.w);
		vertex_array.push_back(c.x); vertex_array.push_back(c.y); vertex_array.push_back(c.z);
		vertex_array.push_back(color_c.x); vertex_array.push_back(color_c.y); vertex_array.push_back(color_c.z); vertex_array.push_back(color_c.w);
	
		auto VBO = unsigned int{};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_array.size(), vertex_array.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
	}
	auto Triangle::draw() -> void
	{
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}
}