#pragma once

#include "vulkan/vulkan.hpp"
#include <memory>
#include <vector>

namespace app {

class DescriptorSetManager final {
public:
  struct SetInfo {
    vk::DescriptorSet set;
    vk::DescriptorPool pool;
  };

  static void Init(uint32_t maxFlight) {
    instance =
        std::make_unique<DescriptorSetManager>(maxFlight);
  }

  static void Quit() {
    instance.reset();
  }

  static auto Instance() -> DescriptorSetManager & {
    return *instance;
  }

  DescriptorSetManager(uint32_t maxFlight);
  ~DescriptorSetManager();

  auto AllocBufferSets(uint32_t num)
      -> std::vector<SetInfo>;
  auto AllocImageSet() -> SetInfo;

  void FreeImageSet(const SetInfo &);

  // private:
  struct PoolInfo {
    vk::DescriptorPool pool;
    uint32_t remainNum_;
  };

  PoolInfo bufferSetPool;

  std::vector<PoolInfo> fulledImageSetPool;
  std::vector<PoolInfo> avalibleImageSetPool;

  void addImageSetPool();
  auto getAvaliableImagePoolInfo() -> PoolInfo &;

  uint32_t maxFlight;

  static std::unique_ptr<DescriptorSetManager> instance;
};

} // namespace app