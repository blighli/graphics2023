#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <thread>

#include <alpaca/alpaca.h>

#include "modelLoader.h"

// bool ModelLoader::loadCache(std::vector<Mesh> &container, const std::filesystem::path &filePath)
// {
//     auto cachePath = filePath / ".cache";
//     std::ifstream input(cachePath, std::ios::in | std::ios::binary);
//     if (!input)
//         return false;

//     std::error_code error;
//     auto size = std::filesystem::file_size(cachePath);
//     struct sMergedMeshContainer
//     {
//         std::vector<Mesh> _;
//     };
//     auto read = alpaca::deserialize<sMergedMeshContainer>(input, size, error);
//     if (!error)
//         throw std::runtime_error("found model cache [" + cachePath.generic_string() + "], but deserialized with error " + error.message() + ".\n");

//     container = read._;
//     input.close();
//     return true;
// }

void ModelLoader::fullyReload(std::vector<Mesh> &container, const std::filesystem::path &filePath, const std::filesystem::path &materialPath, bool interpVertexNormal)
{
    auto data = rapidobj::ParseFile(filePath, materialPath.empty() ? rapidobj::MaterialLibrary::Default() : rapidobj::MaterialLibrary::SearchPath(materialPath));
    if (data.error)
        throw std::runtime_error(data.error.code.message());
    if (!rapidobj::Triangulate(data))
        throw std::runtime_error("loaded model [" + filePath.generic_string() + "] is failed to triangulate.\n");

    container.resize(data.shapes.size());
    std::vector<sMeshTask> task(data.shapes.size());
    for (auto i = 0; i < data.shapes.size(); ++i)
    {
        task[i].loadedData = &data;
        task[i].targetMesh = &container[i];
        task[i].shapeIndex = i;
    }

    auto concurrency = std::min(static_cast<size_t>(std::thread::hardware_concurrency()), task.size());
    auto threads = std::vector<std::thread>{};
    threads.reserve(concurrency);

    auto taskIndex = std::atomic_size_t{0ULL};
    auto activeThreads = std::atomic_size_t{concurrency};
    auto completed = std::promise<void>{};

    auto workerFunc = [&]()
    {
        auto activeTaskIndex = std::atomic_fetch_add(&taskIndex, 1ULL);
        while (activeTaskIndex < task.size())
        {
            {
                // avoid vertex duplicate
                std::unordered_map<VertexAttribute, uint32_t> verticesMap;
                // for vertex normal interpolation
                // smooth_group_index -> vertex_index -> face_index
                std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::vector<uint32_t>>> smoothGroupMap;

                auto attributes = &task[activeTaskIndex].loadedData->attributes;
                auto shape = &task[activeTaskIndex].loadedData->shapes[task[activeTaskIndex].shapeIndex];
                auto mesh = task[activeTaskIndex].targetMesh;
                mesh->name = shape->name;
                mesh->vertices.reserve(shape->mesh.indices.size());
                mesh->indices.reserve(shape->mesh.indices.size());
                for (const auto &index : shape->mesh.indices)
                {
                    VertexAttribute temp{};

                    temp.position = {
                        attributes->positions[3 * index.position_index + 0],
                        attributes->positions[3 * index.position_index + 1],
                        attributes->positions[3 * index.position_index + 2]};
                    if (index.normal_index != -1)
                        temp.normal = {
                            attributes->normals[3 * index.normal_index + 0],
                            attributes->normals[3 * index.normal_index + 1],
                            attributes->normals[3 * index.normal_index + 2]};
                    else if (!interpVertexNormal)
                        printf("WARNING: Loaded model [%s] has missing vertex normal and interpVertexNormal flag is off.\n", filePath.c_str());
                    if (index.texcoord_index != -1)
                    {
                        temp.uv = {
                            attributes->texcoords[2 * index.texcoord_index + 0],
                            attributes->texcoords[2 * index.texcoord_index + 1]};
                        // remap coords to [0, 1]
                        temp.uv.x -= std::floor(temp.uv.x);
                        temp.uv.y -= std::floor(temp.uv.y);
                        // according to Vulkan's coord system
                        temp.uv.y = 1.f - temp.uv.y;
                    }

                    if (verticesMap.count(temp) == 0)
                    {
                        verticesMap[temp] = mesh->vertices.size();
                        mesh->vertices.emplace_back(temp);
                        if (mesh->bounding.minPoint == nvmath::vec3f_zero && mesh->bounding.maxPoint == nvmath::vec3f_zero)
                            mesh->bounding.minPoint = mesh->bounding.maxPoint = temp.position;
                        else
                            mesh->bounding.extend(temp.position);
                    }
                    mesh->indices.emplace_back(verticesMap[temp]);
                }
                mesh->vertices.shrink_to_fit();
                mesh->indices.shrink_to_fit();

                mesh->faces.resize(shape->mesh.material_ids.size());
                auto maxSmoothGroupIndex = 0U;
                for (auto f = 0; f < shape->mesh.material_ids.size(); ++f)
                {
                    mesh->faces[f].materialIndex = shape->mesh.material_ids[f];
                    // assume triangle faces' front are CCW
                    auto vec1 = nvmath::normalize(mesh->vertices[mesh->indices[3 * f + 1]].position - mesh->vertices[mesh->indices[3 * f + 0]].position);
                    auto vec2 = nvmath::normalize(mesh->vertices[mesh->indices[3 * f + 2]].position - mesh->vertices[mesh->indices[3 * f + 1]].position);
                    mesh->faces[f].normal = nvmath::cross(vec1, vec2);
                    // allocate interp task
                    if (interpVertexNormal)
                    {
                        maxSmoothGroupIndex = std::max(maxSmoothGroupIndex, shape->mesh.smoothing_group_ids[f]);
                        if (shape->mesh.indices[3 * f + 0].normal_index == -1)
                        {
                            smoothGroupMap[shape->mesh.smoothing_group_ids[f]][mesh->indices[3 * f + 0]].emplace_back(f);
                            mesh->vertices[mesh->indices[3 * f + 0]].normal = mesh->faces[f].normal;
                        }
                        if (shape->mesh.indices[3 * f + 1].normal_index == -1)
                        {
                            smoothGroupMap[shape->mesh.smoothing_group_ids[f]][mesh->indices[3 * f + 1]].emplace_back(f);
                            mesh->vertices[mesh->indices[3 * f + 1]].normal = mesh->faces[f].normal;
                        }
                        if (shape->mesh.indices[3 * f + 2].normal_index == -1)
                        {
                            smoothGroupMap[shape->mesh.smoothing_group_ids[f]][mesh->indices[3 * f + 2]].emplace_back(f);
                            mesh->vertices[mesh->indices[3 * f + 2]].normal = mesh->faces[f].normal;
                        }
                    }
                }

                if (interpVertexNormal)
                {
                    for (auto i = 0; i < maxSmoothGroupIndex; ++i)
                    {
                        if (!smoothGroupMap.contains(i))
                            continue;

                        for (const auto &iter : smoothGroupMap[i])
                        {
                            auto &vertex = mesh->vertices[iter.first];
                            auto temp = nvmath::vec3f_zero;
                            for (const auto &faceIndex : iter.second)
                                temp += mesh->faces[faceIndex].normal;
                            temp = nvmath::normalize(temp);
                            if (temp != nvmath::vec3f_zero)
                                vertex.normal = temp;
                        }
                    }
                }
            }
            activeTaskIndex = std::atomic_fetch_add(&taskIndex, 1ULL);
        }

        if (std::atomic_fetch_sub(&activeThreads, 1ULL) == 1)
            completed.set_value();
    };

    for (auto i = 0; i < concurrency; ++i)
    {
        threads.emplace_back(workerFunc);
        threads.back().detach();
    }

    completed.get_future().wait();

    // for (const auto &material : data.materials)
    // {
    // }

    // auto cachePath = filePath / ".cache";
    // std::ofstream output(cachePath, std::ios::out | std::ios::binary);
    // if (!output)
    // {
    //     printf("WARNING: Failed to open output cache file corresponding to loaded model [%s].\n", filePath.c_str());
    //     output.close();
    //     return;
    // }

    // struct sMergedMeshContainer
    // {
    //     std::vector<Mesh> _;
    // } outData{container};
    // alpaca::serialize<sMergedMeshContainer>(outData, output);

    // output.close();
}