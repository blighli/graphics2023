#pragma once

#include "resourceAllocator.hpp"

struct Buffer
{
    vk::Buffer buffer{};
    MemoryHandle memHandle{nullptr};
};

struct Image
{
    vk::Image image{};
    MemoryHandle memHandle{nullptr};
};

struct Texture
{
    vk::Image image{};
    MemoryHandle memHandle{nullptr};
    vk::DescriptorImageInfo descriptor{};
};