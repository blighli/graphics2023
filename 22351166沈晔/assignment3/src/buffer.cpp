#include "../header/buffer.h"
#include "../header/application.h"
#include <stdexcept>

namespace app {

BufferPkg::BufferPkg(size_t size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags memProperty) {
  auto &device = Application::GetInstance().device;

  this->size = size;
  vk::BufferCreateInfo createInfo;
  createInfo.setUsage(usage).setSize(size).setSharingMode(
      vk::SharingMode::eExclusive);

  buffer = device.createBuffer(createInfo);
  // vk 需要的内存大小，与我们申请的不同
  auto requirements =
      device.getBufferMemoryRequirements(buffer);
  requireSize = requirements.size;
  auto index = QueryBufferMemTypeIndex(
      requirements.memoryTypeBits, memProperty);
  if (index == 0) {
    throw std::runtime_error(
        "Pht Device can not support such memory!");
  }

  vk::MemoryAllocateInfo allocInfo;
  allocInfo.setMemoryTypeIndex(index).setAllocationSize(
      requirements.size);

  memory = device.allocateMemory(allocInfo);
  if (!memory) {
    std::cerr << "device alloc memory Failed!!!\n";
    throw std::runtime_error("Error: device alloc memory");
  }

  device.bindBufferMemory(buffer, memory, 0);
  // host 可见的需要记录
  if (memProperty &
      vk::MemoryPropertyFlagBits::eHostVisible) {
    map = device.mapMemory(memory, 0, size);
  } else {
    map = nullptr;
  }
}

BufferPkg::~BufferPkg() {
  auto &device = Application::GetInstance().device;
  if (map) {
    device.unmapMemory(memory);
  }
  device.freeMemory(memory);
  device.destroyBuffer(buffer);
}
// 查询硬件设备的内存信息，返回支持的一块内存
auto QueryBufferMemTypeIndex(std::uint32_t type,
    vk::MemoryPropertyFlags flag) -> std::uint32_t {
  auto property = Application::GetInstance()
                      .phyDevice.getMemoryProperties();

  for (std::uint32_t i = 0; i < property.memoryTypeCount;
       i++) {
    if ((1 << i) & type &&
        property.memoryTypes[i].propertyFlags & flag) {
      return i;
    }
  }

  return 0;
}

} // namespace app