#pragma once
#include "Texture.h"
#include "Control.h"
#include <glm/glm.hpp>
#include <vector>

namespace core::primative
{
	using glm::vec2;
	using glm::vec3;
	using glm::vec4;
	using core::control::Ray;
	using core::control::Bounding_Box;

	struct Vertex
	{
		vec3 position;
		vec4 color;
		vec2 uv;
		vec3 normal;
	};

	struct Triangle
	{
		glm::vec3 a, b, c;
		glm::vec4 color_a, color_b, color_c;
		glm::vec2 uv_a, uv_b, uv_c;
		glm::vec3 normal_a, normal_b, normal_c;
		const bool as_buffer;
		Triangle(
			const glm::vec3& _a,
			const glm::vec3& _b,
			const glm::vec3& _c,
			const glm::vec4& _color_a,
			const glm::vec4& _color_b,
			const glm::vec4& _color_c,
			const glm::vec2& _uv_a,
			const glm::vec2& _uv_b,
			const glm::vec2& _uv_c,
			const glm::vec3& _normal_a,
			const glm::vec3& _normal_b,
			const glm::vec3& _normal_c,
			bool _as_buffer = false
		);
		auto set_tex(const std::string& in_tex_name) noexcept -> void;
		auto draw() -> void;
	
	private:
		std::string tex_name{"_None"};
		std::vector<float> vertex_array;
		unsigned int VAO;
	};

	struct Triangle_Mesh
	{
		std::vector<Vertex> mesh_vertex;
		std::vector<int> vertex_index;
		auto intersect(const Ray& ray) -> bool;
	};

	struct Sphere
	{
		std::string name;
		vec3 sheld_color;
		enum struct Tex_Match_Method
		{
			text_2d_default
		};
		float radius;
		vec3 origin;
		int tessellation_x;
		int tessellation_y;
		Sphere(const vec3& in_origin, float in_radius, int in_tessellation_x, int in_tessellation_y, vec4 in_color = vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		auto set_tex(const std::string& in_tex_name) noexcept -> void;
		auto draw() const noexcept -> void;
		auto intersect_mesh(const Ray& ray) -> bool;
		auto intersect_sphere(const Ray& ray) -> bool;

	private:
		Bounding_Box bounding_box;
		Triangle_Mesh triangle_mesh;
		std::string tex_name{"_None"};
		unsigned int VAO;
	};

}