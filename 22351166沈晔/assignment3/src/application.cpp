#include "../header/application.h"
#include <cstdint>
#include <memory>
#include <chrono>

namespace app {
// 实例
std::unique_ptr<Application> Application::instance_ =
    nullptr;
// 实例初始化
void Application::Init(uint32_t w, uint32_t h) {
  if (instance_ == nullptr) {
    instance_.reset(new Application(w, h));
    instance_->initwindow();
    instance_->initVulkan();
  } else {
    std::cout << "repeat create Application" << '\n';
  }
}
// 实例销毁
void Application::Quit() {
  instance_.reset();
}

void Application::run() {
  // showPropInfo();
  mainLoop();
  cleanup();
}
// 初始化 GLFW 窗口
void Application::initwindow() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(
      width, height, "Vulkan", nullptr, nullptr);
}
// vulkan 程序初始化
void Application::initVulkan() {
  // 创建实例
  createInstance();
  createSurface();
  pickPhysicalDevice();
  queryQueueFamilyIndices();
  createDevice();
  getGQueue();
  createSwapchain();
  createShaderModules();
  createRenderProcess();
  createGraphicsPipeline();
  createCommandManager();
  createRenderer();
}
// 窗口内的渲染循环
void Application::mainLoop() {
  auto startTime =
      std::chrono::high_resolution_clock::now();
  uint64_t frame = 0;
  while (!glfwWindowShouldClose(window)) {
    renderer->Render();
    glfwPollEvents();
    auto nowTime =
        std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::seconds>(
            nowTime - startTime);
    if (duration.count() >= 1) {
      std::cout << "FPS : " << frame << "\n";
      frame = 0;
      startTime = nowTime;
    }
    frame++;
  }
  device.waitIdle();
}
// 销毁（与创建顺序需要相反）
void Application::cleanup() {
  commandManager.reset();
  renderer.reset();
  renderProcess.reset();
  shader.reset();
  swapchain.reset();
  device.destroy();
  vkDestroySurfaceKHR(instance, surface, nullptr);
  instance.destroy();
  glfwDestroyWindow(window);
  glfwTerminate();
}
// 创建 vk 实例
void Application::createInstance() {

  vk::ApplicationInfo appInfo;
  appInfo.setApiVersion(VK_API_VERSION_1_3)
      .setPEngineName("Renderer");

  vk::InstanceCreateInfo createInfo;
  createInfo.setPApplicationInfo(&appInfo);

  uint32_t glfwExtensionCount = 0;
  vkEnumerateInstanceExtensionProperties(
      nullptr, &glfwExtensionCount, nullptr);

  auto glfwextensions = glfwGetRequiredInstanceExtensions(
      &glfwExtensionCount);

  createInfo.setEnabledExtensionCount(glfwExtensionCount)
      .setPpEnabledExtensionNames(glfwextensions);

  // valia layer
  createInfo.setEnabledLayerCount(1).setPEnabledLayerNames(
      validationLayers);

  instance = vk::createInstance(createInfo);
}
// 窗口表面创建
void Application::createSurface() {
  VkSurfaceKHR surfaceOld;
  if (glfwCreateWindowSurface(instance, window, nullptr,
          &surfaceOld) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to create window surface!");
  }
  surface = static_cast<vk::SurfaceKHR>(surfaceOld);
}
// 选取一个物理设备（找一个支持拓展的，一般选第一个）
void Application::pickPhysicalDevice() {
  auto devices = instance.enumeratePhysicalDevices();
  phyDevice = devices[0];
  if (!checkDeviceExtensionSupport()) {
    std::cerr << "Do not support swapchain!!!" << '\n';
    exit(-1);
  }
}
// 创建逻辑设备在次之前需要（寻找支持图形命令的队列）
void Application::createDevice() {
  vk::DeviceCreateInfo createInfo;

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
  float proprities = 1.0;
  if (queueFamilyIndices.presentQueue.value() ==
      queueFamilyIndices.graphicQueue.value()) {
    vk::DeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.setPQueuePriorities(&proprities)
        .setQueueCount(1)
        .setQueueFamilyIndex(
            queueFamilyIndices.graphicQueue.value());
    queueCreateInfos.push_back(queueCreateInfo);
  } else {
    vk::DeviceQueueCreateInfo queueCreateInfo1;
    queueCreateInfo1.setPQueuePriorities(&proprities)
        .setQueueCount(1)
        .setQueueFamilyIndex(
            queueFamilyIndices.graphicQueue.value());
    queueCreateInfos.push_back(queueCreateInfo1);
    vk::DeviceQueueCreateInfo queueCreateInfo2;
    queueCreateInfo2.setPQueuePriorities(&proprities)
        .setQueueCount(1)
        .setQueueFamilyIndex(
            queueFamilyIndices.presentQueue.value());
    queueCreateInfos.push_back(queueCreateInfo2);
  }
  createInfo.setPEnabledExtensionNames(deviceExtensions)
      .setQueueCreateInfos(queueCreateInfos);

  createInfo
      .setEnabledExtensionCount(
          static_cast<uint32_t>(deviceExtensions.size()))
      .setPpEnabledExtensionNames(deviceExtensions.data());
  device = phyDevice.createDevice(createInfo);
}
// 获得虚拟设备对应的两种队列
void Application::getGQueue() {
  graphicQueue = device.getQueue(
      queueFamilyIndices.graphicQueue.value(), 0);
  presentQueue = device.getQueue(
      queueFamilyIndices.presentQueue.value(), 0);
}
// 创建交换链
void Application::createSwapchain() {
  swapchain = std::make_unique<Swapchain>(width, height);
}
// 创建 shader
void Application::createShaderModules() {
  std::string vertexSource, fragSource;
  try {
    vertexSource = readSpvFile("spv/vert.spv");
    fragSource = readSpvFile("spv/frag.spv");
    if (vertexSource.size() == 0 ||
        fragSource.size() == 0) {
      throw std::runtime_error(
          "Shader file read failed!!!\n");
    } else {
      std::cout << "Read shader files Success!!!" << '\n';
    }
  } catch (const std::exception &e) {
    std::cerr << "SPV fail : " << e.what() << '\n';
    exit(-1);
  }
  shader =
      std::make_unique<Shader>(vertexSource, fragSource);
}
// 创建渲染流程
void Application::createRenderProcess() {
  renderProcess = std::make_unique<RenderProcess>();
}

void Application::createGraphicsPipeline() {
  swapchain->createFrameBuffers();
  renderProcess->RecreateGraphicsPipeline(*shader);
}

void Application::createCommandManager() {
  commandManager = std::make_unique<CommandManager>();
}
// 创建渲染器
void Application::createRenderer() {
  renderer = std::make_unique<Renderer>();
}

// show some GPU support
void Application::showPropInfo() {
  std::cout << "Layers ---------------------\n";
  auto layers = vk::enumerateInstanceLayerProperties();
  for (auto &layer : layers) {
    std::cout << layer.layerName << '\n';
  }
  std::cout << "Extensions ---------------------\n";
  auto extensions =
      vk::enumerateInstanceExtensionProperties();
  for (auto &extension : extensions) {
    std::cout << extension.extensionName << '\n';
  }
}

void Application::queryQueueFamilyIndices() {
  auto properties = phyDevice.getQueueFamilyProperties();
  for (int i = 0; i < properties.size(); ++i) {
    auto propertie = properties[i];
    if (propertie.queueFlags |
        vk::QueueFlagBits::eGraphics) {
      queueFamilyIndices.graphicQueue = i;
    }
    if (phyDevice.getSurfaceSupportKHR(i, surface)) {
      queueFamilyIndices.presentQueue = i;
    }
    if (queueFamilyIndices)
      break;
  }
}
// 检查物理设备是否支持拓展
auto Application::checkDeviceExtensionSupport() -> bool {
  auto availableExtensions =
      phyDevice.enumerateDeviceExtensionProperties();

  std::set<std::string> requiredExtensions(
      deviceExtensions.begin(), deviceExtensions.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }
  return requiredExtensions.empty();
}

} // namespace app