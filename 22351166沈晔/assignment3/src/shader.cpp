#include "../header/application.h"
#include "../header/shader.h"
#include "../header/math.h"
#include "glm/fwd.hpp"

namespace app {

Shader::Shader(const std::string &vertexSource,
    const std::string &fragSource) {

  vk::ShaderModuleCreateInfo createInfo1;
  createInfo1.setCodeSize(vertexSource.size())
      .setPCode(reinterpret_cast<const uint32_t *>(
          vertexSource.data()));

  vertexModule =
      Application::GetInstance().device.createShaderModule(
          createInfo1);

  vk::ShaderModuleCreateInfo createInfo2;
  createInfo2.setCodeSize(fragSource.size())
      .setPCode(reinterpret_cast<const uint32_t *>(
          fragSource.data()));
  fragmentModule =
      Application::GetInstance().device.createShaderModule(
          createInfo2);

  initDescriptorSetLayouts();
}

Shader::~Shader() {
  auto &device = Application::GetInstance().device;
  for (auto &layout : layouts) {
    device.destroyDescriptorSetLayout(layout);
  }
  device.destroyShaderModule(vertexModule);
  device.destroyShaderModule(fragmentModule);
}

void Shader::initDescriptorSetLayouts() {

  //   vk::DescriptorSetLayoutCreateInfo vertCreateInfo;
  //   vk::DescriptorSetLayoutBinding vertBinding;
  //   vertBinding
  //       .setBinding(0)
  //       .setDescriptorCount(1)
  //       .setDescriptorType(vk::DescriptorType::eUniformBuffer)
  //       .setStageFlags(vk::ShaderStageFlagBits::eVertex);
  //   vertCreateInfo.setBindings(vertBinding);

  //   layouts.push_back(
  //       Application::GetInstance()
  //           .device.createDescriptorSetLayout(vertCreateInfo));
  //   // 图像的描述符
  //   vk::DescriptorSetLayoutCreateInfo fragCreateInfo;
  //   vk::DescriptorSetLayoutBinding fragBinding;
  //   fragBinding
  //       .setBinding(1)
  //       .setDescriptorCount(1)
  //       .setDescriptorType(
  //           vk::DescriptorType::eCombinedImageSampler)
  //       .setStageFlags(vk::ShaderStageFlagBits::eFragment);
  //   fragCreateInfo.setBindings(fragBinding);

  //   layouts.push_back(
  //       Application::GetInstance()
  //           .device.createDescriptorSetLayout(fragCreateInfo));

  vk::DescriptorSetLayoutCreateInfo CreateInfo;
  std::array<vk::DescriptorSetLayoutBinding, 2> Binding;
  Binding[0]
      .setBinding(0)
      .setDescriptorCount(1)
      .setDescriptorType(vk::DescriptorType::eUniformBuffer)
      .setStageFlags(vk::ShaderStageFlagBits::eVertex);

  Binding[1]
      .setBinding(1)
      .setDescriptorCount(1)
      .setDescriptorType(
          vk::DescriptorType::eCombinedImageSampler)
      .setStageFlags(vk::ShaderStageFlagBits::eFragment);
  CreateInfo.setBindings(Binding);
  layouts.push_back(
      Application::GetInstance()
          .device.createDescriptorSetLayout(CreateInfo));
}

auto Shader::GetPushConstantRange() const
    -> std::vector<vk::PushConstantRange> {
  std::vector<vk::PushConstantRange> ranges(2);
  ranges[0]
      .setOffset(0)
      .setSize(sizeof(glm::mat4))
      .setStageFlags(vk::ShaderStageFlagBits::eVertex);
  ranges[1]
      .setOffset(sizeof(glm::mat4))
      .setSize(sizeof(glm::vec3))
      .setStageFlags(vk::ShaderStageFlagBits::eFragment);
  return ranges;
}

} // namespace app