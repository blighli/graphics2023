#pragma once

#include <rapidobj/rapidobj.hpp>

#include "mesh.hpp"
#include "material.hpp"

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

    void load(std::vector<Mesh> &container, const std::filesystem::path &filePath, const std::filesystem::path &materialPath = "", bool interpVertexNormal = true)
    {
        // if (!loadCache(container, filePath))
            fullyReload(container, filePath, materialPath, interpVertexNormal);
    }
    auto load(const std::filesystem::path &filePath, const std::filesystem::path &materialPath = "", bool interpVertexNormal = true) -> std::vector<Mesh>
    {
        std::vector<Mesh> res{};
        load(res, filePath, materialPath, interpVertexNormal);
        return res;
    }

private:
    ModelLoader() {}
    ~ModelLoader() {}

    // bool loadCache(std::vector<Mesh> &container, const std::filesystem::path &filePath);
    void fullyReload(std::vector<Mesh> &container, const std::filesystem::path &filePath, const std::filesystem::path &materialPath = "", bool interpVertexNormal = true);

    struct sMeshTask
    {
        const rapidobj::Result *loadedData{nullptr};
        Mesh *targetMesh{nullptr};
        uint32_t shapeIndex{~0U};
    };
};