#pragma once

#include <vulkan/vulkan.hpp>


namespace app {

class DeepTest {
public:
  vk::Image depthImage;
  vk::DeviceMemory depthImageMemory;
  vk::ImageView view;

  void createDepthResources();
};

}