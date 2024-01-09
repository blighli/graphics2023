#include "../header/swapchain.h"
#include "../header/application.h"

namespace app {

Swapchain::Swapchain(uint32_t width, uint32_t height) {
  queryInfo(width, height);

  vk::SwapchainCreateInfoKHR createInfo;
  createInfo.setClipped(true)
      .setImageArrayLayers(1)
      .setImageUsage(
          vk::ImageUsageFlagBits::eColorAttachment)
      .setCompositeAlpha(
          vk::CompositeAlphaFlagBitsKHR::eOpaque)
      .setSurface(Application::GetInstance().surface)
      .setImageColorSpace(info.format.colorSpace)
      .setImageFormat(info.format.format)
      .setImageExtent(info.imageExtent)
      .setMinImageCount(info.imageCount)
      .setPreTransform(info.transform)
      .setPresentMode(info.present);

  auto &queueIndicecs =
      Application::GetInstance().queueFamilyIndices;
  if (queueIndicecs.graphicQueue.value() ==
      queueIndicecs.presentQueue.value()) {
    createInfo.setImageSharingMode(
        vk::SharingMode::eExclusive);
  } else {
    std::array indices = {
        queueIndicecs.graphicQueue.value(),
        queueIndicecs.presentQueue.value()};
    createInfo.setQueueFamilyIndices(indices)
        .setImageSharingMode(vk::SharingMode::eConcurrent);
  }

  swapchain =
      Application::GetInstance().device.createSwapchainKHR(
          createInfo);
  getImages();
  createImageViews();
}

Swapchain::~Swapchain() {
  for (auto &framebuffer : framebuffers) {
    Application::GetInstance().device.destroyFramebuffer(
        framebuffer);
  }
  for (auto &view : imageViews) {
    Application::GetInstance().device.destroyImageView(
        view);
  }
  Application::GetInstance().device.destroySwapchainKHR(
      swapchain);
}

void Swapchain::queryInfo(uint32_t width, uint32_t height) {
  auto &phyDevice = Application::GetInstance().phyDevice;
  auto &surface = Application::GetInstance().surface;
  auto formats = phyDevice.getSurfaceFormatsKHR(surface);
  info.format = formats[0];
  for (const auto &format : formats) {
    if (format.format == vk::Format::eB8G8R8A8Sint &&
        format.colorSpace ==
            vk::ColorSpaceKHR::eSrgbNonlinear) {
      info.format = format;
      break;
    }
  }

  auto capabilities =
      phyDevice.getSurfaceCapabilitiesKHR(surface);
  info.imageCount =
      std::clamp<uint32_t>(2, capabilities.minImageCount,
          capabilities.maxImageCount);

  info.imageExtent.width = std::clamp<uint32_t>(width,
      capabilities.minImageExtent.width,
      capabilities.minImageExtent.width);
  info.imageExtent.height = std::clamp<uint32_t>(height,
      capabilities.minImageExtent.height,
      capabilities.minImageExtent.height);

  info.transform = capabilities.currentTransform;

  auto presents =
      phyDevice.getSurfacePresentModesKHR(surface);
  info.present = vk::PresentModeKHR::eFifo;
  for (const auto &present : presents) {
    if (present == vk::PresentModeKHR::eMailbox) {
      info.present = present;
      break;
    }
  }
}

void Swapchain::getImages() {
  images = Application::GetInstance()
               .device.getSwapchainImagesKHR(swapchain);
}

void Swapchain::createImageViews() {
  imageViews.resize(images.size());

  for (int i = 0; i < images.size(); i++) {
    vk::ImageViewCreateInfo createInfo;
    vk::ComponentMapping mapping;
    vk::ImageSubresourceRange range;
    range.setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1)
        .setAspectMask(vk::ImageAspectFlagBits::eColor);
    createInfo.setImage(images[i])
        .setViewType(vk::ImageViewType::e2D)
        .setComponents(mapping)
        .setFormat(info.format.format)
        .setSubresourceRange(range);
    imageViews[i] =
        Application::GetInstance().device.createImageView(
            createInfo);
  }
}

void Swapchain::createFrameBuffers() {
  auto &width = info.imageExtent.width,
       &height = info.imageExtent.height;
  framebuffers.resize(images.size());
  for (size_t i = 0; i < images.size(); ++i) {
    vk::FramebufferCreateInfo createInfo;
    // 需要 renderProcess
    createInfo.setAttachments(imageViews[i])
        .setWidth(width)
        .setHeight(height)
        .setRenderPass(Application::GetInstance()
                           .renderProcess->renderPass)
        .setLayers(1);
    framebuffers[i] =
        Application::GetInstance().device.createFramebuffer(
            createInfo);
  }
}

} // namespace app