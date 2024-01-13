#pragma once

#include "vulkan/vulkan.hpp"

/*
    framebuffer - 帧缓冲
    attachment - 纹理附件 - Texture - vk::Image
    1 color Attachment - 1 + 颜色附件
    0 Stencil / Depth Attachment - 0 + 模板/深度附件

*/

namespace app {

class Swapchain {
public:
  vk::SwapchainKHR swapchain;

  Swapchain(uint32_t width, uint32_t height);
  ~Swapchain();

  struct SwapchainInfo {
    vk::Extent2D imageExtent;
    uint32_t imageCount;
    vk::SurfaceFormatKHR format;
    vk::SurfaceTransformFlagBitsKHR transform;
    vk::PresentModeKHR present;
  };

  SwapchainInfo info;
  std::vector<vk::Image> images;
  std::vector<vk::ImageView> imageViews;
  std::vector<vk::Framebuffer> framebuffers;

  void queryInfo(uint32_t width, uint32_t height);
  void getImages();
  void createImageViews();
  void createFrameBuffers();
};

} // namespace app