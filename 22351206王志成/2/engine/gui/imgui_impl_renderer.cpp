#include "imgui_impl_renderer.h"

static unsigned char imgui_display_vert_code[] = {
#include <imgui_display.vert.spv.h>
};

static unsigned char imgui_display_frag_code[] = {
#include <imgui_display.frag.spv.h>
};

ImGuiRenderer::ImGuiRenderer(RenderContext &renderContext, vk::SampleCountFlagBits msaaSamples)
    : m_renderContextHandle(&renderContext), m_msaaSamples(msaaSamples)
{
    ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    // pre create static objects
    if (!m_fontSampler)
    {
        vk::SamplerCreateInfo createInfo{};
        createInfo.setMagFilter(vk::Filter::eLinear)
            .setMinFilter(vk::Filter::eLinear)
            .setMipmapMode(vk::SamplerMipmapMode::eLinear)
            .setAddressModeU(vk::SamplerAddressMode::eRepeat)
            .setAddressModeV(vk::SamplerAddressMode::eRepeat)
            .setAddressModeW(vk::SamplerAddressMode::eRepeat)
            .setMinLod(-1000)
            .setMaxLod(1000)
            .setMaxAnisotropy(1.0);
        m_fontSampler = m_renderContextHandle->getDeviceHandle()->createSampler(createInfo, allocationCallbacks);
    }
    if (!m_descSetLayout)
    {
        std::array<vk::DescriptorSetLayoutBinding, 1> binding{{}};
        binding[0].setDescriptorType(vk::DescriptorType::eCombinedImageSampler).setDescriptorCount(1U).setStageFlags(vk::ShaderStageFlagBits::eFragment);
        vk::DescriptorSetLayoutCreateInfo createInfo{};
        createInfo.setBindings(binding);
        m_descSetLayout = m_renderContextHandle->getDeviceHandle()->createDescriptorSetLayout(createInfo, allocationCallbacks);
    }
    if (!m_pipelineLayout)
    {
        std::array<vk::PushConstantRange, 1> pushConstants{{}};
        pushConstants[0].setStageFlags(vk::ShaderStageFlagBits::eVertex).setOffset(0U).setSize(sizeof(float) * 4);
        vk::PipelineLayoutCreateInfo createInfo{};
        createInfo.setSetLayoutCount(1U)
            .setPSetLayouts(&m_descSetLayout)
            .setPushConstantRanges(pushConstants);
        m_pipelineLayout = m_renderContextHandle->getDeviceHandle()->createPipelineLayout(createInfo, allocationCallbacks);
    }
}

ImGuiRenderer::~ImGuiRenderer()
{
    m_renderContextHandle->getDeviceHandle()->waitIdle();
    if (m_fontImageView)
        m_renderContextHandle->getDeviceHandle()->destroyImageView(m_fontImageView, allocationCallbacks);
    if (m_fontImage)
        m_renderContextHandle->getDeviceHandle()->destroyImage(m_fontImage, allocationCallbacks);
    if (m_fontSampler)
        m_renderContextHandle->getDeviceHandle()->destroySampler(m_fontSampler, allocationCallbacks);
    if (m_descSetLayout)
        m_renderContextHandle->getDeviceHandle()->destroyDescriptorSetLayout(m_descSetLayout, allocationCallbacks);
    if (m_pipelineLayout)
        m_renderContextHandle->getDeviceHandle()->destroyPipelineLayout(m_pipelineLayout, allocationCallbacks);

    clearFrameData();
    m_renderContextHandle->getDeviceHandle()->destroyPipeline(m_pipeline, allocationCallbacks);
    m_renderContextHandle->getDeviceHandle()->destroyRenderPass(m_renderPass, allocationCallbacks);
    m_renderContextHandle->getDeviceHandle()->destroySwapchainKHR(m_swapchain, allocationCallbacks);
    m_renderContextHandle->getInstanceHandle()->destroySurfaceKHR(m_surface, allocationCallbacks);
}

void ImGuiRenderer::initPresenter(const std::initializer_list<vk::Format> &requiredFormats,
                                  const std::initializer_list<vk::PresentModeKHR> &requiredPresentModes,
                                  const vk::ColorSpaceKHR &requiredColorSpace)
{
    if (!m_surface)
        throw std::runtime_error("surface handle has not been initialized!");
    if (requiredFormats.size() == 0)
        throw std::runtime_error("cannot init presenter with none formats!");
    if (requiredPresentModes.size() == 0)
        throw std::runtime_error("cannot init presenter with none present modes!");

    auto surfaceFormats = m_renderContextHandle->getAdapterHandle()->getSurfaceFormatsKHR(m_surface);
    // check if any format is available
    bool found = false;
    if (surfaceFormats.size() == 1)
    {
        if (surfaceFormats.front().format == vk::Format::eUndefined)
        {
            m_surfaceFormat.colorSpace = requiredColorSpace;
            m_surfaceFormat.format = *requiredFormats.begin();
            found = true;
        }
    }
    else
    {
        for (const auto &format : surfaceFormats)
        {
            for (const auto &req_format : requiredFormats)
            {
                if (format.format == req_format && format.colorSpace == requiredColorSpace)
                {
                    m_surfaceFormat = format;
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
    }
    if (!found)
        m_surfaceFormat = surfaceFormats[0];

    auto presentModes = m_renderContextHandle->getAdapterHandle()->getSurfacePresentModesKHR(m_surface);
    found = false;
    for (const auto &mode : presentModes)
    {
        for (const auto &req_mode : requiredPresentModes)
        {
            if (mode == req_mode)
            {
                m_surfacePresentMode = mode;
                found = true;
                break;
            }
        }
        if (found)
            break;
    }
    if (!found)
        m_surfacePresentMode = vk::PresentModeKHR::eFifo;
}

void ImGuiRenderer::recreatePresenter(const uint32_t &width, const uint32_t &height, uint32_t minBufferCount)
{
    auto deviceHandle = m_renderContextHandle->getDeviceHandle();
    deviceHandle->waitIdle();
    auto oldSwapchain = m_swapchain;
    clearFrameData();
    if (m_renderPass)
        deviceHandle->destroyRenderPass(m_renderPass, allocationCallbacks);
    if (m_pipeline)
        deviceHandle->destroyPipeline(m_pipeline, allocationCallbacks);

    if (minBufferCount == 0)
    {
        switch (m_surfacePresentMode)
        {
        case vk::PresentModeKHR::eMailbox:
            minBufferCount = 3;
            break;
        case vk::PresentModeKHR::eFifo:
        case vk::PresentModeKHR::eFifoRelaxed:
            minBufferCount = 2;
            break;
        case vk::PresentModeKHR::eImmediate:
            minBufferCount = 1;
            break;
        default:
            throw std::runtime_error("failed to calculate appropriate min buffer count!");
        }
    }

    // recreate swapchain
    {
        auto surfaceCapabilities = m_renderContextHandle->getAdapterHandle()->getSurfaceCapabilitiesKHR(m_surface);
        minBufferCount = std::min(minBufferCount, surfaceCapabilities.minImageCount);
        if (surfaceCapabilities.maxImageCount && surfaceCapabilities.maxImageCount <= minBufferCount)
            minBufferCount = std::min(minBufferCount, surfaceCapabilities.maxImageCount);
        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.setSurface(m_surface)
            .setMinImageCount(minBufferCount)
            .setImageFormat(m_surfaceFormat.format)
            .setImageColorSpace(m_surfaceFormat.colorSpace)
            .setPresentMode(m_surfacePresentMode)
            .setImageArrayLayers(1U)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setClipped(VK_TRUE)
            .setOldSwapchain(oldSwapchain);
        if (surfaceCapabilities.currentExtent.width == 0xffffffff)
        {
            createInfo.imageExtent.width = m_renderWidth = width;
            createInfo.imageExtent.height = m_renderHeight = height;
        }
        else
        {
            createInfo.imageExtent.width = m_renderWidth = surfaceCapabilities.currentExtent.width;
            createInfo.imageExtent.height = m_renderHeight = surfaceCapabilities.currentExtent.height;
        }
        m_swapchain = deviceHandle->createSwapchainKHR(createInfo, allocationCallbacks);
        auto swapchainBuffers = deviceHandle->getSwapchainImagesKHR(m_swapchain);
        if (swapchainBuffers.size() < minBufferCount)
            throw std::runtime_error("created swapchain has less buffer than required!");
        m_commandPools.resize(swapchainBuffers.size());
        m_commandBuffers.resize(swapchainBuffers.size());
        m_fences.resize(swapchainBuffers.size());
        m_backBuffers = swapchainBuffers;
        m_backBufferViews.resize(swapchainBuffers.size());
        m_framebuffers.resize(swapchainBuffers.size());
        m_imageAcquireSemaphores.resize(swapchainBuffers.size());
        m_renderCompleteSemaphores.resize(swapchainBuffers.size());
    }
    if (oldSwapchain)
        deviceHandle->destroySwapchainKHR(oldSwapchain, allocationCallbacks);

    // recreate renderpass
    // do not use dynamic state rendering by default
    {
        std::array<vk::AttachmentDescription, 1> attachment{{}};
        attachment[0].setFormat(m_surfaceFormat.format).setSamples(vk::SampleCountFlagBits::e1).setLoadOp(vk::AttachmentLoadOp::eClear).setStoreOp(vk::AttachmentStoreOp::eStore).setStencilLoadOp(vk::AttachmentLoadOp::eDontCare).setStencilStoreOp(vk::AttachmentStoreOp::eDontCare).setInitialLayout(vk::ImageLayout::eUndefined).setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
        std::array<vk::AttachmentReference, 1> color_attchment{{}};
        color_attchment[0].setAttachment(0U).setLayout(vk::ImageLayout::eColorAttachmentOptimal);
        std::array<vk::SubpassDescription, 1> subpass{{}};
        subpass[0].setPipelineBindPoint(vk::PipelineBindPoint::eGraphics).setColorAttachments(color_attchment);
        std::array<vk::SubpassDependency, 1> dependency{{}};
        dependency[0].setSrcSubpass(VK_SUBPASS_EXTERNAL).setDstSubpass(0U).setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput).setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput).setSrcAccessMask(vk::AccessFlagBits::eNone).setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
        vk::RenderPassCreateInfo createInfo{};
        createInfo.setAttachments(attachment)
            .setSubpasses(subpass)
            .setDependencies(dependency);
        m_renderPass = deviceHandle->createRenderPass(createInfo, allocationCallbacks);

        // We do not create a pipeline by default as this is also used by examples' main.cpp,
        // but secondary viewport in multi-viewport mode may want to create one with:
        // ImGui_ImplVulkan_CreatePipeline(device, allocator, VK_NULL_HANDLE, wd->RenderPass, VK_SAMPLE_COUNT_1_BIT, &wd->Pipeline, bd->Subpass);
    }

    // recreate image views
    {
        vk::ImageViewCreateInfo createInfo{};
        createInfo.setViewType(vk::ImageViewType::e2D)
            .setFormat(m_surfaceFormat.format)
            .setComponents({vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA})
            .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        for (auto &view : m_backBufferViews)
            view = deviceHandle->createImageView(createInfo, allocationCallbacks);
    }

    // recreate framebuffers
    {
        std::array<vk::ImageView, 1> attachment{{}};
        vk::FramebufferCreateInfo createInfo{};
        createInfo.setRenderPass(m_renderPass)
            .setAttachments(attachment)
            .setWidth(m_renderWidth)
            .setHeight(m_renderHeight)
            .setLayers(1U);
        for (auto i = 0; i < m_framebuffers.size(); ++i)
        {
            attachment[0] = m_backBufferViews[i];
            m_framebuffers[i] = deviceHandle->createFramebuffer(createInfo, allocationCallbacks);
        }
    }

    // reallocate command buffers
    for (auto i = 0; i < m_commandPools.size(); ++i)
    {
        {
            vk::CommandPoolCreateInfo createInfo{};
            createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                .setQueueFamilyIndex(m_renderContextHandle->getQueueInstanceHandle(vk::QueueFlagBits::eGraphics)->queue_family_index);
            m_commandPools[i] = deviceHandle->createCommandPool(createInfo, allocationCallbacks);
        }
        {
            vk::CommandBufferAllocateInfo allocInfo{};
            allocInfo.setCommandPool(m_commandPools[i])
                .setLevel(vk::CommandBufferLevel::ePrimary)
                .setCommandBufferCount(1U);
            m_commandBuffers[i] = deviceHandle->allocateCommandBuffers(allocInfo).front();
        }
        {
            vk::FenceCreateInfo createInfo{};
            createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
            m_fences[i] = deviceHandle->createFence(createInfo, allocationCallbacks);
        }
        {
            vk::SemaphoreCreateInfo createInfo{};
            m_imageAcquireSemaphores[i] = deviceHandle->createSemaphore(createInfo, allocationCallbacks);
            m_renderCompleteSemaphores[i] = deviceHandle->createSemaphore(createInfo, allocationCallbacks);
        }
    }

    // recreate pipeline
    {
        vk::ShaderModuleCreateInfo moduleCreateInfo{};
        moduleCreateInfo.setPCode((uint32_t *)imgui_display_vert_code).setCodeSize(sizeof(imgui_display_vert_code));
        vk::ShaderModule vertShader = deviceHandle->createShaderModule(moduleCreateInfo, allocationCallbacks);
        moduleCreateInfo.setPCode((uint32_t *)imgui_display_frag_code).setCodeSize(sizeof(imgui_display_frag_code));
        vk::ShaderModule fragShader = deviceHandle->createShaderModule(moduleCreateInfo, allocationCallbacks);

        std::array<vk::PipelineShaderStageCreateInfo, 2> stages{{}};
        stages[0].setStage(vk::ShaderStageFlagBits::eVertex).setModule(vertShader).setPName("main");
        stages[1].setStage(vk::ShaderStageFlagBits::eFragment).setModule(fragShader).setPName("main");

        std::array<vk::VertexInputBindingDescription, 1> vertBinding{{}};
        vertBinding[0].setStride(sizeof(ImDrawVert)).setInputRate(vk::VertexInputRate::eVertex);

        std::array<vk::VertexInputAttributeDescription, 3> vertAttribute{{}};
        vertAttribute[0].setLocation(0U).setBinding(vertBinding[0].binding).setFormat(vk::Format::eR32G32Sfloat).setOffset(IM_OFFSETOF(ImDrawVert, pos));
        vertAttribute[1].setLocation(1U).setBinding(vertBinding[0].binding).setFormat(vk::Format::eR32G32Sfloat).setOffset(IM_OFFSETOF(ImDrawVert, uv));
        vertAttribute[2].setLocation(2U).setBinding(vertBinding[0].binding).setFormat(vk::Format::eR8G8B8A8Unorm).setOffset(IM_OFFSETOF(ImDrawVert, col));

        vk::PipelineVertexInputStateCreateInfo vertexInputStateInfo{};
        vertexInputStateInfo.setVertexBindingDescriptions(vertBinding).setVertexAttributeDescriptions(vertAttribute);

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo{};
        inputAssemblyStateInfo.setTopology(vk::PrimitiveTopology::eTriangleList);

        vk::PipelineViewportStateCreateInfo viewportStateInfo{};
        viewportStateInfo.setViewportCount(1U).setScissorCount(1U);

        vk::PipelineRasterizationStateCreateInfo rasterStateInfo{};
        rasterStateInfo.setPolygonMode(vk::PolygonMode::eFill).setCullMode(vk::CullModeFlagBits::eNone).setFrontFace(vk::FrontFace::eCounterClockwise).setLineWidth(1.f);

        vk::PipelineMultisampleStateCreateInfo msStateInfo{};
        msStateInfo.setRasterizationSamples(m_msaaSamples);

        std::array<vk::PipelineColorBlendAttachmentState, 1> colorAttachment{{}};
        colorAttachment[0].setBlendEnable(VK_TRUE).setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha).setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha).setColorBlendOp(vk::BlendOp::eAdd).setSrcAlphaBlendFactor(vk::BlendFactor::eOne).setSrcAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha).setAlphaBlendOp(vk::BlendOp::eAdd).setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

        vk::PipelineDepthStencilStateCreateInfo depthStencilStateInfo{};

        vk::PipelineColorBlendStateCreateInfo blendStateInfo{};
        blendStateInfo.setAttachments(colorAttachment);

        std::array<vk::DynamicState, 2> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.setDynamicStates(dynamicStates);

        vk::GraphicsPipelineCreateInfo createInfo{};
        createInfo.setStages(stages)
            .setPVertexInputState(&vertexInputStateInfo)
            .setPInputAssemblyState(&inputAssemblyStateInfo)
            .setPViewportState(&viewportStateInfo)
            .setPRasterizationState(&rasterStateInfo)
            .setPMultisampleState(&msStateInfo)
            .setPDepthStencilState(&depthStencilStateInfo)
            .setPColorBlendState(&blendStateInfo)
            .setPDynamicState(&dynamicStateInfo)
            .setLayout(m_pipelineLayout)
            .setRenderPass(m_renderPass)
            .setSubpass({});

        auto [state, res] = deviceHandle->createGraphicsPipeline(*m_renderContextHandle->getPipelineCacheHandle(), createInfo, allocationCallbacks);
        if (state != vk::Result::eSuccess)
            throw std::runtime_error("imgui renderer failed to create internal graphics pipeline!");
        m_pipeline = res;

        deviceHandle->destroyShaderModule(vertShader, allocationCallbacks);
        deviceHandle->destroyShaderModule(fragShader, allocationCallbacks);
    }
}

void ImGuiRenderer::render()
{
    auto [status, res] = m_renderContextHandle->getDeviceHandle()->acquireNextImageKHR(m_swapchain, UINT64_MAX, m_imageAcquireSemaphores[m_frameIndex], {});
    m_frameIndex = res;
    assert(status >= vk::Result::eSuccess);
    if (status == vk::Result::eErrorOutOfDateKHR || status == vk::Result::eSuboptimalKHR)
    {
        m_dirtySwapchain = true;
        return;
    }

    // prepare
    vk::ClearValue clearValue = vk::ClearColorValue{.0f, .0f, .0f, 1.f};
    {
        m_renderContextHandle->getDeviceHandle()->waitForFences(m_fences[m_frameIndex], VK_TRUE, UINT64_MAX);
        m_renderContextHandle->getDeviceHandle()->resetFences(m_fences[m_frameIndex]);
    }
    {
        m_renderContextHandle->getDeviceHandle()->resetCommandPool(m_commandPools[m_frameIndex]);
        m_commandBuffers[m_frameIndex].begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    }
    {
        vk::Rect2D rect{{0, 0}, {static_cast<uint32_t>(m_renderWidth), static_cast<uint32_t>(m_renderHeight)}};
        vk::RenderPassBeginInfo beginInfo;
        beginInfo.setRenderPass(m_renderPass)
            .setFramebuffer(m_framebuffers[m_frameIndex])
            .setRenderArea(rect)
            .setClearValues(clearValue);
        m_commandBuffers[m_frameIndex].beginRenderPass(beginInfo, vk::SubpassContents::eInline);
    }

    // // render imgui draw data
    // {
    //     auto drawData = ImGui::GetDrawData();
    //     int frameBufferWidth = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
    //     int frameBufferHeight = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
    //     if (frameBufferWidth > 0 && frameBufferHeight > 0)
    //     {
    //         if (drawData->TotalVtxCount > 0)
    //         {
    //             // Create or resize the vertex/index buffers
    //             size_t vertex_size = drawData->TotalVtxCount * sizeof(ImDrawVert);
    //             size_t index_size = drawData->TotalIdxCount * sizeof(ImDrawIdx);
    //             if (rb->VertexBuffer == VK_NULL_HANDLE || rb->VertexBufferSize < vertex_size)
    //                 CreateOrResizeBuffer(rb->VertexBuffer, rb->VertexBufferMemory, rb->VertexBufferSize, vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    //             if (rb->IndexBuffer == VK_NULL_HANDLE || rb->IndexBufferSize < index_size)
    //                 CreateOrResizeBuffer(rb->IndexBuffer, rb->IndexBufferMemory, rb->IndexBufferSize, index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    //             // Upload vertex/index data into a single contiguous GPU buffer
    //             ImDrawVert *vtx_dst = nullptr;
    //             ImDrawIdx *idx_dst = nullptr;
    //             VkResult err = vkMapMemory(v->Device, rb->VertexBufferMemory, 0, rb->VertexBufferSize, 0, (void **)(&vtx_dst));
    //             check_vk_result(err);
    //             err = vkMapMemory(v->Device, rb->IndexBufferMemory, 0, rb->IndexBufferSize, 0, (void **)(&idx_dst));
    //             check_vk_result(err);
    //             for (int n = 0; n < draw_data->CmdListsCount; n++)
    //             {
    //                 const ImDrawList *cmd_list = draw_data->CmdLists[n];
    //                 memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
    //                 memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
    //                 vtx_dst += cmd_list->VtxBuffer.Size;
    //                 idx_dst += cmd_list->IdxBuffer.Size;
    //             }
    //             VkMappedMemoryRange range[2] = {};
    //             range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    //             range[0].memory = rb->VertexBufferMemory;
    //             range[0].size = VK_WHOLE_SIZE;
    //             range[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    //             range[1].memory = rb->IndexBufferMemory;
    //             range[1].size = VK_WHOLE_SIZE;
    //             err = vkFlushMappedMemoryRanges(v->Device, 2, range);
    //             check_vk_result(err);
    //             vkUnmapMemory(v->Device, rb->VertexBufferMemory);
    //             vkUnmapMemory(v->Device, rb->IndexBufferMemory);
    //         }
    //     }
    // }

    // post
    m_commandBuffers[m_frameIndex].endRenderPass();
    m_commandBuffers[m_frameIndex].end();
    {
        vk::PipelineStageFlags stageFlag = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(m_commandBuffers[m_frameIndex])
            .setWaitDstStageMask(stageFlag)
            .setWaitSemaphores(m_imageAcquireSemaphores[m_frameIndex])
            .setSignalSemaphores(m_renderCompleteSemaphores[m_frameIndex]);
        m_renderContextHandle->getQueueInstanceHandle(vk::QueueFlagBits::eGraphics)->queue_handle->submit(submitInfo, m_fences[m_frameIndex]);
    }

    if (!m_dirtySwapchain)
    {
        vk::PresentInfoKHR info{};
        info.setWaitSemaphores(m_renderCompleteSemaphores[m_frameIndex])
            .setImageIndices(m_frameIndex)
            .setSwapchains(m_swapchain);
        m_renderContextHandle->getQueueInstanceHandle(vk::QueueFlagBits::eGraphics)->queue_handle->presentKHR(info);
    }
}

void ImGuiRenderer::clearFrameData()
{
    auto deviceHandle = m_renderContextHandle->getDeviceHandle();
    for (auto i = 0; i < m_commandPools.size(); ++i)
        if (m_commandPools[i])
        {
            deviceHandle->freeCommandBuffers(m_commandPools[i], 1, &m_commandBuffers[i]);
            deviceHandle->destroyCommandPool(m_commandPools[i], allocationCallbacks);
        }
    for (auto &obj : m_backBuffers)
        if (obj)
            deviceHandle->destroyImage(obj, allocationCallbacks);
    for (auto &obj : m_backBufferViews)
        if (obj)
            deviceHandle->destroyImageView(obj, allocationCallbacks);
    for (auto &obj : m_framebuffers)
        if (obj)
            deviceHandle->destroyFramebuffer(obj, allocationCallbacks);
    for (auto &obj : m_fences)
        if (obj)
            deviceHandle->destroyFence(obj, allocationCallbacks);
    for (auto &obj : m_imageAcquireSemaphores)
        if (obj)
            deviceHandle->destroySemaphore(obj, allocationCallbacks);
    for (auto &obj : m_imageAcquireSemaphores)
        if (obj)
            deviceHandle->destroySemaphore(obj, allocationCallbacks);
}