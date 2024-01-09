#include "../header/descriptorManager.h"
#include "../header/application.h"
#include <cstdint>

namespace app {

std::unique_ptr<DescriptorSetManager>
    DescriptorSetManager::instance = nullptr;

DescriptorSetManager::DescriptorSetManager(
    uint32_t maxFlight)
    : maxFlight(maxFlight) {
  std::array<vk::DescriptorPoolSize, 2> size;
  size[0]
      .setType(vk::DescriptorType::eUniformBuffer)
      .setDescriptorCount(maxFlight);
  size[1]
      .setType(vk::DescriptorType::eCombinedImageSampler)
      .setDescriptorCount(maxFlight);
  vk::DescriptorPoolCreateInfo createInfo;
  createInfo.setMaxSets(maxFlight).setPoolSizes(size);
  auto pool = Application::GetInstance()
                  .device.createDescriptorPool(createInfo);
  bufferSetPool.pool = pool;
  bufferSetPool.remainNum_ = maxFlight;
}

// DescriptorSetManager::DescriptorSetManager(uint32_t
// maxFlight)
//     : maxFlight(maxFlight) {
//   vk::DescriptorPoolSize size;
//   size.setType(vk::DescriptorType::eUniformBuffer)
//       .setDescriptorCount(maxFlight);
//   vk::DescriptorPoolCreateInfo createInfo;
//   createInfo.setMaxSets(maxFlight).setPoolSizes(size);
//   auto pool =
//       Application::GetInstance().device.createDescriptorPool(createInfo);
//   bufferSetPool.pool = pool;
//   bufferSetPool.remainNum_ = maxFlight;
// }

DescriptorSetManager::~DescriptorSetManager() {
  auto &device = Application::GetInstance().device;

  device.destroyDescriptorPool(bufferSetPool.pool);
  for (auto pool : fulledImageSetPool) {
    device.destroyDescriptorPool(pool.pool);
  }
  for (auto pool : avalibleImageSetPool) {
    device.destroyDescriptorPool(pool.pool);
  }
}

void DescriptorSetManager::addImageSetPool() {
  constexpr uint32_t MaxSetNum = 10;

  vk::DescriptorPoolSize size;
  size.setType(vk::DescriptorType::eCombinedImageSampler)
      .setDescriptorCount(MaxSetNum);
  vk::DescriptorPoolCreateInfo createInfo;
  createInfo.setMaxSets(MaxSetNum)
      .setPoolSizes(size)
      .setFlags(vk::DescriptorPoolCreateFlagBits::
              eFreeDescriptorSet);
  auto pool = Application::GetInstance()
                  .device.createDescriptorPool(createInfo);
  avalibleImageSetPool.push_back({pool, MaxSetNum});
}

auto DescriptorSetManager::AllocBufferSets(uint32_t num)
    -> std::vector<DescriptorSetManager::SetInfo> {
  std::vector<vk::DescriptorSetLayout> layouts(maxFlight,
      Application::GetInstance().shader->layouts[0]);
  vk::DescriptorSetAllocateInfo allocInfo;
  allocInfo.setDescriptorPool(bufferSetPool.pool)
      .setDescriptorSetCount(num)
      .setSetLayouts(layouts);
  auto sets = Application::GetInstance()
                  .device.allocateDescriptorSets(allocInfo);

  std::vector<SetInfo> result(num);

  for (uint32_t i = 0; i < num; i++) {
    result[i].set = sets[i];
    result[i].pool = bufferSetPool.pool;
  }

  return result;
}

// auto DescriptorSetManager::AllocImageSet() ->
// DescriptorSetManager::SetInfo {
//   std::vector<vk::DescriptorSetLayout> layouts{
//     maxFlight,
//       Application::GetInstance().shader->layouts[0]};
//   vk::DescriptorSetAllocateInfo allocInfo;
//   auto &poolInfo = getAvaliableImagePoolInfo();
//   allocInfo.setDescriptorPool(poolInfo.pool)
//       .setDescriptorSetCount(maxFlight)
//       .setSetLayouts(layouts);
//   auto sets =
//       Application::GetInstance().device.allocateDescriptorSets(allocInfo);

//   SetInfo result;
//   result.pool = poolInfo.pool;
//   result.set = sets[0];

//   poolInfo.remainNum_ =
//       std::max<int>(static_cast<int>(poolInfo.remainNum_)
//       - sets.size(), 0);
//   if (poolInfo.remainNum_ == 0) {
//     fulledImageSetPool.push_back(poolInfo);
//     avalibleImageSetPool.pop_back();
//   }

//   return result;
// }

auto DescriptorSetManager::AllocImageSet()
    -> DescriptorSetManager::SetInfo {
  std::vector<vk::DescriptorSetLayout> layouts{
      Application::GetInstance().shader->layouts[0]};
  vk::DescriptorSetAllocateInfo allocInfo;
  auto &poolInfo = getAvaliableImagePoolInfo();
  allocInfo.setDescriptorPool(poolInfo.pool)
      .setDescriptorSetCount(1)
      .setSetLayouts(layouts);
  auto sets = Application::GetInstance()
                  .device.allocateDescriptorSets(allocInfo);

  SetInfo result;
  result.pool = poolInfo.pool;
  result.set = sets[0];

  poolInfo.remainNum_ = std::max<int>(
      static_cast<int>(poolInfo.remainNum_) - sets.size(),
      0);
  if (poolInfo.remainNum_ == 0) {
    fulledImageSetPool.push_back(poolInfo);
    avalibleImageSetPool.pop_back();
  }

  return result;
}

void DescriptorSetManager::FreeImageSet(
    const SetInfo &info) {
  auto it = std::find_if(fulledImageSetPool.begin(),
      fulledImageSetPool.end(),
      [&](const PoolInfo &poolInfo) {
        return poolInfo.pool == info.pool;
      });
  if (it != fulledImageSetPool.end()) {
    it->remainNum_++;
    avalibleImageSetPool.push_back(*it);
    fulledImageSetPool.erase(it);
    return;
  }

  it = std::find_if(avalibleImageSetPool.begin(),
      avalibleImageSetPool.end(),
      [&](const PoolInfo &poolInfo) {
        return poolInfo.pool == info.pool;
      });
  if (it != avalibleImageSetPool.end()) {
    it->remainNum_++;
    return;
  }
}

auto DescriptorSetManager::getAvaliableImagePoolInfo()
    -> DescriptorSetManager::PoolInfo & {
  if (avalibleImageSetPool.empty()) {
    addImageSetPool();
    return avalibleImageSetPool.back();
  }
  return avalibleImageSetPool.back();
}

} // namespace app
