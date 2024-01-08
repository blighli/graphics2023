#pragma once

#include <nvvkhl/appbase_vk.hpp>
#include <nvvk/context_vk.hpp>
#include <nvvk/structs_vk.hpp>
#include <nvvk/memallocator_vma_vk.hpp>
#include <nvvk/descriptorsets_vk.hpp>
#include <nvvk/images_vk.hpp>
#include <nvvk/commands_vk.hpp>
#include <nvvk/shaders_vk.hpp>
#include <nvvk/pipeline_vk.hpp>
#include <nvvk/profiler_vk.hpp>

#include "scene.hpp"

using Allocator = nvvk::ResourceAllocatorVma;

constexpr uint32_t renderWidth = 1024;
constexpr uint32_t renderHeight = 768;

alignas(16) struct PushConstants
{
	nvmath::mat4 matrixModel;
	nvmath::mat4 matrixVP;
	nvmath::vec4 lightProperty{1.f, 1.f, 1.f, 1.f};
	uint32_t lightType{0};
};

class Application : public nvvkhl::AppBaseVk
{
public:
	void setup(const nvvk::Context &context);
	void createRenderer();

	void render(const VkCommandBuffer &cmdBuffer, nvvk::ProfilerVK &profiler);
	void finalBlit(const VkCommandBuffer &cmdBuffer, nvvk::ProfilerVK &profiler);
	void renderGUI(nvvk::ProfilerVK &profiler);

	void updateBuffers(const VkCommandBuffer &cmdBuffer);

	void destroyResources();

private:
	void onResize(int /*w*/, int /*h*/) override final;
	void onMouseMotion(int x, int y) override final;
	void onMouseButton(int button, int action, int mods) override final;
	void onMouseWheel(int delta) override final;
	void updateInputs() override final;

	void recreateRenderTarget();
	void createDescriptors();
	void createPipeline();

	bool guiProfilerMeasures(nvvk::ProfilerVK &profiler);

	nvvk::Context::Queue m_graphicsQueue{};
	nvvk::Context::Queue m_computeQueue{};
	nvvk::Context::Queue m_transferQueue{};

	Allocator m_allocator;

	nvvk::Texture m_visibilityBuffer{};
	nvvk::Texture m_depthBuffer{};
	VkSampler m_defaultBufferImageSampler{};
	nvvk::DescriptorSetContainer m_attachmentsContainer{};
	VkRenderingInfo m_dynamicRenderingInfo{VK_STRUCTURE_TYPE_RENDERING_INFO, nullptr, 0};
	std::array<VkRenderingAttachmentInfo, 1> m_dynamicColorAttachs{};
	std::array<VkRenderingAttachmentInfo, 1> m_dynamicDepthAttach{};

	VkPipelineLayout m_visibilityPipelineLayout{VK_NULL_HANDLE};
	VkPipeline m_visibilityPipeline{VK_NULL_HANDLE};
	VkPipelineLayout m_shadingPipelineLayout{VK_NULL_HANDLE};
	VkPipeline m_shadingPipeline{VK_NULL_HANDLE};

	PushConstants m_pushConstants{};

	// interactive
	int m_selectedObject{-1}; // -3 for light, -2 for camera, -1 for none, 0...max to model parts
	bool m_leftMouseButton{false};
	bool m_middleMouseButton{false};
	bool m_rightMouseButton{false};
};