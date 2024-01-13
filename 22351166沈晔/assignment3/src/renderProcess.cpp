#include "../header/renderProcess.h"
#include "../header/application.h"
#include "../header/vertex.h"

namespace app {

RenderProcess::RenderProcess() {
  layout = createLayout();
  renderPass = createRenderPass();
  graphicsPipeline = nullptr;
}

RenderProcess::~RenderProcess() {
  auto device = &Application::GetInstance().device;
  device->destroyRenderPass(renderPass);
  device->destroyPipelineLayout(layout);
  device->destroyPipeline(graphicsPipeline);
}

void RenderProcess::RecreateGraphicsPipeline(
    const Shader &shader) {
  if (graphicsPipeline) {
    Application::GetInstance().device.destroyPipeline(
        graphicsPipeline);
  }
  graphicsPipeline = createGraphicsPipeline(shader);
}

void RenderProcess::RecreateRenderPass() {
  if (renderPass) {
    Application::GetInstance().device.destroyRenderPass(
        renderPass);
  }
  renderPass = createRenderPass();
}

auto RenderProcess::createLayout() -> vk::PipelineLayout {
  vk::PipelineLayoutCreateInfo createInfo;
  auto layout = Application::GetInstance().shader->layouts;
  createInfo.setSetLayouts(layout);
  return Application::GetInstance()
      .device.createPipelineLayout(createInfo);
}

auto RenderProcess::createGraphicsPipeline(
    const Shader &shader) -> vk::Pipeline {
  auto &app = Application::GetInstance();
  vk::GraphicsPipelineCreateInfo createInfo;

  // 0. shader prepare
  std::array<vk::PipelineShaderStageCreateInfo, 2>
      stageCreateInfos;
  stageCreateInfos[0]
      .setModule(shader.vertexModule)
      .setPName("main")
      .setStage(vk::ShaderStageFlagBits::eVertex);
  stageCreateInfos[1]
      .setModule(shader.fragmentModule)
      .setPName("main")
      .setStage(vk::ShaderStageFlagBits::eFragment);

  // 1. Vertex input
  vk::PipelineVertexInputStateCreateInfo
      vertexInputCreateInfo;
  auto attribute = Vertex::GetAttribute();
  auto binding = Vertex::GetBinding();

  vertexInputCreateInfo
      .setVertexBindingDescriptions(binding)
      .setVertexAttributeDescriptions(attribute);

  // 2. Vertex Assembly
  vk::PipelineInputAssemblyStateCreateInfo inputAss;
  inputAss.setPrimitiveRestartEnable(false).setTopology(
      vk::PrimitiveTopology::eTriangleList);

  // 3. viewport & scissor
  vk::PipelineViewportStateCreateInfo viewportInfo;
  vk::Viewport viewport(0, 0,
      static_cast<float>(
          app.swapchain->info.imageExtent.width),
      static_cast<float>(
          app.swapchain->info.imageExtent.height),
      0, 1);
  vk::Rect2D scissor(
      {0, 0}, app.swapchain->info.imageExtent);
  viewportInfo.setViewports(viewport).setScissors(scissor);

  // 4. Rastrization
  vk::PipelineRasterizationStateCreateInfo rastInfo;
  // 非常要注意这里，图像不要被错误的剔除了
  rastInfo.setRasterizerDiscardEnable(false)
      .setCullMode(vk::CullModeFlagBits::eBack)
      .setFrontFace(vk::FrontFace::eCounterClockwise)
      .setPolygonMode(vk::PolygonMode::eFill)
      .setRasterizerDiscardEnable(false)
      .setLineWidth(1);

  // 5 .multi sample
  vk::PipelineMultisampleStateCreateInfo multiSample;
  multiSample.setSampleShadingEnable(false)
      .setRasterizationSamples(vk::SampleCountFlagBits::e1);

  // 6. depth test

  // 7. color blending

  vk::PipelineColorBlendStateCreateInfo blendInfo;
  vk::PipelineColorBlendAttachmentState attachs;
  attachs.setBlendEnable(false).setColorWriteMask(
      vk::ColorComponentFlagBits::eA |
      vk::ColorComponentFlagBits::eB |
      vk::ColorComponentFlagBits::eG |
      vk::ColorComponentFlagBits::eR);

  blendInfo.setLogicOpEnable(false).setAttachments(attachs);

  // renderpass and layout
  // createInfo.setRenderPass(renderPass).setLayout(layout);

  // create graphics pipeline
  createInfo.setStages(stageCreateInfos)
      .setPVertexInputState(&vertexInputCreateInfo)
      .setPInputAssemblyState(&inputAss)
      .setPViewportState(&viewportInfo)
      .setPRasterizationState(&rastInfo)
      .setPMultisampleState(&multiSample)
      .setPColorBlendState(&blendInfo)
      .setRenderPass(renderPass)
      .setLayout(layout);

  // 创建
  auto result = Application::GetInstance()
                    .device.createGraphicsPipeline(
                        nullptr, createInfo);
  if (result.result != vk::Result::eSuccess) {
    throw std::runtime_error(
        "create graphic pipeline failed");
  }
  return result.value;
}

auto RenderProcess::createRenderPass() -> vk::RenderPass {
  vk::RenderPassCreateInfo createInfo;

  vk::SubpassDependency dependency;
  dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
      .setDstSubpass(0)
      .setSrcStageMask(
          vk::PipelineStageFlagBits::eColorAttachmentOutput)
      .setDstStageMask(
          vk::PipelineStageFlagBits::eColorAttachmentOutput)
      .setDstAccessMask(
          vk::AccessFlagBits::eColorAttachmentWrite);

  vk::AttachmentDescription AttachDescription;

  AttachDescription
      .setFormat(Application::GetInstance()
                     .swapchain->info.format.format)
      .setInitialLayout(vk::ImageLayout::eUndefined)
      .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
      .setLoadOp(vk::AttachmentLoadOp::eClear)
      .setStoreOp(vk::AttachmentStoreOp::eStore)
      .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
      .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
      .setSamples(vk::SampleCountFlagBits::e1);

  vk::AttachmentReference reference;
  reference
      .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
      .setAttachment(0);

  vk::SubpassDescription subpass;
  subpass
      .setPipelineBindPoint(
          vk::PipelineBindPoint::eGraphics)
      .setColorAttachments(reference);

  createInfo.setSubpasses(subpass)
      .setAttachments(AttachDescription)
      .setDependencies(dependency);

  return Application::GetInstance().device.createRenderPass(
      createInfo);
}

} // namespace app
