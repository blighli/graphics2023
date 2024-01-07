#pragma once

#include "shader.h"
#include "mesh.h"
//libs
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STBI_WINDOWS_UTF8

//std
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

inline uint32_t TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model
{
public:
    //模型数据  
    std::vector<Texture> textures_loaded;
    std::vector<Mesh>    meshes;
    std::string          directory;
    bool gammaCorrection;
    glm::mat4 position;

    //构建函数
    Model(std::string const& path, bool gamma = false);

    //绘制模型每个mesh
    void Draw(Shader shader);
private:
    //读取模型
    void loadModel(std::string path);
    //遍历网格结点
    void processNode(aiNode* node, const aiScene* scene);
    //处理网格
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    //加载材质纹理
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

