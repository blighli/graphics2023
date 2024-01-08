#pragma once

#include <nvmath/nvmath_glsltypes.h>

namespace std
{
    inline void hash_combine(size_t &seed, size_t hash)
    {
        hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hash;
    }

    template <>
    struct hash<nvmath::vec2>
    {
        size_t operator()(nvmath::vec2 const &obj) const
        {
            size_t seed = 0ULL;
            hash_combine(seed, hash<float>()(obj.x));
            return seed;
        }
    };

    template <>
    struct hash<nvmath::vec3>
    {
        size_t operator()(nvmath::vec3 const &obj) const
        {
            size_t seed = 0ULL;
            hash_combine(seed, hash<float>()(obj.x));
            hash_combine(seed, hash<float>()(obj.y));
            hash_combine(seed, hash<float>()(obj.z));
            return seed;
        }
    };
}