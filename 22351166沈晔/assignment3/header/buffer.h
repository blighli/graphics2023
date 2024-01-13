#pragma once

#include <vulkan/vulkan.hpp>

namespace app {

struct BufferPkg {
  vk::Buffer buffer;
  vk::DeviceMemory memory;
  void *map;
  size_t size;
  size_t requireSize;

  BufferPkg(size_t size, vk::BufferUsageFlags usage,
      vk::MemoryPropertyFlags property);
  ~BufferPkg();

  BufferPkg(const BufferPkg &) = delete;
  auto operator=(const BufferPkg &) -> BufferPkg & = delete;
};

auto QueryBufferMemTypeIndex(std::uint32_t requirementBit,
    vk::MemoryPropertyFlags) -> std::uint32_t;

} // namespace app