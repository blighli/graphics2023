#include "Primative.h"
#include "Texture.h"
#include <numbers>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
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
			auto manager = core::resource::Tex_Manager::instance();
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

				// x direction start from -z
				now_position.z = - std::cosf(now_angle_x) * temp_z;
				now_position.x = - std::sinf(now_angle_x) * temp_z;

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
		triangle_mesh.setup_mesh();

	}

	auto Sphere::set_tex(const std::string& in_tex_name) noexcept -> void
	{
		for (auto& tex : triangle_mesh.textures) {
			if (tex == in_tex_name) return;
		}
		triangle_mesh.textures.push_back(in_tex_name);
	}

	auto Sphere::draw(Shader& shader) const noexcept -> void
	{
		triangle_mesh.draw(shader);
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

	auto Triangle_Mesh::draw(Shader& shader) const noexcept -> void
	{
		unsigned int diffuse_nr = 1;
		unsigned int specular_nr = 1;
		unsigned int normal_nr = 1;
		unsigned int height_nr = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			std::string number;
			auto tex = core::resource::Tex_Manager::instance()->get_texture(textures[i]);
			std::string name = tex->kind;
			if (name == "texture_diffuse")
				number = std::to_string(diffuse_nr++);
			else if (name == "texture_specular")
				number = std::to_string(specular_nr++); // transfer unsigned int to string
			else if (name == "texture_normal")
				number = std::to_string(normal_nr++); // transfer unsigned int to string
			else if (name == "texture_height")
				number = std::to_string(height_nr++); // transfer unsigned int to string

			// now set the sampler to the correct texture unit
			shader.set_int(name + number, i);
			// and finally bind the texture
			tex->bind();
		}

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(vertex_index.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

	auto Triangle_Mesh::setup_mesh() noexcept -> void
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vertex_index.size(), vertex_index.data(), GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh_vertex.size(), mesh_vertex.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, uv)));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tangent)));
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, bitangent)));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, bonel_ids)));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, weight)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);

		glBindVertexArray(0);

	}

	auto Model::draw(core::shader::Shader& shader) -> void
	{
		for (auto& mesh : meshes) {
			mesh.draw(shader);
		}
	}
	
	auto Model::load_model(const std::string& path) -> void
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		process_node(scene->mRootNode, scene);
	}

	auto Model::process_node(aiNode* node, const aiScene* scene) -> void
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(process_mesh(mesh, scene));
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			process_node(node->mChildren[i], scene);
		}
	}

	auto Model::process_mesh(aiMesh* mesh, const aiScene* scene) -> Triangle_Mesh
	{
		// data to fill
		auto triangle_mesh = Triangle_Mesh{};

		// walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;
			// colors
			if (mesh->HasVertexColors(i))
			{
				auto& color = vertex.color;
				color.x = mesh->mColors[i]->r;
				color.y = mesh->mColors[i]->g;
				color.z = mesh->mColors[i]->b;
				color.w = mesh->mColors[i]->a;
			}
			// normals
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.normal = vector;
			}
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.uv = vec;
				// tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.bitangent = vector;
			} else {
				vertex.uv = glm::vec2(0.0f, 0.0f);
			}

			triangle_mesh.mesh_vertex.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				triangle_mesh.vertex_index.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		auto& textures = triangle_mesh.textures;
		auto diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
		// 2. specular maps
		auto specular_maps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
		// 3. normal maps
		auto normal_maps = load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
		// 4. height maps
		auto height_maps = load_material_textures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), height_maps.begin(), height_maps.end());

		triangle_mesh.setup_mesh();
		// return a mesh object created from the extracted mesh data
		return triangle_mesh;
	}

	auto Model::load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name) -> std::vector<std::string>
	{
		std::vector<std::string> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			auto tex_name = std::string{ str.C_Str() };
			auto tex_path = directory + '/' + tex_name;
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			auto tex_managet = core::resource::Tex_Manager::instance();
			auto texture = tex_managet->load_texture(tex_path, tex_path, type_name, false);
			if (texture) {   
			// if texture hasn't been loaded already, load it
				textures.push_back(tex_path);
			}
		}
		return textures;
	}


}