#include "Primative.h"
#include <numbers>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
namespace core::primative
{
	Triangle::Triangle(
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
		bool _as_buffer
	) :
		a{ _a },
		b{ _b },
		c{ _c },
		color_a{ _color_a },
		color_b{ _color_b },
		color_c{ _color_c },
		uv_a{ _uv_a },
		uv_b{ _uv_b },
		uv_c{ _uv_c },
		normal_a{ _normal_a },
		normal_b{ _normal_b },
		normal_c{ _normal_c },
		as_buffer{ _as_buffer },
		VAO{},
		vertex_array{}
	{
		vertex_array.push_back(a.x); vertex_array.push_back(a.y); vertex_array.push_back(a.z);
		vertex_array.push_back(color_a.x); vertex_array.push_back(color_a.y); vertex_array.push_back(color_a.z); vertex_array.push_back(color_a.w);
		vertex_array.push_back(uv_a.x); vertex_array.push_back(uv_a.y);
		vertex_array.push_back(normal_a.x); vertex_array.push_back(normal_a.y); vertex_array.push_back(normal_a.z);
		
		vertex_array.push_back(b.x); vertex_array.push_back(b.y); vertex_array.push_back(b.z);
		vertex_array.push_back(color_b.x); vertex_array.push_back(color_b.y); vertex_array.push_back(color_b.z); vertex_array.push_back(color_b.w);
		vertex_array.push_back(uv_b.x); vertex_array.push_back(uv_b.y);
		vertex_array.push_back(normal_b.x); vertex_array.push_back(normal_b.y); vertex_array.push_back(normal_b.z);
		
		vertex_array.push_back(c.x); vertex_array.push_back(c.y); vertex_array.push_back(c.z);
		vertex_array.push_back(color_c.x); vertex_array.push_back(color_c.y); vertex_array.push_back(color_c.z); vertex_array.push_back(color_c.w);
		vertex_array.push_back(uv_c.x); vertex_array.push_back(uv_c.y);
		vertex_array.push_back(normal_c.x); vertex_array.push_back(normal_c.y); vertex_array.push_back(normal_c.z);

		if (!as_buffer) {
			auto VBO = unsigned int{};
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_array.size(), vertex_array.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(7 * sizeof(float)));
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(9 * sizeof(float)));


			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glBindVertexArray(0);
		}
	}

	auto Triangle::set_tex(const std::string& in_tex_name) noexcept -> void
	{
		tex_name = in_tex_name;
	}

	auto Triangle::draw() -> void
	{
		if (!as_buffer) {
			auto manager = core::resource::Text_Manager::instance();
			auto tex = manager->get_texture(tex_name);
			if (tex) {
				tex->bind();
			}
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);
		}
	}

	Sphere::Sphere(const vec3& in_origin, float in_radius, int in_tessellation_x, int in_tessellation_y, vec4 in_color):
		origin{in_origin},
		radius{in_radius},
		tessellation_x{in_tessellation_x},
		tessellation_y{in_tessellation_y},
		bounding_box{in_origin - vec3{in_radius, in_radius, in_radius}, in_origin + vec3{in_radius, in_radius, in_radius}}
	{
		if (tessellation_x < 1) tessellation_x = 1;
		if (tessellation_y < 2) tessellation_y = 2;
		auto step_angle_x = 2 * std::numbers::pi / (tessellation_x);
		auto step_angle_y = std::numbers::pi / (tessellation_y - 1);
		for (int x = 0; x < tessellation_x; x++) {
			auto now_position = vec3{};
			auto now_angle_x = step_angle_x * x;
			for (int y = 0; y < tessellation_y; y++) {
				auto now_angle_y = step_angle_y * y;
				auto temp_z = std::sinf(now_angle_y);
				now_position.y = std::cosf(now_angle_y);

				// x direction start from -x
				now_position.z = std::sinf(now_angle_x) * temp_z;
				now_position.x = - std::cosf(now_angle_x) * temp_z;

				auto now_vertex = Vertex{};
				now_vertex.position = now_position * radius + origin;
				now_vertex.color = in_color;
				now_vertex.normal = glm::normalize(now_position);
				now_vertex.uv = vec2{ static_cast<float>(x) / static_cast<float>(tessellation_x - 1), static_cast<float>(y) / static_cast<float>(tessellation_y) };
				triangle_mesh.mesh_vertex.push_back(now_vertex);
			}
		}

		//EBO
		auto vertex_number = tessellation_x * tessellation_y;
		for (int x = 0; x < tessellation_x; x++) {
			for (int y = 0; y < tessellation_y - 1; y++) {
				auto index_a = tessellation_y * x + y;
				auto index_b = index_a + 1;
				auto index_c = (index_a + tessellation_y) % vertex_number;
				auto index_d = (index_b - tessellation_y + vertex_number) % vertex_number;
				triangle_mesh.vertex_index.push_back(index_a);
				triangle_mesh.vertex_index.push_back(index_b);
				triangle_mesh.vertex_index.push_back(index_c);
				triangle_mesh.vertex_index.push_back(index_a);
				triangle_mesh.vertex_index.push_back(index_b);
				triangle_mesh.vertex_index.push_back(index_d);
			}
		}

		//uv


		auto VBO = unsigned int{};
		auto EBO = unsigned int{};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangle_mesh.vertex_index.size(), triangle_mesh.vertex_index.data(), GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * triangle_mesh.mesh_vertex.size(), triangle_mesh.mesh_vertex.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(7 * sizeof(float)));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(9 * sizeof(float)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glBindVertexArray(0);


	}

	auto Sphere::set_tex(const std::string& in_tex_name) noexcept -> void
	{
		tex_name = in_tex_name;
	}

	auto Sphere::draw() const noexcept -> void
	{
		auto manager = core::resource::Text_Manager::instance();
		auto tex = manager->get_texture(tex_name);
		if (tex) {
			tex->bind();
		}
		//std::cout << "sizeee: "<<triangle_mesh.vertex_index.size() << std::endl;
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, triangle_mesh.vertex_index.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	auto Sphere::intersect_mesh(const Ray& ray) -> bool
	{
		if (!bounding_box.intersect(ray)) return false;

		return triangle_mesh.intersect(ray);

	}

	auto Triangle_Mesh::intersect(const Ray& ray) -> bool
	{
		for (int i = 0; i < static_cast<int>(vertex_index.size()) - 2; i += 3) {
			auto& p0 = mesh_vertex[vertex_index[i]].position;
			auto& p1 = mesh_vertex[vertex_index[i + 1]].position;
			auto& p2 = mesh_vertex[vertex_index[i + 2]].position;
			if (core::control::test_intersect_with_triangle(ray, p0, p1, p2)) return true;
		}
		return false;
	}

}