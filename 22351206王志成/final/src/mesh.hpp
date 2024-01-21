#pragma once

#include <string>
#include <vector>

#include <nvvk/resourceallocator_vk.hpp>

#include "utils.hpp"
#include "boundingBox.hpp"

/* basic primitive attributes */
struct alignas(16) VertexAttribute
{
    nvmath::vec3 position{nvmath::vec3f_zero};
    nvmath::vec3 normal{nvmath::vec3f_zero};
    nvmath::vec2 uv{nvmath::vec2f_zero};

    bool operator==(const VertexAttribute &other) const
    {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};

struct alignas(16) FaceAttribute
{
    nvmath::vec3 normal{nvmath::vec3f_zero};
    uint32_t materialIndex{0x7FFFFFFF};
};

namespace std
{
    template <>
    struct hash<VertexAttribute>
    {
        size_t operator()(VertexAttribute const &obj) const
        {
            size_t seed = 0ULL;
            hash_combine(seed, hash<nvmath::vec3>()(obj.position));
            hash_combine(seed, hash<nvmath::vec3>()(obj.normal));
            hash_combine(seed, hash<nvmath::vec2>()(obj.uv));
            return seed;
        }
    };
};

/* mesh struct */
struct Mesh
{
    std::string name{""};
    std::vector<VertexAttribute> vertices;
    std::vector<uint32_t> indices;
    std::vector<FaceAttribute> faces;
    BoundingBox bounding;
};