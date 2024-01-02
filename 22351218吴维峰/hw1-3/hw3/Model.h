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
    
    vector<Texture> textures_loaded;
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    Model(string const& path, bool gamma = false);

    
    void Draw(Shader& shader);

private:
    
    void loadModel(string const& path);

    
    void processNode(aiNode* node, const aiScene* scene);
    
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};


unsigned int TextureFromFile(const char* path, const string& directory, bool gamma);
#endif