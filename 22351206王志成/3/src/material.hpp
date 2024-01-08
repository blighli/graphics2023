#pragma once

#include <string>

#include "utils.hpp"

alignas(16) struct MaterialAttribute
{
    /* basic */
    // shuffle variables to keep memory order compact
    nvmath::vec3 ambient = {0, 0, 0};       // Ka
    uint32_t ambient_map_index = {0x7FFFFFFF};
    nvmath::vec3 diffuse = {0, 0, 0};       // Kd
    uint32_t diffuse_map_index = {0x7FFFFFFF};
    nvmath::vec3 specular = {0, 0, 0};      // Ks
    uint32_t specular_map_index = {0x7FFFFFFF};
    nvmath::vec3 transmittance = {0, 0, 0}; // Kt
    float dissolve = 1.0f;                  // d
    nvmath::vec3 emission = {0, 0, 0};      // Ke
    int illum = 0;                          // illum
    float shininess = 1.0f;                 // Ns
    float ior = 1.0f;                       // Ni
    uint32_t normal_map_index = {0x7FFFFFFF};
    uint32_t displacement_map_index = {0x7FFFFFFF};
    uint32_t reflection_map_index = {0x7FFFFFFF};
    
    /* PBR's */
    float roughness = 0.0f;           // Pr
    float metallic = 0.0f;            // Pm
    float sheen = 0.0f;               // Ps
    float clearcoat_thickness = 0.0f; // Pc
    float clearcoat_roughness = 0.0f; // Pcr
    float anisotropy = 0.0f;          // aniso
    float anisotropy_rotation = 0.0f; // anisor
    uint32_t basic_pbr_map_index = {0x7FFFFFFF};
    uint32_t extra_pbr_map_index = {0x7FFFFFFF};
};

struct Material
{
    std::string name{""};

    /* simple parameters */
    MaterialAttribute properties{};
};