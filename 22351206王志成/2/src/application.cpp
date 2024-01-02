#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <nvh/fileoperations.hpp>
#include <stb_image.h>

#include "application.h"

void Application::setup(const VkInstance &instance, const VkPhysicalDevice &physicalDevice, const VkDevice &device, const uint32_t queueFamilyIndex)
{
	AppBaseVk::setup(instance, device, physicalDevice, queueFamilyIndex);

	m_allocator.init(instance, device, physicalDevice);
}

void Application::createRenderer()
{
	createBuffers();
	createTextures();
	createDescriptors();
	createPipeline();
}

void Application::render(const VkCommandBuffer &cmdBuffer, nvvk::ProfilerVK &profiler)
{
	VkViewport viewport{0, 0, m_size.width, m_size.height, 0, 1};
	VkRect2D scissor{{0, 0}, m_size};

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
	vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
	vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descSet, 0, nullptr);
	vkCmdPushConstants(cmdBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstants), &m_pushConstants);

	VkDeviceSize offset{};
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &m_vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(cmdBuffer, m_indexBuffer.buffer, offset, VkIndexType::VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(cmdBuffer, m_vertexCount, 3, 0, 0, 0);
}

void Application::renderGUI(nvvk::ProfilerVK &profiler)
{

	// update title for every 1 sec
	static float dirtyTimer = .0f;

	dirtyTimer += ImGui::GetIO().DeltaTime;
	if (dirtyTimer > 1)
	{
		std::stringstream output;
		output << "implicit surface ray tracing";
		output << "\t|\t" << m_size.width << "x" << m_size.height;
		output << "\t|\t" << static_cast<int>(ImGui::GetIO().Framerate) << "FPS / " << std::setprecision(3) << 1000.F / ImGui::GetIO().Framerate << "ms";
		glfwSetWindowTitle(m_window, output.str().c_str());
		dirtyTimer = .0f;
	}

	// render GUI
	if (showGui())
	{
		ImGuiH::Control::style.ctrlPerc = .55f;
		ImGuiH::Panel::Begin(ImGuiH::Panel::Side::Left, 0.8f);

		if (ImGui::CollapsingHeader("Camera"))
			ImGuiH::CameraWidget();
		if (ImGui::CollapsingHeader("Stats"))
		{
			ImGuiH::Control::Group<bool>("Profiler Measure", false, [&]
										 { return guiProfilerMeasures(profiler); });
		}

		ImGui::End();
	}
}

void Application::updateBuffers(const VkCommandBuffer &cmdBuffer)
{
	// update CameraProperty (Push Constants)
	const auto &matView = CameraManip.getMatrix();
	const auto &matProj = nvmath::perspectiveVK(CameraManip.getFov(), (float)m_size.height / m_size.width, CameraManip.getClipPlanes().x, CameraManip.getClipPlanes().y);
	m_pushConstants.matrixVP = matProj * matView;

	// get time period
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float duration = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	// update model matrices
	nvmath::vec3 earthPos{10.f * std::cos(duration * nvmath::radians(15.f)), .0f, 10.f * std::sin(duration * nvmath::radians(15.f))};
	nvmath::vec3 moonPos{4.f * std::cos(duration * nvmath::radians(45.f) + 75.f), .0f, 4.f * std::sin(duration * nvmath::radians(45.f) + 75.f)};
	moonPos += earthPos;
	std::array<nvmath::mat4, 3> matModels{};
	matModels[0] = nvmath::translation_mat4(nvmath::vec3f_zero) * nvmath::rotation_mat4_y(duration * nvmath::radians(30.f)) * nvmath::rotation_mat4_x(nvmath::radians(90.f)) * nvmath::scale_mat4(nvmath::vec3f_one * 5.f);
	matModels[1] = nvmath::translation_mat4(earthPos) * nvmath::rotation_mat4_y(duration * nvmath::radians(30.f)) * nvmath::rotation_mat4_x(nvmath::radians(90.f)) * nvmath::scale_mat4(nvmath::vec3f_one * 2.5f);
	matModels[2] = nvmath::translation_mat4(moonPos) * nvmath::rotation_mat4_y(duration * nvmath::radians(30.f)) * nvmath::rotation_mat4_x(nvmath::radians(90.f)) * nvmath::scale_mat4(nvmath::vec3f_one * 1.f);
	{
		auto mapped = m_allocator.map(m_modelMatrixBuffer);
		memcpy(mapped, matModels.data(), sizeof(nvmath::mat4) * 3);
		m_allocator.unmap(m_modelMatrixBuffer);
	}
}

void Application::createPipeline()
{
	vkDestroyPipeline(m_device, m_pipeline, VK_NULL_HANDLE);
	vkDestroyPipelineLayout(m_device, m_pipelineLayout, VK_NULL_HANDLE);

	VkPushConstantRange pushConstantRange = {VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstants)};
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = nvvk::make<VkPipelineLayoutCreateInfo>();
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &m_descLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
	NVVK_CHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, VK_NULL_HANDLE, &m_pipelineLayout));

	nvvk::GraphicsPipelineGeneratorCombined graphicsPipelineHelper(m_device, m_pipelineLayout, m_renderPass);
	graphicsPipelineHelper.addShader(nvh::loadFile("builtin_resources/shaders/planet.vert.spv", true), VK_SHADER_STAGE_VERTEX_BIT);
	graphicsPipelineHelper.addShader(nvh::loadFile("builtin_resources/shaders/planet.frag.spv", true), VK_SHADER_STAGE_FRAGMENT_BIT);
	graphicsPipelineHelper.addBindingDescription(graphicsPipelineHelper.makeVertexInputBinding(0, sizeof(VertexAttribute)));
	graphicsPipelineHelper.addAttributeDescription(graphicsPipelineHelper.makeVertexInputAttribute(0, 0, VkFormat::VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexAttribute, position)));
	graphicsPipelineHelper.addAttributeDescription(graphicsPipelineHelper.makeVertexInputAttribute(1, 0, VkFormat::VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexAttribute, normal)));
	graphicsPipelineHelper.addAttributeDescription(graphicsPipelineHelper.makeVertexInputAttribute(2, 0, VkFormat::VK_FORMAT_R32G32_SFLOAT, offsetof(VertexAttribute, texCoord)));
	m_pipeline = graphicsPipelineHelper.createPipeline();
}

void Application::createBuffers()
{
	auto sphereGeometry = nvh::geometry::Sphere<VertexAttribute>(64, 64);
	m_vertexCount = sphereGeometry.getTriangleIndicesCount();

	m_vertexBuffer = m_allocator.createBuffer(sphereGeometry.getVerticesSize(), VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	m_indexBuffer = m_allocator.createBuffer(sphereGeometry.getTriangleIndicesSize(), VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	m_modelMatrixBuffer = m_allocator.createBuffer(sizeof(nvmath::mat4) * 3, VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	{
		nvvk::ScopeCommandBuffer scopedBuffer(m_device, m_graphicsQueueIndex, m_queue);

		m_allocator.getStaging()->cmdToBuffer(scopedBuffer, m_vertexBuffer.buffer, 0, sphereGeometry.getVerticesSize(), sphereGeometry.m_vertices.data());
		m_allocator.getStaging()->cmdToBuffer(scopedBuffer, m_indexBuffer.buffer, 0, sphereGeometry.getTriangleIndicesSize(), sphereGeometry.m_indicesTriangles.data());
	}
}

void Application::createTextures()
{
	int width, height, channel;
	auto earthTextureData = stbi_load("builtin_resources/textures/earth.png", &width, &height, &channel, 4);
	VkImageCreateInfo earthInfo = nvvk::makeImage2DCreateInfo({width, height}, VK_FORMAT_R8G8B8A8_SRGB);
	auto moonTextureData = stbi_load("builtin_resources/textures/moon.png", &width, &height, &channel, 4);
	VkImageCreateInfo moonInfo = nvvk::makeImage2DCreateInfo({width, height}, VK_FORMAT_R8G8B8A8_SRGB);
	auto sunTextureData = stbi_load("builtin_resources/textures/sun.png", &width, &height, &channel, 4);
	VkImageCreateInfo sunInfo = nvvk::makeImage2DCreateInfo({width, height}, VK_FORMAT_R8G8B8A8_SRGB);
	VkSamplerCreateInfo samplerInfo = nvvk::makeSamplerCreateInfo(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT);

	{
		nvvk::ScopeCommandBuffer scopedBuffer(m_device, m_graphicsQueueIndex, m_queue);

		m_earthTexture = m_allocator.createTexture(scopedBuffer, earthInfo.extent.width * earthInfo.extent.height * 4, earthTextureData, earthInfo, samplerInfo);
		m_moonTexture = m_allocator.createTexture(scopedBuffer, moonInfo.extent.width * moonInfo.extent.height * 4, moonTextureData, moonInfo, samplerInfo);
		m_sunTexture = m_allocator.createTexture(scopedBuffer, sunInfo.extent.width * sunInfo.extent.height * 4, sunTextureData, sunInfo, samplerInfo);
	}

	stbi_image_free(earthTextureData);
	stbi_image_free(moonTextureData);
	stbi_image_free(sunTextureData);
}

void Application::createDescriptors()
{
	nvvk::DescriptorSetBindings bindings;

	vkDestroyDescriptorPool(m_device, m_descPool, VK_NULL_HANDLE);
	vkDestroyDescriptorSetLayout(m_device, m_descLayout, VK_NULL_HANDLE);

	bindings.addBinding(0, VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	bindings.addBinding(1, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, &m_earthTexture.descriptor.sampler);
	bindings.addBinding(2, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, &m_moonTexture.descriptor.sampler);
	bindings.addBinding(3, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, &m_sunTexture.descriptor.sampler);

	m_descLayout = bindings.createLayout(m_device);
	m_descPool = bindings.createPool(m_device);
	m_descSet = nvvk::allocateDescriptorSet(m_device, m_descPool, m_descLayout);

	std::vector<VkWriteDescriptorSet> writeDescSets;
	VkDescriptorBufferInfo modelBufferInfo{m_modelMatrixBuffer.buffer, 0, VK_WHOLE_SIZE};
	writeDescSets.push_back(bindings.makeWrite(m_descSet, 0, &modelBufferInfo));
	writeDescSets.push_back(bindings.makeWrite(m_descSet, 1, &m_earthTexture.descriptor));
	writeDescSets.push_back(bindings.makeWrite(m_descSet, 2, &m_moonTexture.descriptor));
	writeDescSets.push_back(bindings.makeWrite(m_descSet, 3, &m_sunTexture.descriptor));

	vkUpdateDescriptorSets(m_device, writeDescSets.size(), writeDescSets.data(), 0, nullptr);
}

bool Application::guiProfilerMeasures(nvvk::ProfilerVK &profiler)
{
	struct Info
	{
		nvmath::vec2f statRender{0.0f, 0.0f};
		float frameTime{0.0f};
	};
	static Info display;
	static Info collect;

	static float dirtyCount = 1.f;
	{
		dirtyCount++;
		nvvk::ProfilerVK::TimerInfo info;
		profiler.getTimerInfo("rendering", info);
		collect.statRender.x += float(info.gpu.average / 1000.f);
		collect.statRender.y += float(info.cpu.average / 1000.f);
		collect.frameTime += 1000.0f / ImGui::GetIO().Framerate;
	}

	static float dirtyTimer = 1.f;
	dirtyTimer += ImGui::GetIO().DeltaTime;
	if (dirtyTimer >= .5f)
	{
		display.statRender = collect.statRender / dirtyCount;
		display.frameTime = collect.frameTime / dirtyCount;
		dirtyCount = .0f;
		dirtyTimer = .0f;
		collect = Info{};
	}

	ImGui::Text("Frame time: %.3f[ms]", display.frameTime);
	ImGui::Text("Rendering time(GPU/CPU): %.3f / %.3f[ms]", display.statRender.x, display.statRender.y);
	ImGui::ProgressBar(display.statRender.x / display.frameTime);
	ImGui::Spacing();
	ImGui::TextWrapped("Current average rendering time %.3f ms / %.1F FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	return false;
}

void Application::destroyResources()
{
	m_allocator.destroy(m_earthTexture);
	m_allocator.destroy(m_moonTexture);
	m_allocator.destroy(m_sunTexture);
	m_allocator.destroy(m_vertexBuffer);
	m_allocator.destroy(m_indexBuffer);
	m_allocator.destroy(m_modelMatrixBuffer);

	vkDestroyPipeline(m_device, m_pipeline, VK_NULL_HANDLE);
	vkDestroyPipelineLayout(m_device, m_pipelineLayout, VK_NULL_HANDLE);

	vkDestroyDescriptorPool(m_device, m_descPool, VK_NULL_HANDLE);
	vkDestroyDescriptorSetLayout(m_device, m_descLayout, VK_NULL_HANDLE);

	m_allocator.deinit();
}
