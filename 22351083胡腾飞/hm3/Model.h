#include "Mesh.h"
#ifndef MODEL_H
#define MODEL_H
class Model
{
public:
    std::vector<Texture> textures_loaded;
    /*  模型数据  */
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;
    /*  函数   */
    Model(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }
    void Draw(Shader &shader);
private:
    /*  函数   */
    void loadModel(std::string const &path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        std::string typeName);
};
#endif