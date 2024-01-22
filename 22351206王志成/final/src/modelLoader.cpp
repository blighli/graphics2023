#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <thread>

#include <stb_image.h>
#include <alpaca/alpaca.h>

#include "modelLoader.h"

// bool ModelLoader::loadCache(std::vector<Mesh> &meshContainer, std::vector<Texture> &texContainer, std::vector<Material> &matContainer, const std::filesystem::path &filePath)
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

//     meshContainer = read._;
//     input.close();
//     return true;
// }

void ModelLoader::fullyReload(std::vector<Mesh> &meshContainer, std::vector<Material> &matContainer, std::vector<Texture> &texContainer,
                              const std::filesystem::path &filePath, const std::filesystem::path &texPath, const std::filesystem::path &materialPath,
                              bool interpVertexNormal)
{
    auto data = rapidobj::ParseFile(filePath, materialPath.empty() ? rapidobj::MaterialLibrary::Default() : rapidobj::MaterialLibrary::SearchPath(materialPath));
    if (data.error)
        throw std::runtime_error(data.error.code.message());
    if (!rapidobj::Triangulate(data))
        throw std::runtime_error("loaded model [" + filePath.generic_string() + "] is failed to triangulate.\n");

    meshContainer.resize(data.shapes.size());
    std::vector<sMeshTask> task(data.shapes.size());
    for (auto i = 0; i < data.shapes.size(); ++i)
    {
        task[i].loadedData = &data;
        task[i].targetMesh = &meshContainer[i];
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
                    // else if (!interpVertexNormal)
                    //     printf("WARNING: Loaded model [%s] has missing vertex normal and interpVertexNormal flag is off.\n", filePath.generic_string());
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
                    mesh->faces[f].materialIndex = shape->mesh.material_ids[f] + matContainer.size();
                    // assume triangle faces' front are CCW
                    auto vec1 = nvmath::normalize(mesh->vertices[mesh->indices[3 * f + 1]].position - mesh->vertices[mesh->indices[3 * f + 0]].position);
                    auto vec2 = nvmath::normalize(mesh->vertices[mesh->indices[3 * f + 2]].position - mesh->vertices[mesh->indices[3 * f + 1]].position);
                    mesh->faces[f].normal = nvmath::normalize(nvmath::cross(vec1, vec2));
                    // allocate interp task
                    if (interpVertexNormal)
                    {
                        maxSmoothGroupIndex = std::max(maxSmoothGroupIndex, shape->mesh.smoothing_group_ids[f]);
                        if (shape->mesh.indices[3 * f + 0].normal_index == -1)
                            smoothGroupMap[shape->mesh.smoothing_group_ids[f]][mesh->indices[3 * f + 0]].emplace_back(f);
                        if (shape->mesh.indices[3 * f + 1].normal_index == -1)
                            smoothGroupMap[shape->mesh.smoothing_group_ids[f]][mesh->indices[3 * f + 1]].emplace_back(f);
                        if (shape->mesh.indices[3 * f + 2].normal_index == -1)
                            smoothGroupMap[shape->mesh.smoothing_group_ids[f]][mesh->indices[3 * f + 2]].emplace_back(f);
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

    if (texPath.empty())
    {
        printf("ERROR: Invalid texture input path.\n");
        return;
    }
    stbi_set_flip_vertically_on_load(true);
    std::unordered_map<std::string, uint32_t> texMap{};
    for (const auto &material : data.materials)
    {
        Material temp{};
        temp.name = material.name;
        memcpy(&temp.properties.ambient, material.ambient.data(), 3 * sizeof(float));
        memcpy(&temp.properties.diffuse, material.diffuse.data(), 3 * sizeof(float));
        memcpy(&temp.properties.specular, material.specular.data(), 3 * sizeof(float));
        memcpy(&temp.properties.transmittance, material.transmittance.data(), 3 * sizeof(float));
        memcpy(&temp.properties.emission, material.emission.data(), 3 * sizeof(float));
        temp.properties.dissolve = material.dissolve;
        temp.properties.illum = material.illum;
        temp.properties.shininess = material.shininess;
        temp.properties.ior = material.ior;
        temp.properties.roughness = material.roughness;
        temp.properties.metallic = material.metallic;
        temp.properties.sheen = material.sheen;
        temp.properties.clearcoat_roughness = material.clearcoat_roughness;
        temp.properties.clearcoat_thickness = material.clearcoat_thickness;
        temp.properties.anisotropy = material.anisotropy;
        temp.properties.anisotropy_rotation = material.anisotropy_rotation;

        if (!material.diffuse_texname.empty())
        {
            if (texMap.find(material.diffuse_texname) == texMap.end())
            {
                texMap[material.diffuse_texname] = texContainer.size();
                texContainer.push_back({});
                texContainer.back().name = material.diffuse_texname;
                texContainer.back().format = VK_FORMAT_R8G8B8A8_UNORM;

                int channel;
                auto data = stbi_load((texPath / material.diffuse_texname).generic_string().c_str(), &texContainer.back().width, &texContainer.back().height, &channel, 4);
                texContainer.back().cpuHandle = new unsigned char[texContainer.back().width * texContainer.back().height * 4];
                memcpy(texContainer.back().cpuHandle, data, texContainer.back().width * texContainer.back().height * 4);
                stbi_image_free(data);

                if (!material.alpha_texname.empty())
                {
                    int width, height;
                    auto data = stbi_load((texPath / material.alpha_texname).generic_string().c_str(), &width, &height, &channel, 0);
                    if (width != texContainer.back().width || height != texContainer.back().height)
                        printf("ERROR: alpha texture has different size than diffuse texture!\n");
                    else
                    {
                        if (channel > 1)
                            printf("WARNING: alpha texture has more than 1 channel, only the first channel will be considered.\n");

                        for (auto x = 0; x < width; ++x)
                            for (auto y = 0; y < width; ++y)
                                *(static_cast<unsigned char*>(texContainer.back().cpuHandle) + (y * width * 4) + x * 4 + 3) =
                                    *(data + (y * width * channel) + x * channel);
                    }
                    stbi_image_free(data);
                }
            }
            temp.properties.diffuse_map_index = texMap[material.diffuse_texname];
        }

        if (!material.reflection_texname.empty())
        {
            if (texMap.find(material.reflection_texname) == texMap.end())
            {
                texMap[material.reflection_texname] = texContainer.size();
                texContainer.push_back({});
                texContainer.back().name = material.reflection_texname;
                texContainer.back().format = VK_FORMAT_R8G8B8A8_UNORM;

                int channel;
                auto data = stbi_load((texPath / material.reflection_texname).generic_string().c_str(), &texContainer.back().width, &texContainer.back().height, &channel, 4);
                texContainer.back().cpuHandle = new unsigned char[texContainer.back().width * texContainer.back().height * 4];
                memcpy(texContainer.back().cpuHandle, data, texContainer.back().width * texContainer.back().height * 4);
                stbi_image_free(data);
            }
            temp.properties.reflection_map_index = texMap[material.reflection_texname];
        }

        matContainer.emplace_back(temp);
    }

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
    // } outData{meshContainer};
    // alpaca::serialize<sMergedMeshContainer>(outData, output);

    // output.close();
}