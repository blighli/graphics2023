#pragma once

#include "vulkan/vulkan.hpp"
#include <functional>

namespace app {

class CommandManager final {
public:
  CommandManager();
  ~CommandManager();

  auto CreateOneCommandBuffer() -> vk::CommandBuffer;
  auto CreateCommandBuffers(std::uint32_t count)
      -> std::vector<vk::CommandBuffer>;
  void ResetCmds();
  void FreeCmd(const vk::CommandBuffer &);

  using RecordCmdFunc =
      std::function<void(vk::CommandBuffer &)>;
  void ExecuteCmd(vk::Queue, RecordCmdFunc);

private:
  vk::CommandPool pool_;

  auto createCommandPool() -> vk::CommandPool;
};

} // namespace app