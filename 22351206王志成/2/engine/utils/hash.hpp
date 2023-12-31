#pragma once

#include <cstdint>
#include <memory>

#include "metaProgram.hpp"

template <typename T>
std::size_t hash(const T &obj);

template <typename... Ts>
std::size_t hash(const Ts &...objs)
{
    std::size_t res{0U};
    hash_param(res, (..., hash(objs)));
    return res;
}

inline void hash_combine(std::size_t &seed, const std::size_t &hashCode)
{
    const auto temp = hashCode + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= temp;
}

inline void hash_param(std::size_t &seed, const std::size_t &direct_hash_code)
{
    hash_combine(seed, direct_hash_code);
}

template <typename T>
    requires is_container_type<T>
inline void hash_param(std::size_t &seed, const T &direct_hash_code)
{
    for (const auto &code : direct_hash_code)
        hash_combine(seed, code);
}

// template <size_t... Codes>
// inline void hash_param(std::size_t &seed, const Codes&...codes)
// {
//     hash_param(seed, codes...);
// }

template <typename T>
struct Aligned32Hasher
{
    std::size_t operator()(const T &obj) const
    {
        const uint32_t size = sizeof(T) / sizeof(uint32_t);
        const uint32_t *vBits = reinterpret_cast<const uint32_t *>(&obj);
        std::size_t seed = 0;
        for (uint32_t i = 0u; i < size; i++)
        {
            hash_combine(seed, vBits[i]);
        }
        return seed;
    }
};