#pragma once

#include <nvh/geometry.hpp>
#include <nvmath/nvmath_glsltypes.h>
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

using Allocator = nvvk::ResourceAllocatorVma;

constexpr uint32_t renderWidth = 1024;
constexpr uint32_t renderHeight = 1024;

struct PushConstants
{
	nvmath::mat4 matrixVP;
};

alignas(16) struct VertexAttribute
{
	VertexAttribute(const nvh::geometry::Vertex &vertex)
	{
		for (int i = 0; i < 3; i++)
		{
			position[i] = vertex.position[i];
			normal[i] = vertex.normal[i];
		}
		for (int i = 0; i < 2; i++)
			texCoord[i] = vertex.texcoord[i];
	}

	nvmath::vec3 position;
	nvmath::vec3 normal;
	nvmath::vec2 texCoord;
};

class Application : public nvvkhl::AppBaseVk
{
public:
	void setup(const VkInstance &instance, const VkPhysicalDevice &physicalDevice, const VkDevice &device, const uint32_t queueFamilyIndex);
	void createRenderer();

	void render(const VkCommandBuffer &cmdBuffer, nvvk::ProfilerVK &profiler);
	void renderGUI(nvvk::ProfilerVK &profiler);

	void updateBuffers(const VkCommandBuffer &cmdBuffer);

	void destroyResources();

private:
	void createBuffers();
	void createTextures();
	void createDescriptors();
	void createPipeline();

	bool guiProfilerMeasures(nvvk::ProfilerVK &profiler);

	Allocator m_allocator;

	std::size_t m_vertexCount{};
	nvvk::Buffer m_vertexBuffer{};
	nvvk::Buffer m_indexBuffer{};
	nvvk::Buffer m_modelMatrixBuffer{};
	nvvk::Texture m_earthTexture{};
	nvvk::Texture m_moonTexture{};
	nvvk::Texture m_sunTexture{};

	VkDescriptorPool m_descPool{VK_NULL_HANDLE};
	VkDescriptorSetLayout m_descLayout{VK_NULL_HANDLE};
	VkDescriptorSet m_descSet{VK_NULL_HANDLE};
	nvvk::DescriptorSetBindings m_bindings;

	VkPipeline m_pipeline{VK_NULL_HANDLE};
	VkPipelineLayout m_pipelineLayout{VK_NULL_HANDLE};

	PushConstants m_pushConstants{};
};