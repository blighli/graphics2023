#pragma once
#include "shader.h"
//lib
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//std
#include <string>
#include <vector> 
#define MAX_BONE_INFLUENCE 4


struct Vertex {
	//位置
	glm::vec3 Position;
	//法向量
	glm::vec3 Normal;
	//纹理坐标
	glm::vec2 TexCoords;
	//切向
	glm::vec3 Tangent;
	//bitangent
	glm::vec3 Bitangent;
	//会影响到顶点的骨骼编号
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//骨骼影响的权重
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
	uint32_t id;
	std::string type;
	std::string path;
};

class Mesh {
public:
	//网格数据
	std::vector<Vertex>           vertices;
	std::vector<uint32_t>         indices;
	std::vector<Texture>          textures;
	uint32_t VAO;

	//构造函数
	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);

	//渲染
	void Draw(Shader& shader);

private:
	uint32_t VBO, EBO;
	void setupMesh();
};