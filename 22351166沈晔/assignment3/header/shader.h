#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

namespace app {

class Shader {
public:
  vk::ShaderModule vertexModule;
  vk::ShaderModule fragmentModule;
  std::vector<vk::DescriptorSetLayout> layouts;

  Shader(const std::string &vertexSource,
      const std::string &fragSource);
  ~Shader();

  static void Quit();
  [[nodiscard]] auto GetPushConstantRange() const
      -> std::vector<vk::PushConstantRange>;

private:
  void initDescriptorSetLayouts();
};

} // namespace app