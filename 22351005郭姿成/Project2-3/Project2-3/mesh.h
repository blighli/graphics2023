#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
using namespace std;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};


enum ETextureType {
	ETextureTypeDiffuse = 1, //漫反射
	ETextureTypeSpecular = 2 //镜面反射
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

class Mesh {
public:
	//网格数据
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	Material material;
	//构造器
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material material) {
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->material = material;
		setupMesh();
	}
	void Draw(Shader &shader, bool vertexMode) {
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++) {
			// 在绑定之前激活相应的纹理单元
			glActiveTexture(GL_TEXTURE0 + i);
			// 获取纹理序号
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse") {
				number = to_string(diffuseNr++);
			}
			else if (name == "texture_specular") {
				number = to_string(specularNr++);
			}
			else if (name == "texture_normal") {
				number = std::to_string(normalNr++);
			}
			else if (name == "texture_height") {
				number = std::to_string(heightNr++);
			}
			glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		shader.setVec3("material.ambient", this->material.ambient);
		shader.setVec3("material.diffuse", this->material.diffuse);
		shader.setVec3("material.specular", this->material.specular);
		shader.setFloat("material.shininess", this->material.shininess);
		// 绘制网格
		glBindVertexArray(VAO);
		if (!vertexMode) {
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}
		else {
			glPointSize(8.0f);
			glDrawArrays(GL_POINTS, 0, this->vertices.size());
		}
		glBindVertexArray(0);
	}
private:
	unsigned int VAO, VBO, EBO;

	void setupMesh() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// 顶点位置
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// 顶点法线
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// 顶点纹理坐标
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// 顶点切线
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// 顶点余切线
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);
	}
};
#endif

