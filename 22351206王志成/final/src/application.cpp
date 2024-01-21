#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <nvh/fileoperations.hpp>
#include <stb_image.h>

#include "modelLoader.h"
#include "application.h"

void Application::setup(const nvvk::Context &context)
{
	AppBaseVk::setup(context.m_instance, context.m_device, context.m_physicalDevice, context.m_queueGCT);
	m_graphicsQueue = context.m_queueGCT;
	m_computeQueue = context.m_queueC;
	m_transferQueue = context.m_queueT;

	m_allocator.init(context.m_instance, context.m_device, context.m_physicalDevice);

	m_attachmentsContainer.init(m_device);

	m_dynamicColorAttachs.fill({VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO, nullptr});
	m_dynamicDepthAttach.fill({VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO, nullptr});
	VkClearValue colorClear{.0f, .0f, .0f, .0f};
	VkClearValue depthClear{1.f, 0};
	m_dynamicColorAttachs[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	m_dynamicColorAttachs[0].resolveMode = VK_RESOLVE_MODE_NONE;
	m_dynamicColorAttachs[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	m_dynamicColorAttachs[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	m_dynamicColorAttachs[0].clearValue = colorClear;
	m_dynamicDepthAttach[0].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	m_dynamicDepthAttach[0].resolveMode = VK_RESOLVE_MODE_NONE;
	m_dynamicDepthAttach[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	m_dynamicDepthAttach[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	m_dynamicDepthAttach[0].clearValue = depthClear;

	Scene::getInstance().m_deviceHandle = m_device;
	Scene::getInstance().m_allocatorHandle.init(context.m_instance, context.m_device, context.m_physicalDevice);
	Scene::getInstance().m_transferQueueFamilyIndex = m_transferQueue.familyIndex;
	Scene::getInstance().m_transferQueue = m_transferQueue.queue;
	Scene::getInstance().addObjectGroup({});
	ModelLoader::getInstance().load(Scene::getInstance().m_objects.back(), Scene::getInstance().m_materials, Scene::getInstance().m_textures,
									"builtin_resources/models/cgaxis_107_11_cafe_stall_obj.obj", "builtin_resources/textures", "", false);

	printf("Init done.\n");
}

void Application::createRenderer()
{
	// initialize
	Scene::getInstance().prepareToDraw();
	createDescriptors();
	recreateRenderTarget();
	createPipeline();
}

void Application::render(const VkCommandBuffer &cmdBuffer, nvvk::ProfilerVK &profiler)
{
	Scene::getInstance().prepareToDraw();
	nvvk::cmdBarrierImageLayout(cmdBuffer, m_visibilityBuffer.image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	nvvk::cmdBarrierImageLayout(cmdBuffer, m_depthBuffer.image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
	m_visibilityBuffer.descriptor.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	m_depthBuffer.descriptor.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkViewport viewport{0, 0, m_size.width, m_size.height, 0, 1};
	VkRect2D scissor{{0, 0}, m_size};

	vkCmdBeginRendering(cmdBuffer, &m_dynamicRenderingInfo);

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_visibilityPipeline);
	vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
	vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

	vkCmdPushConstants(cmdBuffer, m_visibilityPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstants), &m_pushConstants);

	VkDeviceSize offset{};
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &Scene::getInstance().m_vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(cmdBuffer, Scene::getInstance().m_indexBuffer.buffer, offset, VkIndexType::VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(cmdBuffer, Scene::getInstance().m_totalVertexCount, 3, 0, 0, 0);

	vkCmdEndRendering(cmdBuffer);

	nvvk::cmdBarrierImageLayout(cmdBuffer, m_visibilityBuffer.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	nvvk::cmdBarrierImageLayout(cmdBuffer, m_depthBuffer.image, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
	m_visibilityBuffer.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	m_depthBuffer.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void Application::finalBlit(const VkCommandBuffer &cmdBuffer, nvvk::ProfilerVK &profiler)
{
	VkViewport viewport{0, 0, m_size.width, m_size.height, 0, 1};
	VkRect2D scissor{{0, 0}, m_size};

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadingPipeline);
	vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
	vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

	std::array<VkDescriptorSet, 3> mergedSets{Scene::getInstance().m_geometrySet, m_attachmentsContainer.getSet(), Scene::getInstance().m_textureSet};
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadingPipelineLayout, 0, mergedSets.size(), mergedSets.data(), 0, nullptr);
	vkCmdPushConstants(cmdBuffer, m_shadingPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstants), &m_pushConstants);
	vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
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
		setTitle(output.str());
		dirtyTimer = .0f;
	}

	// render GUI
	if (showGui())
	{
		ImGuiH::Control::style.ctrlPerc = .55f;

		ImGuiH::Panel::Begin(ImGuiH::Panel::Side::Left, 0.8f);
		if (ImGui::BeginTabBar("LeftGUIs"))
		{
			if (ImGui::BeginTabItem("Scene"))
			{
				auto hierarchy = Scene::getInstance().getSceneHierarchy();

				for (const auto &iter : hierarchy)
				{
					ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen;
					if (iter.second.empty())
						flag |= ImGuiTreeNodeFlags_Leaf;
					if (ImGui::TreeNodeEx(iter.first, flag))
					{
						auto index = 0ULL;
						for (const auto &objectLabel : iter.second)
						{
							flag = ImGuiTreeNodeFlags_Leaf;
							if (index == m_selectedObject)
								flag |= ImGuiTreeNodeFlags_Selected;
							if (ImGui::TreeNodeEx(objectLabel, flag))
							{
								if (ImGui::IsItemClicked())
									m_selectedObject = index;
								ImGui::TreePop();
							}
							index++;
						}

						ImGui::TreePop();
					}
				}

				ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_Leaf;
				if (m_selectedObject == -2)
					flag |= ImGuiTreeNodeFlags_Selected;
				if (ImGui::TreeNodeEx("main camera", flag))
				{
					if (ImGui::IsItemClicked())
						m_selectedObject = -2;
					ImGui::TreePop();
				}

				flag = ImGuiTreeNodeFlags_Leaf;
				if (m_selectedObject == -3)
					flag |= ImGuiTreeNodeFlags_Selected;
				if (ImGui::TreeNodeEx("light", flag))
				{
					if (ImGui::IsItemClicked())
						m_selectedObject = -3;
					ImGui::TreePop();
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Property"))
			{
				if (m_selectedObject == -3)
				{
					ImGuiH::PropertyEditor::begin();

					ImGuiH::PropertyEditor::entry("direction", [&]()
												  { return ImGui::InputFloat3("##direction", &m_pushConstants.lightDirection.x); });
					ImGuiH::PropertyEditor::entry("intensity", [&]()
												  { return ImGui::SliderFloat("##intensity", &m_pushConstants.lightIntensity, .01f, 10.f); });

					ImGuiH::PropertyEditor::end();
				}
				else if (m_selectedObject == -2)
					ImGuiH::CameraWidget();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Profiler"))
			{
				if (ImGui::CollapsingHeader("Stats"))
				{
					ImGuiH::Control::Group<bool>("Profiler Measure", false, [&]
												 { return guiProfilerMeasures(profiler); });
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
	}
}

void Application::updateBuffers(const VkCommandBuffer &cmdBuffer)
{
	// update CameraProperty (Push Constants)
	const auto &matView = CameraManip.getMatrix();
	const auto &matProj = nvmath::perspectiveVK(CameraManip.getFov(), (float)m_size.height / m_size.width, CameraManip.getClipPlanes().x, CameraManip.getClipPlanes().y);
	m_pushConstants.matrixModel = nvmath::scale_mat4(nvmath::vec3f_one * .15f);
	m_pushConstants.matrixView = matView;
	m_pushConstants.matrixProjection = matProj;
	m_pushConstants.nearClip = CameraManip.getClipPlanes().x;
	m_pushConstants.farClip = CameraManip.getClipPlanes().y;
}

void Application::recreateRenderTarget()
{
	if (m_visibilityBuffer.memHandle != nullptr)
	{
		m_visibilityBuffer.descriptor.sampler = VK_NULL_HANDLE;
		m_allocator.destroy(m_visibilityBuffer);
	}
	if (m_depthBuffer.memHandle != nullptr)
	{
		m_visibilityBuffer.descriptor.sampler = VK_NULL_HANDLE;
		m_allocator.destroy(m_depthBuffer);
	}

	auto visibilityBufferImage = m_allocator.createImage(nvvk::makeImage2DCreateInfo({m_size.width, m_size.height}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
	m_visibilityBuffer = m_allocator.createTexture(visibilityBufferImage, nvvk::makeImage2DViewCreateInfo(visibilityBufferImage.image));
	m_visibilityBuffer.descriptor.sampler = m_defaultBufferImageSampler;
	auto depthBufferImage = m_allocator.createImage(nvvk::makeImage2DCreateInfo({m_size.width, m_size.height}, m_depthFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
	m_depthBuffer = m_allocator.createTexture(depthBufferImage, nvvk::makeImage2DViewCreateInfo(depthBufferImage.image, m_depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT));
	m_depthBuffer.descriptor.sampler = m_defaultBufferImageSampler;

	{
		nvvk::ScopeCommandBuffer scopedBuffer(m_device, m_graphicsQueue.familyIndex, m_graphicsQueue.queue);
		nvvk::cmdBarrierImageLayout(scopedBuffer, m_visibilityBuffer.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		nvvk::cmdBarrierImageLayout(scopedBuffer, m_depthBuffer.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
		m_visibilityBuffer.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		m_depthBuffer.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	{
		nvvk::ScopeCommandBuffer scopedBuffer(m_device, m_transferQueue.familyIndex, m_transferQueue.queue);

		std::vector<VkWriteDescriptorSet> writeDescs{};
		writeDescs.emplace_back(m_attachmentsContainer.makeWrite(0, 0, &m_visibilityBuffer.descriptor));
		writeDescs.emplace_back(m_attachmentsContainer.makeWrite(0, 1, &m_depthBuffer.descriptor));
		vkUpdateDescriptorSets(m_device, writeDescs.size(), writeDescs.data(), 0, nullptr);
	}

	m_dynamicColorAttachs[0].imageView = m_visibilityBuffer.descriptor.imageView;
	m_dynamicDepthAttach[0].imageView = m_depthBuffer.descriptor.imageView;
	m_dynamicRenderingInfo.renderArea = {{}, m_size};
	m_dynamicRenderingInfo.layerCount = 1;
	m_dynamicRenderingInfo.colorAttachmentCount = m_dynamicColorAttachs.size();
	m_dynamicRenderingInfo.pColorAttachments = m_dynamicColorAttachs.data();
	m_dynamicRenderingInfo.pDepthAttachment = m_dynamicDepthAttach.data();
	m_dynamicRenderingInfo.pStencilAttachment = m_dynamicDepthAttach.data();
}

void Application::createDescriptors()
{
	if (m_defaultBufferImageSampler == VK_NULL_HANDLE)
		m_defaultBufferImageSampler = m_allocator.acquireSampler(nvvk::makeSamplerCreateInfo());

	m_attachmentsContainer.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, &m_defaultBufferImageSampler);
	m_attachmentsContainer.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, &m_defaultBufferImageSampler);
	m_attachmentsContainer.initLayout();
	m_attachmentsContainer.initPool(1);
}

void Application::createPipeline()
{
	vkDestroyPipelineLayout(m_device, m_visibilityPipelineLayout, VK_NULL_HANDLE);
	vkDestroyPipeline(m_device, m_visibilityPipeline, VK_NULL_HANDLE);
	vkDestroyPipelineLayout(m_device, m_shadingPipelineLayout, VK_NULL_HANDLE);
	vkDestroyPipeline(m_device, m_shadingPipeline, VK_NULL_HANDLE);

	VkPushConstantRange pushConstantRange = {VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstants)};
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = nvvk::make<VkPipelineLayoutCreateInfo>();
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
	NVVK_CHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, VK_NULL_HANDLE, &m_visibilityPipelineLayout));
	std::array<VkDescriptorSetLayout, 3> mergedLayouts{};
	mergedLayouts[0] = Scene::getInstance().m_geometrySetLayout;
	mergedLayouts[1] = m_attachmentsContainer.getLayout();
	mergedLayouts[2] = Scene::getInstance().m_textureSetLayout;
	pipelineLayoutCreateInfo.setLayoutCount = mergedLayouts.size();
	pipelineLayoutCreateInfo.pSetLayouts = mergedLayouts.data();
	NVVK_CHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, VK_NULL_HANDLE, &m_shadingPipelineLayout));

	std::array<VkFormat, 1> dynamicColorAttachFormat{VK_FORMAT_R8G8B8A8_UNORM};
	VkPipelineRenderingCreateInfo pipelineRenderingInfo{VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO, nullptr};
	pipelineRenderingInfo.colorAttachmentCount = m_dynamicColorAttachs.size();
	pipelineRenderingInfo.pColorAttachmentFormats = dynamicColorAttachFormat.data();
	pipelineRenderingInfo.depthAttachmentFormat = m_depthFormat;
	pipelineRenderingInfo.stencilAttachmentFormat = m_depthFormat;

	nvvk::GraphicsPipelineGeneratorCombined visibilityPipelineHelper(m_device, m_visibilityPipelineLayout, VK_NULL_HANDLE);
	visibilityPipelineHelper.addShader(nvh::loadFile("builtin_resources/shaders/visibilityPass.vert.spv", true), VK_SHADER_STAGE_VERTEX_BIT);
	visibilityPipelineHelper.addShader(nvh::loadFile("builtin_resources/shaders/visibilityPass.frag.spv", true), VK_SHADER_STAGE_FRAGMENT_BIT);
	visibilityPipelineHelper.addBindingDescription(visibilityPipelineHelper.makeVertexInputBinding(0, sizeof(VertexAttribute)));
	visibilityPipelineHelper.addAttributeDescription(visibilityPipelineHelper.makeVertexInputAttribute(0, 0, VkFormat::VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexAttribute, position)));
	visibilityPipelineHelper.addAttributeDescription(visibilityPipelineHelper.makeVertexInputAttribute(1, 0, VkFormat::VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexAttribute, normal)));
	visibilityPipelineHelper.addAttributeDescription(visibilityPipelineHelper.makeVertexInputAttribute(2, 0, VkFormat::VK_FORMAT_R32G32_SFLOAT, offsetof(VertexAttribute, uv)));
	visibilityPipelineHelper.setPipelineRenderingCreateInfo(pipelineRenderingInfo);
	m_visibilityPipeline = visibilityPipelineHelper.createPipeline();

	nvvk::GraphicsPipelineGeneratorCombined shadingPipelineHelper(m_device, m_shadingPipelineLayout, m_renderPass);
	shadingPipelineHelper.addShader(nvh::loadFile("builtin_resources/shaders/screenQuad.vert.spv", true), VK_SHADER_STAGE_VERTEX_BIT);
	shadingPipelineHelper.addShader(nvh::loadFile("builtin_resources/shaders/shadingPass.frag.spv", true), VK_SHADER_STAGE_FRAGMENT_BIT);
	shadingPipelineHelper.rasterizationState.cullMode = VK_CULL_MODE_NONE;
	m_shadingPipeline = shadingPipelineHelper.createPipeline();
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
	m_allocator.releaseSampler(m_defaultBufferImageSampler);
	m_visibilityBuffer.descriptor.sampler = VK_NULL_HANDLE;
	m_depthBuffer.descriptor.sampler = VK_NULL_HANDLE;
	if (m_visibilityBuffer.memHandle != nullptr)
		m_allocator.destroy(m_visibilityBuffer);
	if (m_depthBuffer.memHandle != nullptr)
		m_allocator.destroy(m_depthBuffer);

	vkDestroyPipelineLayout(m_device, m_visibilityPipelineLayout, VK_NULL_HANDLE);
	vkDestroyPipeline(m_device, m_visibilityPipeline, VK_NULL_HANDLE);
	vkDestroyPipelineLayout(m_device, m_shadingPipelineLayout, VK_NULL_HANDLE);
	vkDestroyPipeline(m_device, m_shadingPipeline, VK_NULL_HANDLE);

	m_attachmentsContainer.deinit();
	m_allocator.deinit();
	Scene::getInstance().deinit();
}
