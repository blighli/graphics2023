#pragma once
#include "Texture.h"
#include "Control.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

namespace core::primative
{
	using glm::vec2;
	using glm::vec3;
	using glm::vec4;
	using core::control::Ray;
	using core::control::Bounding_Box;

	namespace
	{
		using core::shader::Shader;
		constexpr int MAX_BONE_INFLUENCE = 4;
		auto model_prefix = std::string{ "./assets/model/" };
		auto add_model_prefix(const std::string& str) -> std::string
		{
			return model_prefix + str;
		}
	}

	struct Vertex
	{
		vec3 position;
		vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		vec2 uv;
		vec3 normal;
		vec3 tangent{ 0.0f, 0.0f, 0.0f };
		vec3 bitangent{ 0.0f, 0.0f, 0.0f };
		int bonel_ids[MAX_BONE_INFLUENCE]{ 0 };
		float weight[MAX_BONE_INFLUENCE]{ 0.0f };

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
		std::vector<std::string> textures;
		auto intersect(const Ray& ray) -> bool;
		auto setup_mesh() noexcept -> void;
		auto draw(Shader& shader) const noexcept -> void;
	private:
		unsigned int VAO, VBO, EBO;
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
		auto draw(Shader& shader) const noexcept -> void;
		auto intersect_mesh(const Ray& ray) -> bool;
		auto intersect_sphere(const Ray& ray) -> bool;

	private:
		Bounding_Box bounding_box;
		Triangle_Mesh triangle_mesh;
		std::string tex_name{"_None"};
		unsigned int VAO;
	};

	struct Model
	{
		Model(const std::string& path, bool default_prefix = true)
		{
			auto model_path = default_prefix ? add_model_prefix(path) : path;
			load_model(model_path);
		}
		auto draw(Shader& shader) -> void;

	private:
		std::vector<Triangle_Mesh> meshes;
		std::string directory;
		auto load_model(const std::string& path) -> void;

		auto process_node(aiNode* node, const aiScene* scene) -> void;
		auto process_mesh(aiMesh* mesh, const aiScene* scene) -> Triangle_Mesh;
		auto load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name) -> std::vector<std::string>;
	};

}