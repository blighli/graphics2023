#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <chrono>
#include <vulkan/vulkan.hpp>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "buffer.h"
#include "descriptorManager.h"
#include "vertex.h"
#include "texture.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "../external/tiny_obj_loader.h"

namespace app {


const std::string MODEL_PATH = "resources/viking_room.obj";
const std::string TEXTURE_PATH = "resources/viking_room.png";
std::vector<Vertex> vertices;
std::vector<uint32_t> indices;
VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferMemory;

class Renderer {
public:
  Renderer(int maxFlightCount = 2);
  ~Renderer();

  void Render();

private:
  int maxFlightCount;
  int curFrame;
  std::vector<vk::Fence> fences;
  std::vector<vk::Semaphore> imageAvaliableSems;
  std::vector<vk::Semaphore> renderFinishSems;
  std::vector<vk::CommandBuffer> cmdBufs;

  std::unique_ptr<BufferPkg> hostVertexBuffer;
  std::unique_ptr<BufferPkg> deviceVertexBuffer;

  std::unique_ptr<BufferPkg> hostIndexsBuffer;
  std::unique_ptr<BufferPkg> deviceIndexsBuffer;

  std::vector<std::unique_ptr<BufferPkg>>
      hostUniformBuffers;
  std::vector<std::unique_ptr<BufferPkg>>
      deviceUniformBuffers;

  glm::mat4 projectMat_;
  glm::mat4 viewMat_;

  std::vector<DescriptorSetManager::SetInfo> descriptorSets;

  std::unique_ptr<Texture> texture;
  vk::Sampler sampler;
  // std::unique_ptr<DescriptorSetManager>
  // descriptorManager;

  void createFences();
  void createSemaphores();
  void createCmdBuffers();
  void loadModel();
  void createBuffers();
  void bufferData();
  void copyBuffer(vk::Buffer &src, vk::Buffer &dst,
      size_t size, size_t srcOffset, size_t dstOffset);

  // void bufferMVPData(const glm::mat4& model);

  auto updateUniformBuffer(uint32_t curFrame) -> void;
  auto updateDescriptorSets() -> void;
  auto createTexture() -> void;
  auto createSampler() -> void;
  // auto createDescriptorPool(uint32_t maxFlightCount) ->
  // void; auto allocDescriptorSets(uint32_t maxFlightCount)
  // -> void;
};

} // namespace app