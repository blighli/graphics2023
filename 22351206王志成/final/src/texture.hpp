#pragma once

#include <string>

#include <nvvk/resourceallocator_vk.hpp>

struct Texture
{
    std::string name{""};

    int32_t width{};
    int32_t height{};
    VkFormat format{};

    void* cpuHandle{nullptr};
    nvvk::Texture gpuHandle{};
};