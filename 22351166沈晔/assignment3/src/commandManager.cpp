#include "../header/commandManager.h"
#include "../header/application.h"

namespace app {

CommandManager::CommandManager() {
  pool_ = createCommandPool();
}

CommandManager::~CommandManager() {
  auto &app = Application::GetInstance();
  app.device.destroyCommandPool(pool_);
}

void CommandManager::ResetCmds() {
  Application::GetInstance().device.resetCommandPool(pool_);
}

auto CommandManager::createCommandPool()
    -> vk::CommandPool {
  auto &app = Application::GetInstance();

  vk::CommandPoolCreateInfo createInfo;

  createInfo
      .setQueueFamilyIndex(
          app.queueFamilyIndices.graphicQueue.value())
      .setFlags(vk::CommandPoolCreateFlagBits::
              eResetCommandBuffer);

  return app.device.createCommandPool(createInfo);
}

auto CommandManager::CreateCommandBuffers(
    std::uint32_t count) -> std::vector<vk::CommandBuffer> {
  auto &app = Application::GetInstance();

  vk::CommandBufferAllocateInfo allocInfo;
  allocInfo.setCommandPool(pool_)
      .setCommandBufferCount(1)
      .setLevel(vk::CommandBufferLevel::ePrimary);

  return app.device.allocateCommandBuffers(allocInfo);
}

auto CommandManager::CreateOneCommandBuffer()
    -> vk::CommandBuffer {
  return CreateCommandBuffers(1)[0];
}

void CommandManager::FreeCmd(
    const vk::CommandBuffer &cmdBuf) {
  Application::GetInstance().device.freeCommandBuffers(
      pool_, cmdBuf);
}

void CommandManager::ExecuteCmd(
    vk::Queue queue, RecordCmdFunc func) {
  auto cmdBuf = CreateOneCommandBuffer();

  vk::CommandBufferBeginInfo beginInfo;
  beginInfo.setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  cmdBuf.begin(beginInfo);
  if (func)
    func(cmdBuf);
  cmdBuf.end();

  vk::SubmitInfo submitInfo;
  submitInfo.setCommandBuffers(cmdBuf);
  queue.submit(submitInfo);
  queue.waitIdle();
  Application::GetInstance().device.waitIdle();
  FreeCmd(cmdBuf);
}

} // namespace app