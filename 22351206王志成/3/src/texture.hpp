#pragma once

#include <string>

#include <nvvk/resourceallocator_vk.hpp>

struct Texture
{
    std::string name{""};

    uint32_t width{};
    uint32_t height{};
    VkFormat format{};

    void* cpuHandle{nullptr};
    nvvk::Texture gpuHandle{};
};