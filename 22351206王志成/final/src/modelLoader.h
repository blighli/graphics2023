#pragma once

#include <rapidobj/rapidobj.hpp>

#include "mesh.hpp"
#include "material.hpp"
#include "texture.hpp"

class ModelLoader
{
public:
    ModelLoader(const ModelLoader &) = delete;
    ModelLoader(ModelLoader &&) = delete;
    ModelLoader &operator=(const ModelLoader &) = delete;
    ModelLoader &operator=(ModelLoader &&) = delete;

    static ModelLoader &getInstance()
    {
        static ModelLoader instance;
        return instance;
    }

    void load(std::vector<Mesh> &meshContainer, std::vector<Material> &matContainer, std::vector<Texture> &texContainer,
              const std::filesystem::path &filePath, const std::filesystem::path &texPath, const std::filesystem::path &materialPath = "",
              bool interpVertexNormal = true)
    {
        // if (!loadCache(meshContainer, matContainer, texContainer, filePath))
        fullyReload(meshContainer, matContainer, texContainer, filePath, texPath, materialPath, interpVertexNormal);
    }
    auto load(const std::filesystem::path &filePath, const std::filesystem::path &texPath, const std::filesystem::path &materialPath = "", bool interpVertexNormal = true)
        -> std::tuple<std::vector<Mesh>, std::vector<Material>, std::vector<Texture>>
    {
        std::vector<Mesh> resMesh{};
        std::vector<Material> resMat{};
        std::vector<Texture> resTex{};
        load(resMesh, resMat, resTex, filePath, texPath, materialPath, interpVertexNormal);
        return std::tuple{resMesh, resMat, resTex};
    }

private:
    ModelLoader() {}
    ~ModelLoader() {}

    // bool loadCache(std::vector<Mesh> &meshContainer, std::vector<Material> &matContainer, std::vector<Texture> &texContainer, const std::filesystem::path &filePath);
    void fullyReload(std::vector<Mesh> &meshContainer, std::vector<Material> &matContainer, std::vector<Texture> &texContainer,
                     const std::filesystem::path &filePath, const std::filesystem::path &texPath, const std::filesystem::path &materialPath = "",
                     bool interpVertexNormal = true);

    struct sMeshTask
    {
        const rapidobj::Result *loadedData{nullptr};
        Mesh *targetMesh{nullptr};
        uint32_t shapeIndex{~0U};
    };
};