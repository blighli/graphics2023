#include <cstddef>
#include <cstdint>
#include <memory>

#include <memory>
#include <stdexcept>

#include "../header/renderer.h"
#include "../header/Application.h"
#include "vertex.cpp"
#include "vulkan/vulkan_structs.hpp"

namespace app {

Renderer::Renderer(int maxFlightCount)
    : maxFlightCount(maxFlightCount), curFrame(0) {
  createFences();
  createSemaphores();
  createCmdBuffers();
  loadModel();
  createBuffers();
  bufferData();
  DescriptorSetManager::Init(maxFlightCount);
  createSampler();
  createTexture();
  descriptorSets =
      DescriptorSetManager::Instance().AllocBufferSets(
          maxFlightCount);
  updateDescriptorSets();
}

Renderer::~Renderer() {
  auto &device = Application::GetInstance().device;
  device.destroySampler(sampler);
  texture.reset();
  DescriptorSetManager::Quit();
  hostIndexsBuffer.reset();
  deviceIndexsBuffer.reset();
  hostVertexBuffer.reset();
  deviceVertexBuffer.reset();
  for (auto &sem : imageAvaliableSems) {
    device.destroySemaphore(sem);
  }
  for (auto &sem : renderFinishSems) {
    device.destroySemaphore(sem);
  }
  for (auto &fence : fences) {
    device.destroyFence(fence);
  }
}

void Renderer::Render() {
  auto &device = Application::GetInstance().device;
  auto &swapchain = Application::GetInstance().swapchain;
  auto &renderProcess =
      Application::GetInstance().renderProcess;
  auto &cmdMgr = Application::GetInstance().commandManager;

  // 等待第一个 fence
  if (device.waitForFences(fences[curFrame], true,
          std::numeric_limits<std::uint64_t>::max()) !=
      vk::Result::eSuccess) {
    throw std::runtime_error("wait for fence failed");
  }
  device.resetFences(fences[curFrame]);

  auto acqResult =
      device.acquireNextImageKHR(swapchain->swapchain,
          std::numeric_limits<uint64_t>::max(),
          imageAvaliableSems[curFrame]);

  if (acqResult.result != vk::Result::eSuccess) {
    throw std::runtime_error(
        "device.acquireNextImageKHR failed!!!");
  }
  // 获得需要写入的图像的下标
  auto imageIndex = acqResult.value;

  cmdBufs[curFrame].reset();
  // 更新 MVP
  updateUniformBuffer(curFrame);
  // begin
  vk::CommandBufferBeginInfo beginInfo;
  beginInfo.setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  cmdBufs[curFrame].begin(beginInfo);
  {
    vk::RenderPassBeginInfo renderPassBegin;
    vk::Rect2D area;
    area.setOffset({0, 0}).setExtent(
        swapchain->info.imageExtent);
    vk::ClearValue clearValue;
    clearValue.setColor(
        vk::ClearColorValue(0.1f, 0.1f, 0.1f, 1.0f));
    renderPassBegin.setRenderPass(renderProcess->renderPass)
        .setRenderArea(area)
        .setFramebuffer(swapchain->framebuffers[imageIndex])
        .setClearValues(clearValue);

    cmdBufs[curFrame].beginRenderPass(
        renderPassBegin, vk::SubpassContents::eInline);
    {
      cmdBufs[curFrame].bindPipeline(
          vk::PipelineBindPoint::eGraphics,
          renderProcess->graphicsPipeline);
      // vertex count, prim, first idx,
      vk::DeviceSize offset = 0;
      cmdBufs[curFrame].bindVertexBuffers(
          0, deviceVertexBuffer->buffer, offset);
      cmdBufs[curFrame].bindIndexBuffer(
          deviceIndexsBuffer->buffer, 0,
          vk::IndexType::eUint32);

      if (curFrame > descriptorSets.size()) {
        std::cerr << "Error : descriptorSets outflow!"
                  << "\n";
        throw std::runtime_error("descriptorSets outflow!");
      }
      cmdBufs[curFrame].bindDescriptorSets(
          vk::PipelineBindPoint::eGraphics,
          renderProcess->layout, 0,
          {descriptorSets[curFrame].set}, {});

      cmdBufs[curFrame].drawIndexed(
          deviceIndexsBuffer->size / sizeof(uint32_t), 1, 0,
          0, 0);
    }
    cmdBufs[curFrame].endRenderPass();
  }
  cmdBufs[curFrame].end();
  vk::PipelineStageFlags waitDstStageMask =
      vk::PipelineStageFlagBits::eColorAttachmentOutput;

  vk::SubmitInfo submit;
  submit.setWaitDstStageMask(waitDstStageMask)
      .setWaitSemaphores(imageAvaliableSems[curFrame])
      .setSignalSemaphores(renderFinishSems[curFrame])
      .setCommandBuffers(cmdBufs[curFrame]);

  Application::GetInstance().graphicQueue.submit(
      submit, fences[curFrame]);

  vk::PresentInfoKHR present;
  present.setWaitSemaphores(renderFinishSems[curFrame])
      .setImageIndices(imageIndex)
      .setSwapchains(swapchain->swapchain);

  auto result =
      Application::GetInstance().presentQueue.presentKHR(
          present);

  if (result != vk::Result::eSuccess) {
    throw std::runtime_error("image present failed!!!");
  }

  curFrame = (curFrame + 1) % maxFlightCount;

  // CPU GPU 同步

  //   auto waitResult =
  //   device.waitForFences(fences[curFrame],
  //       true, std::numeric_limits<uint64_t>::max());
  //   if (waitResult != vk::Result::eSuccess) {
  //     throw std::runtime_error("wait for fence
  //     failed!!!");
  //   }
  //   device.resetFences(fences[curFrame]);
}
void Renderer::createFences() {
  fences.resize(maxFlightCount, nullptr);

  for (auto &fence : fences) {
    vk::FenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.setFlags(
        vk::FenceCreateFlagBits::eSignaled);
    fence = Application::GetInstance().device.createFence(
        fenceCreateInfo);
  }
}

void Renderer::createSemaphores() {
  auto &device = Application::GetInstance().device;
  vk::SemaphoreCreateInfo info;

  imageAvaliableSems.resize(maxFlightCount);
  renderFinishSems.resize(maxFlightCount);

  for (auto &sem : imageAvaliableSems) {
    sem = device.createSemaphore(info);
  }

  for (auto &sem : renderFinishSems) {
    sem = device.createSemaphore(info);
  }
}

void Renderer::createCmdBuffers() {
  cmdBufs.resize(maxFlightCount);

  for (auto &cmd : cmdBufs) {
    cmd = Application::GetInstance()
              .commandManager->CreateOneCommandBuffer();
  }
}

void Renderer::loadModel() {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn,
          &err, MODEL_PATH.c_str())) {
    throw std::runtime_error(warn + err);
  }

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex vertex{};

      vertex.pos = {
          attrib.vertices[3 * index.vertex_index + 0],
          attrib.vertices[3 * index.vertex_index + 1],
          attrib.vertices[3 * index.vertex_index + 2]};

      vertex.texCoord = {
          attrib.texcoords[2 * index.texcoord_index + 0],
          1.0f - attrib.texcoords[2 * index.texcoord_index +
                                  1]};

      vertex.color = {1.0f, 1.0f, 1.0f};

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] =
            static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }

      indices.push_back(uniqueVertices[vertex]);
    }
  }
}

void Renderer::createBuffers() {
  hostVertexBuffer = std::make_unique<BufferPkg>(
      sizeof(vertices[0]) * vertices.size(),
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible |
          vk::MemoryPropertyFlagBits::eHostCoherent);
  deviceVertexBuffer = std::make_unique<BufferPkg>(
      sizeof(vertices[0]) * vertices.size(),
      vk::BufferUsageFlagBits::eVertexBuffer |
          vk::BufferUsageFlagBits::eTransferDst,
      vk::MemoryPropertyFlagBits::eDeviceLocal);

  hostIndexsBuffer = std::make_unique<BufferPkg>(
      sizeof(indices[0]) * indices.size(),
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible |
          vk::MemoryPropertyFlagBits::eHostCoherent);
  deviceIndexsBuffer = std::make_unique<BufferPkg>(
      sizeof(indices[0]) * indices.size(),
      vk::BufferUsageFlagBits::eIndexBuffer |
          vk::BufferUsageFlagBits::eTransferDst,
      vk::MemoryPropertyFlagBits::eDeviceLocal);
}

void Renderer::bufferData() {
  memcpy(hostVertexBuffer->map, vertices.data(),
      sizeof(vertices[0]) * vertices.size());

  copyBuffer(hostVertexBuffer->buffer,
      deviceVertexBuffer->buffer, hostVertexBuffer->size, 0,
      0);

  memcpy(hostIndexsBuffer->map, indices.data(),
      sizeof(indices[0]) * indices.size());
  copyBuffer(hostIndexsBuffer->buffer,
      deviceIndexsBuffer->buffer, hostIndexsBuffer->size, 0,
      0);

  hostUniformBuffers.resize(maxFlightCount);
  size_t size = sizeof(float) * 4 * 4 * 3;
  for (auto &buffer : hostUniformBuffers) {
    buffer = std::make_unique<BufferPkg>(size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent);
  }
  deviceUniformBuffers.resize(maxFlightCount);
  for (auto &buffer : deviceUniformBuffers) {
    buffer = std::make_unique<BufferPkg>(size,
        vk::BufferUsageFlagBits::eTransferDst |
            vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal);
  }
  for (uint32_t i = 0; i < maxFlightCount; ++i) {
    copyBuffer(hostUniformBuffers[i]->buffer,
        deviceUniformBuffers[i]->buffer,
        hostUniformBuffers[i]->size, 0, 0);
  }
}

void Renderer::copyBuffer(vk::Buffer &src, vk::Buffer &dst,
    size_t size, size_t srcOffset, size_t dstOffset) {
  auto cmdBuf =
      Application::GetInstance()
          .commandManager->CreateOneCommandBuffer();

  vk::CommandBufferBeginInfo begin;
  begin.setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  cmdBuf.begin(begin);
  {
    vk::BufferCopy region;
    region.setSize(size)
        .setSrcOffset(srcOffset)
        .setDstOffset(dstOffset);
    cmdBuf.copyBuffer(src, dst, region);
  }
  cmdBuf.end();

  vk::SubmitInfo submit;
  submit.setCommandBuffers(cmdBuf);
  Application::GetInstance().graphicQueue.submit(submit);

  Application::GetInstance().device.waitIdle();

  Application::GetInstance().commandManager->FreeCmd(
      cmdBuf);
}

void Renderer::updateUniformBuffer(uint32_t currentImage) {
  auto &swapchainExtentInfo =
      Application::GetInstance()
          .swapchain->info.imageExtent;
  static auto startTime =
      std::chrono::high_resolution_clock::now();
  auto currentTime =
      std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float,
      std::chrono::seconds::period>(currentTime - startTime)
                   .count();
  MVP ubo;
  ubo.model = glm::rotate(glm::mat4(1.0f),
      time * glm::radians(90.0f),
      glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.project = glm::perspective(glm::radians(45.0f),
      swapchainExtentInfo.width /
          (float)swapchainExtentInfo.height,
      0.1f, 10.0f);
  ubo.project[1][1] *= -1;
  memcpy(hostUniformBuffers[currentImage]->map, &ubo,
      sizeof(ubo));
  copyBuffer(hostUniformBuffers[currentImage]->buffer,
      deviceUniformBuffers[currentImage]->buffer,
      hostUniformBuffers[currentImage]->size, 0, 0);
}

// bind uniform
void Renderer::updateDescriptorSets() {
  for (size_t i = 0; i < descriptorSets.size(); i++) {
    // bind MVP buffer
    vk::DescriptorBufferInfo bufferInfo1;
    bufferInfo1.setBuffer(deviceUniformBuffers[i]->buffer)
        .setOffset(0)
        .setRange(sizeof(MVP));

    // bind sampler
    vk::DescriptorImageInfo imageInfo;
    imageInfo
        .setImageLayout(
            vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(texture->view)
        .setSampler(sampler);

    std::array<vk::WriteDescriptorSet, 2> writeInfos;
    writeInfos[0]
        .setBufferInfo(bufferInfo1)
        .setDstBinding(0)
        .setDescriptorType(
            vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(1)
        .setDstArrayElement(0)
        .setDstSet(descriptorSets[i].set);

    writeInfos[1]
        .setImageInfo(imageInfo)
        .setDstBinding(1)
        .setDstArrayElement(0)
        .setDstSet(descriptorSets[i].set)
        .setDescriptorCount(1)
        .setDescriptorType(
            vk::DescriptorType::eCombinedImageSampler);

    Application::GetInstance().device.updateDescriptorSets(
        writeInfos, {});
  }
}

auto Renderer::createTexture() -> void {
  texture = std::make_unique<Texture>(
      TEXTURE_PATH, sampler);
}
auto Renderer::createSampler() -> void {
  vk::SamplerCreateInfo createInfo;
  createInfo.setMagFilter(vk::Filter::eLinear)
      .setMinFilter(vk::Filter::eLinear)
      .setAddressModeU(vk::SamplerAddressMode::eRepeat)
      .setAddressModeV(vk::SamplerAddressMode::eRepeat)
      .setAddressModeW(vk::SamplerAddressMode::eRepeat)
      .setAnisotropyEnable(false)
      .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
      .setUnnormalizedCoordinates(false)
      .setCompareEnable(false)
      .setMipmapMode(vk::SamplerMipmapMode::eLinear);
  sampler = Application::GetInstance().device.createSampler(
      createInfo);
}

} // namespace app
