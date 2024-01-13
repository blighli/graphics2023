#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include<stdio.h>
#include<iostream>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#define M_PI 3.1415926
#include"camara.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include<cstring>
#include<vector>
#ifndef MESH_H
#define MESH_H
//模型加载结构定义
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};
struct Texture {
    unsigned int id;
    std::string type;
    aiString path;
};
class Mesh {
public:
    /*  网格数据  */
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    /*  函数  */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader shader);
private:
    /*  渲染数据  */
    unsigned int VAO, VBO, EBO;
    /*  函数  */
    void setupMesh();
};
#endif