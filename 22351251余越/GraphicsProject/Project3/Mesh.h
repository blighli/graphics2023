#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Config.h"

using namespace std;

class Mesh {
public:
    // 网格数据
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

    // 绘制网格
    void Draw(Shader& shader);

private:
    // 缓存对象
    unsigned int VBO, EBO;

    // 初始化所有缓存对象
    void setupMesh();
};
#endif