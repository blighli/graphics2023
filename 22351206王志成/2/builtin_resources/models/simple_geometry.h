#pragma once

#define _USE_MATH_DEFINES
#include <cstdint>
#include <cmath>
#include <vector>
#include <tuple>

#include <glm/glm.hpp>

struct VertexAttribute
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

inline auto generateSphere(const float &radius, const uint32_t &segmentX, const uint32_t &segmentY)
    -> std::pair<std::vector<float>, std::vector<uint32_t>>
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    for (auto x = 0; x <= segmentX; ++x)
    {
        for (auto y = 0; y <= segmentY; ++y)
        {
            float posX = radius * std::cos(x * 2.f * M_PI / segmentX) * std::sin(y * M_PI / segmentY);
            float posY = radius * std::cos(y * M_PI / segmentY);
            float posZ = radius * std::sin(x * 2.f * M_PI / segmentX) * std::sin(y * M_PI / segmentY);
            float texX = static_cast<float>(x) / segmentX;
            float texY = 1.f - static_cast<float>(y) / segmentY;
            // pos, normal, uv
            vertices.insert(vertices.end(), {posX, posY, posZ, posX, posY, posZ, texX, texY});

            if (x == segmentX || y == segmentY)
                continue;
            indices.insert(indices.end(), {x * (segmentX + 1) + y, (x + 1) * (segmentX + 1) + y, (x + 1) * (segmentX + 1) + y + 1});
            indices.insert(indices.end(), {x * (segmentX + 1) + y, (x + 1) * (segmentX + 1) + y + 1, x * (segmentX + 1) + y + 1});
        }
    }
    return std::pair{vertices, indices};
}