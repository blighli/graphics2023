#include <iostream>
#include <fstream>

#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include <renderContext.h>
#include <simple_geometry.h>
#include <imageLoader.hpp>

static RenderContext g_renderContext{};
static vk::DescriptorPool g_descriptorPool{};

static ImGui_ImplVulkanH_Window g_mainWindow{};
static uint32_t g_minImageBufferCount = 3;
static bool g_rebuildSwapchain = false;

static Buffer g_earthVertexBuffer{}, g_moonVertexBuffer{}, g_sunVertexBuffer{};
static Buffer g_earthIndexBuffer{}, g_moonIndexBuffer{}, g_sunIndexBuffer{};
static uint32_t g_earthVertexCount{}, g_moonVertexCount{}, g_sunVertexCount{};
static Texture g_earthTexture{}, g_moonTexture{};
static vk::DescriptorSetLayout g_descSetLayout{};
static vk::DescriptorSet g_descSet{};

static vk::PipelineLayout g_graphicsPipelineLayout{};
static vk::Pipeline g_graphicsPipeline{};

static std::vector<unsigned char> g_vertShaderSPVCode = {
#include <planet.vert.spv.h>
};
static std::vector<unsigned char> g_fragShaderSPVCode = {
#include <planet.frag.spv.h>
};

struct PushConstants
{
    glm::mat4 mvp;
};

static void check_vk_result(VkResult res)
{
    if (!res)
        return;
    std::cerr << "[Vulkan] Error: VkResult = " << res << "\n";
    if (res < 0)
        abort();
}

static void render_frame(ImDrawData *draw_data)
{
    vk::SwapchainKHR swapchain = g_mainWindow.Swapchain;
    vk::Semaphore acquireSemaphore = g_mainWindow.FrameSemaphores[g_mainWindow.FrameIndex].ImageAcquiredSemaphore;
    vk::Semaphore waitSemaphore = g_mainWindow.FrameSemaphores[g_mainWindow.FrameIndex].RenderCompleteSemaphore;
    vk::CommandBuffer buffer = g_mainWindow.Frames[g_mainWindow.FrameIndex].CommandBuffer;
    vk::Fence fence = g_mainWindow.Frames[g_mainWindow.FrameIndex].Fence;
    vk::ClearValue clearValue = vk::ClearColorValue{.0f, .0f, .0f, 1.f};

    auto [status, res] = g_renderContext.getDeviceHandle()->acquireNextImageKHR(swapchain, UINT64_MAX, acquireSemaphore, {});
    g_mainWindow.FrameIndex = res;
    assert(status >= vk::Result::eSuccess);
    if (status == vk::Result::eErrorOutOfDateKHR || status == vk::Result::eSuboptimalKHR)
    {
        g_rebuildSwapchain = true;
        return;
    }

    {
        g_renderContext.getDeviceHandle()->waitForFences(fence, VK_TRUE, UINT64_MAX);
        g_renderContext.getDeviceHandle()->resetFences(fence);
    }
    {
        vk::CommandPool pool = g_mainWindow.Frames[g_mainWindow.FrameIndex].CommandPool;
        g_renderContext.getDeviceHandle()->resetCommandPool(pool);
        buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    }
    {
        vk::RenderPass pass = g_mainWindow.RenderPass;
        vk::Framebuffer frameBuffer = g_mainWindow.Frames[g_mainWindow.FrameIndex].Framebuffer;
        vk::Rect2D rect{{0, 0}, {static_cast<uint32_t>(g_mainWindow.Width), static_cast<uint32_t>(g_mainWindow.Height)}};
        vk::RenderPassBeginInfo beginInfo;
        beginInfo.setRenderPass(pass)
            .setFramebuffer(frameBuffer)
            .setRenderArea(rect)
            .setClearValues(clearValue);
        buffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
    }

    // RENDER SOMETHING HERE...
    {
        buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, g_graphicsPipeline);
        buffer.setViewport(0, vk::Viewport(.0f, .0f, g_mainWindow.Width, g_mainWindow.Height, .0f, 1.f));
        buffer.setScissor(0, vk::Rect2D({}, {static_cast<uint32_t>(g_mainWindow.Width), static_cast<uint32_t>(g_mainWindow.Height)}));
        buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, g_graphicsPipelineLayout, 0, g_descSet, {});
        
        vk::DeviceSize offset{};
        buffer.bindVertexBuffers(0, g_earthVertexBuffer.buffer, offset);
        buffer.bindIndexBuffer(g_earthIndexBuffer.buffer, offset, vk::IndexType::eUint32);
        buffer.drawIndexed(g_earthVertexCount, 1, 0, 0, 0);
    }
    ImGui_ImplVulkan_RenderDrawData(draw_data, buffer);

    buffer.endRenderPass();
    buffer.end();
    {
        vk::PipelineStageFlags stageFlag = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(buffer)
            .setWaitDstStageMask(stageFlag)
            .setWaitSemaphores(acquireSemaphore)
            .setSignalSemaphores(waitSemaphore);
        g_renderContext.getQueueInstanceHandle(vk::QueueFlagBits::eGraphics)->queue_handle->submit(submitInfo, fence);
    }
}

int main(int argc, char **argv)
{
    std::ofstream consoleOutput("console.txt");
    if (!consoleOutput)
    {
        std::cerr << "failed to open console output file.\n";
        return -1;
    }
    std::streambuf *pOldCoutStream = std::cout.rdbuf(consoleOutput.rdbuf());
    std::streambuf *pOldCerrStream = std::cerr.rdbuf(consoleOutput.rdbuf());

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window *window = SDL_CreateWindow("imgui_SDL2_Vulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    {
        std::vector<const char *> instanceExtensions{};
        uint32_t extensionCount = 0U;
        SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
        instanceExtensions.resize(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, instanceExtensions.data());
        g_renderContext.init(instanceExtensions, {}, {}, {});
    }
    {
        std::vector<vk::DescriptorPoolSize> poolSizes;
        poolSizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, 8U);
        vk::DescriptorPoolCreateInfo createInfo;
        createInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setMaxSets(4U)
            .setPoolSizes(poolSizes);
        g_descriptorPool = g_renderContext.getDeviceHandle()->createDescriptorPool(createInfo, allocationCallbacks);
    }

    // VkSurfaceKHR surfaceBase;
    vk::SurfaceKHR surface{};
    if (SDL_Vulkan_CreateSurface(window, *g_renderContext.getInstanceHandle(), reinterpret_cast<VkSurfaceKHR *>(&surface)) == SDL_FALSE)
    {
        std::cerr << "Failed to create Vulkan surface.\n";
        abort();
        return -1;
    }
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    {
        g_mainWindow.Surface = surface;

        auto surfaceSupportedFormats = g_renderContext.getAdapterHandle()->getSurfaceSupportKHR(g_renderContext.getQueueInstanceHandle(vk::QueueFlagBits::eGraphics)->queue_family_index, surface);

        const std::vector<VkFormat> requiredFormats{VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
        const VkColorSpaceKHR requiredColorSapce = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        g_mainWindow.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(*g_renderContext.getAdapterHandle(), surface, requiredFormats.data(), requiredFormats.size(), requiredColorSapce);
        const std::vector<VkPresentModeKHR> requiredPresentModes{VK_PRESENT_MODE_FIFO_KHR, VK_PRESENT_MODE_MAILBOX_KHR};
        g_mainWindow.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(*g_renderContext.getAdapterHandle(), surface, requiredPresentModes.data(), requiredPresentModes.size());

        ImGui_ImplVulkanH_CreateOrResizeWindow(*g_renderContext.getInstanceHandle(), *g_renderContext.getAdapterHandle(), *g_renderContext.getDeviceHandle(), &g_mainWindow, g_renderContext.getQueueInstanceHandle(vk::QueueFlagBits::eGraphics)->queue_family_index, reinterpret_cast<VkAllocationCallbacks *>(&allocationCallbacks), width, height, g_minImageBufferCount);
    }

    auto earthGeometry = generateSphere(0.5f, 32, 32);
    auto moonGeometry = generateSphere(1.f, 32, 32);
    auto sunGeometry = generateSphere(5.f, 32, 32);
    g_earthVertexCount = earthGeometry.second.size();
    g_moonVertexCount = moonGeometry.second.size();
    g_sunVertexCount = sunGeometry.second.size();

    g_earthVertexBuffer = g_renderContext.createVertexBuffer(earthGeometry.first);
    g_earthIndexBuffer = g_renderContext.createIndexBuffer(earthGeometry.second);
    g_moonVertexBuffer = g_renderContext.createVertexBuffer(moonGeometry.first);
    g_moonIndexBuffer = g_renderContext.createIndexBuffer(moonGeometry.second);
    g_sunVertexBuffer = g_renderContext.createVertexBuffer(sunGeometry.first);
    g_sunIndexBuffer = g_renderContext.createIndexBuffer(sunGeometry.second);

    auto earthTextureData = ImageLoader::getInstance().loadImage("./builtin_resources/textures/earth.png");
    auto moonTextureData = ImageLoader::getInstance().loadImage("./builtin_resources/textures/moon.png");
    // auto sunTextureData = ImageLoader::getInstance().loadImage("./builtin_resources/textures/sun.png");

    g_earthTexture = g_renderContext.createTexture(earthTextureData);
    g_moonTexture = g_renderContext.createTexture(moonTextureData);
    // auto sunTexture = g_renderContext.createTexture(sunTextureData);

    {
        std::array<vk::DescriptorSetLayoutBinding, 2> bindings{{}};
        bindings[0].setBinding(0).setStageFlags(vk::ShaderStageFlagBits::eFragment).setDescriptorCount(1).setDescriptorType(vk::DescriptorType::eCombinedImageSampler).setImmutableSamplers(g_earthTexture.descriptor.sampler);
        bindings[1].setBinding(1).setStageFlags(vk::ShaderStageFlagBits::eFragment).setDescriptorCount(1).setDescriptorType(vk::DescriptorType::eCombinedImageSampler).setImmutableSamplers(g_earthTexture.descriptor.sampler);
        vk::DescriptorSetLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.setBindings(bindings);
        g_descSetLayout = g_renderContext.getDeviceHandle()->createDescriptorSetLayout(layoutCreateInfo, allocationCallbacks);
        vk::DescriptorSetAllocateInfo setAllocInfo{};
        setAllocInfo.setDescriptorPool(g_descriptorPool)
            .setDescriptorSetCount(1)
            .setSetLayouts(g_descSetLayout);
        g_descSet = g_renderContext.getDeviceHandle()->allocateDescriptorSets(setAllocInfo).front();

        std::array<vk::WriteDescriptorSet, 2> writeSets{{}};
        writeSets[0].setDstSet(g_descSet).setDstBinding(0).setDescriptorCount(1).setDescriptorType(vk::DescriptorType::eCombinedImageSampler).setImageInfo(g_earthTexture.descriptor);
        writeSets[1].setDstSet(g_descSet).setDstBinding(1).setDescriptorCount(1).setDescriptorType(vk::DescriptorType::eCombinedImageSampler).setImageInfo(g_moonTexture.descriptor);
        g_renderContext.getDeviceHandle()->updateDescriptorSets(writeSets, {});
    }
    {
        vk::PipelineLayoutCreateInfo createInfo{};
        createInfo.setSetLayouts(g_descSetLayout);
        g_graphicsPipelineLayout = g_renderContext.getDeviceHandle()->createPipelineLayout(createInfo, allocationCallbacks);

        GraphicsPipelineHelper graphicsPipelineHelper{g_renderContext.getDeviceHandle(), g_graphicsPipelineLayout, g_mainWindow.RenderPass};
        graphicsPipelineHelper.setPipelineCache(g_renderContext.getPipelineCacheHandle());
        graphicsPipelineHelper.addShader(g_vertShaderSPVCode, vk::ShaderStageFlagBits::eVertex);
        graphicsPipelineHelper.addShader(g_fragShaderSPVCode, vk::ShaderStageFlagBits::eFragment);
        graphicsPipelineHelper.addBindingDescription(graphicsPipelineHelper.makeVertexInputBinding(0, sizeof(VertexAttribute)));
        graphicsPipelineHelper.addAttributeDescription(graphicsPipelineHelper.makeVertexInputAttribute(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexAttribute, position)));
        graphicsPipelineHelper.addAttributeDescription(graphicsPipelineHelper.makeVertexInputAttribute(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexAttribute, normal)));
        graphicsPipelineHelper.addAttributeDescription(graphicsPipelineHelper.makeVertexInputAttribute(2, 0, vk::Format::eR32G32Sfloat, offsetof(VertexAttribute, texCoord)));
        graphicsPipelineHelper.addBlendAttachmentState(graphicsPipelineHelper.makePipelineColorBlendAttachmentState());
        g_graphicsPipeline = graphicsPipelineHelper.createPipeline();
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

    ImGui_ImplSDL2_InitForVulkan(window);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = *g_renderContext.getInstanceHandle();
    init_info.PhysicalDevice = *g_renderContext.getAdapterHandle();
    init_info.Device = *g_renderContext.getDeviceHandle();
    init_info.QueueFamily = g_renderContext.getQueueInstanceHandle(vk::QueueFlagBits::eGraphics)->queue_family_index;
    init_info.Queue = *g_renderContext.getQueueInstanceHandle(vk::QueueFlagBits::eGraphics)->queue_handle;
    init_info.PipelineCache = *g_renderContext.getPipelineCacheHandle();
    init_info.DescriptorPool = g_descriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = g_minImageBufferCount;
    init_info.ImageCount = g_mainWindow.ImageCount;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = reinterpret_cast<VkAllocationCallbacks *>(&allocationCallbacks);
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, g_mainWindow.RenderPass);

    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if (event.type == SDL_WINDOWEVENT && (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) && event.window.windowID == SDL_GetWindowID(window))
                g_rebuildSwapchain = true;
        }

        if (g_rebuildSwapchain)
        {
            int width, height;
            SDL_GetWindowSize(window, &width, &height);
            if (width > 0 && height > 0)
            {
                ImGui_ImplVulkan_SetMinImageCount(g_minImageBufferCount);
                ImGui_ImplVulkanH_CreateOrResizeWindow(*g_renderContext.getInstanceHandle(), *g_renderContext.getAdapterHandle(), *g_renderContext.getDeviceHandle(), &g_mainWindow, g_renderContext.getQueueInstanceHandle(vk::QueueFlagBits::eGraphics)->queue_family_index, reinterpret_cast<VkAllocationCallbacks *>(&allocationCallbacks), width, height, g_minImageBufferCount);
                g_mainWindow.FrameIndex = 0;
                g_rebuildSwapchain = false;
            }
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();
        const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
        if (!is_minimized)
        {
            g_mainWindow.ClearValue.color = {.0f, .0f, .0f, 1.f};
            vk::SwapchainKHR swapchain = g_mainWindow.Swapchain;
            vk::Semaphore waitSemaphore = g_mainWindow.FrameSemaphores[g_mainWindow.FrameIndex].RenderCompleteSemaphore;
            // render contexts
            render_frame(draw_data);
            // present contexts
            if (!g_rebuildSwapchain)
            {
                vk::PresentInfoKHR presentInfo;
                presentInfo.setWaitSemaphores(waitSemaphore)
                    .setImageIndices(g_mainWindow.FrameIndex)
                    .setSwapchains(swapchain);
                g_renderContext.getQueueInstanceHandle(vk::QueueFlagBits::eGraphics)->queue_handle->presentKHR(presentInfo);
            }
        }
    }

    g_renderContext.getDeviceHandle()->waitIdle();
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    ImGui_ImplVulkanH_DestroyWindow(*g_renderContext.getInstanceHandle(), *g_renderContext.getDeviceHandle(), &g_mainWindow, reinterpret_cast<VkAllocationCallbacks *>(&allocationCallbacks));
    {
        g_renderContext.destroyBuffer(g_earthVertexBuffer);
        g_renderContext.destroyBuffer(g_earthIndexBuffer);
        g_renderContext.destroyBuffer(g_moonVertexBuffer);
        g_renderContext.destroyBuffer(g_moonIndexBuffer);
        g_renderContext.destroyBuffer(g_sunVertexBuffer);
        g_renderContext.destroyBuffer(g_sunIndexBuffer);

        g_renderContext.destroyTexture(g_earthTexture);
        g_renderContext.destroyTexture(g_moonTexture);

        g_renderContext.getDeviceHandle()->destroy(g_graphicsPipelineLayout, allocationCallbacks);
        g_renderContext.getDeviceHandle()->destroy(g_graphicsPipeline, allocationCallbacks);
        g_renderContext.getDeviceHandle()->freeDescriptorSets(g_descriptorPool, g_descSet);
        g_renderContext.getDeviceHandle()->destroy(g_descSetLayout, allocationCallbacks);
        g_renderContext.getDeviceHandle()->destroy(g_descriptorPool, allocationCallbacks);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    consoleOutput.close();

    return 0;
}