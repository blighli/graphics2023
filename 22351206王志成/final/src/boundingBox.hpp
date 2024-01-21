#pragma once

#include "utils.hpp"

struct BoundingBox
{
    auto getCenter()
    {
        auto pos = minPoint;
        pos += maxPoint;
        pos *= .5f;
        return pos;
    }

    void extend(const nvmath::vec3 p)
    {
        minPoint = {std::min(minPoint.x, p.x), std::min(minPoint.y, p.y), std::min(minPoint.z, p.z)};
        maxPoint = {std::max(maxPoint.x, p.x), std::max(maxPoint.y, p.y), std::max(maxPoint.z, p.z)};
    }

    nvmath::vec3 minPoint{};
    nvmath::vec3 maxPoint{};
};