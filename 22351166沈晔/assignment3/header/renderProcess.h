#pragma once

#include <vulkan/vulkan.hpp>
#include "shader.h"

namespace app {
class RenderProcess {
public:
  vk::RenderPass renderPass;
  vk::PipelineLayout layout;
  vk::Pipeline graphicsPipeline;

  RenderProcess();
  ~RenderProcess();

  void RecreateGraphicsPipeline(const Shader &shader);
  void RecreateRenderPass();

private:
  auto createLayout() -> vk::PipelineLayout;
  auto createGraphicsPipeline(const Shader &shader)
      -> vk::Pipeline;
  auto createRenderPass() -> vk::RenderPass;
  // void InitRenderPass();
  // void InitLayout();
  // void createGraphicsPipeline(const Shader& shader);
};

} // namespace app