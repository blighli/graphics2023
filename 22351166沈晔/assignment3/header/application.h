#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>
// #define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <optional>
#include <set>

#include "renderProcess.h"
#include "renderer.h"
#include "shader.h"
#include "swapchain.h"
#include "commandManager.h"
#include "tool.h"

namespace app {

// 验证层和拓展层 showPropInfo() 可以输出信息
const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};
const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

// 图形与显示队列信息
struct QueueFamilyIndices {
  std::optional<uint32_t> graphicQueue;
  std::optional<uint32_t> presentQueue;
  operator bool() {
    return graphicQueue.has_value() &&
           presentQueue.has_value();
  }
};

// 应用实例（单例模式）
class Application final {
private:
  // 单例
  static std::unique_ptr<Application> instance_;
  // 私有构造函数，防止外部实例化
  Application(uint32_t w, uint32_t h)
      : width(w), height(h){};

public:
  static auto GetInstance() -> Application & {
    return *instance_;
  }
  static void Init(uint32_t w, uint32_t h);
  static void Quit();

public:
  uint32_t width, height;
  // glfw 窗口
  GLFWwindow *window = nullptr;
  // vk 实例
  vk::Instance instance;
  // 选用的显卡
  vk::PhysicalDevice phyDevice = VK_NULL_HANDLE;
  // 虚拟的逻辑显卡
  vk::Device device;
  // 窗口
  vk::SurfaceKHR surface;
  // 图形的队列
  QueueFamilyIndices queueFamilyIndices;
  vk::Queue graphicQueue;
  vk::Queue presentQueue;
  // 交换链
  std::unique_ptr<Swapchain> swapchain;
  // shader
  std::unique_ptr<Shader> shader;
  // commandManger
  std::unique_ptr<CommandManager> commandManager;
  // pipeline
  std::unique_ptr<RenderProcess> renderProcess;
  // renderer
  std::unique_ptr<Renderer> renderer;
  // // 采样器
  // vk::Sampler sampler;

public:
  void run();

  // 禁止复制构造函数和赋值运算符
  Application(const Application &) = delete;
  auto operator=(const Application &)
      -> Application & = delete;

private:
  void initwindow();
  void initVulkan();
  void mainLoop();
  void cleanup();

  // 创建实例
  void createInstance();
  void createSurface();
  void pickPhysicalDevice();
  void createDevice();
  void getGQueue();
  void createSwapchain();
  void createShaderModules();
  void createRenderProcess();
  void createGraphicsPipeline();
  void createCommandManager();
  void createRenderer();

  // 输出一些信息
  void showPropInfo();
  void queryQueueFamilyIndices();
  auto checkDeviceExtensionSupport() -> bool;
};
} // namespace app
