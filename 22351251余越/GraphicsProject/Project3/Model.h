#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
    // model 数据
    // 存储已加载纹理，避免重复加载
    vector<Texture> textures_loaded;
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    Model(string const& path, bool gamma = false);

    // 绘制模型
    void Draw(Shader& shader);

private:
    // 加载Assimp支持的模型文件，存储网格数据
    void loadModel(string const& path);

    // 递归处理Assimp节点
    void processNode(aiNode* node, const aiScene* scene);
    // 处理网格数据
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    // 加载纹理，检查是否重复加载
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

// 加载纹理
unsigned int TextureFromFile(const char* path, const string& directory, bool gamma);
#endif