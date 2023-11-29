#include <iostream>

#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

static vk::AllocationCallbacks g_allocCallbacks{};
static vk::Instance g_instance{VK_NULL_HANDLE};
static vk::PhysicalDevice g_adapter{VK_NULL_HANDLE};
static uint32_t g_graphicsQueueFamilyIndex{static_cast<uint32_t>(-1)};
static vk::Queue g_graphicsQueue{VK_NULL_HANDLE};
static vk::Device g_device{VK_NULL_HANDLE};
static vk::DescriptorPool g_descriptorPool{VK_NULL_HANDLE};
static vk::DebugReportCallbackEXT g_debugReporter{VK_NULL_HANDLE};

static ImGui_ImplVulkanH_Window g_mainWindow{};
static uint32_t g_minImageBufferCount = 3;
static bool g_rebuildSwapchain = false;

#ifdef _DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage, void *pUserData)
{
    std::cerr << "[Vulkan] Debug report message: " << pMessage << "(from object type: " << objectType << ")\n";
    return VK_FALSE;
}
#endif

static void check_vk_result(VkResult res)
{
    if (!res)
        return;
    std::cerr << "[Vulkan] Error: VkResult = " << res << "\n";
    if (res < 0)
        abort();
}

int main(int argc, char **argv)
{
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
        vk::ApplicationInfo appInfo;
        appInfo.setApiVersion(VK_API_VERSION_1_3);
        vk::InstanceCreateInfo createInfo;
        std::vector<const char *> instanceExtensions{};
        std::vector<const char *> instanceLayers{};

        uint32_t extensionCount = 0U;
        SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
        instanceExtensions.resize(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, instanceExtensions.data());

#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
        auto instanceExtProperties = vk::enumerateInstanceExtensionProperties();
        for (const auto &property : instanceExtProperties)
        {
            if (property.extensionName == VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)
            {
                createInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
                instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            }
        }
#endif
#ifdef _DEBUG
        instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
        instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
        createInfo.setPApplicationInfo(&appInfo)
            .setPEnabledExtensionNames(instanceExtensions)
            .setPEnabledLayerNames(instanceLayers);
        g_instance = vk::createInstance(createInfo, g_allocCallbacks);

#ifdef _DEBUG
        vk::DebugReportCallbackCreateInfoEXT callbackCreateInfo;
        callbackCreateInfo.setFlags(vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning | vk::DebugReportFlagBitsEXT::eError);
        callbackCreateInfo.setPfnCallback(debug_report);
        callbackCreateInfo.setPUserData(nullptr);
        g_debugReporter = g_instance.createDebugReportCallbackEXT(callbackCreateInfo, g_allocCallbacks);
#endif
    }

    {
        auto adapters = g_instance.enumeratePhysicalDevices();
        for (const auto &adapter : adapters)
        {
            auto property = adapter.getProperties2();
            if (property.properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
                g_adapter = adapter;
        }
        if (!adapters.empty() && !g_adapter)
            g_adapter = adapters[0];
    }

    {
        auto queueFamilyProperties = g_adapter.getQueueFamilyProperties2();
        for (auto i = 0; i < queueFamilyProperties.size(); ++i)
            if (queueFamilyProperties[i].queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics)
                g_graphicsQueueFamilyIndex = i;
    }

    {
        std::vector<const char *> deviceExtensions;
        deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
        auto deviceExtProperties = g_adapter.enumerateDeviceExtensionProperties();
        for (const auto &property : deviceExtProperties)
        {
            if (property.extensionName == VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)
            {
                deviceExtensions.emplace_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
            }
        }
#endif

        const float priority = 1.f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};
        queueCreateInfos.resize(1);
        queueCreateInfos[0].setQueueCount(1U).setQueueFamilyIndex(g_graphicsQueueFamilyIndex).setQueuePriorities(priority);
        vk::DeviceCreateInfo createInfo;
        createInfo.setQueueCreateInfos(queueCreateInfos)
            .setPEnabledExtensionNames(deviceExtensions);
        g_device = g_adapter.createDevice(createInfo, g_allocCallbacks);
        g_graphicsQueue = g_device.getQueue(g_graphicsQueueFamilyIndex, 0U);
    }

    {
        std::vector<vk::DescriptorPoolSize> poolSizes;
        poolSizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, 8U);
        vk::DescriptorPoolCreateInfo createInfo;
        createInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setMaxSets(1U)
            .setPoolSizes(poolSizes);
        g_descriptorPool = g_device.createDescriptorPool(createInfo, g_allocCallbacks);
    }

    VkSurfaceKHR surfaceBase;
    if (SDL_Vulkan_CreateSurface(window, g_instance, &surfaceBase) == SDL_FALSE)
    {
        std::cerr << "Failed to create Vulkan surface.\n";
        abort();
        return -1;
    }
    vk::SurfaceKHR surface{surfaceBase};

    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    {
        g_mainWindow.Surface = surface;

        auto surfaceSupportedFormats = g_adapter.getSurfaceSupportKHR(g_graphicsQueueFamilyIndex, surface);

        const std::vector<VkFormat> requiredFormats{VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
        const VkColorSpaceKHR requiredColorSapce = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        g_mainWindow.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_adapter, surface, requiredFormats.data(), requiredFormats.size(), requiredColorSapce);
        const std::vector<VkPresentModeKHR> requiredPresentModes{VK_PRESENT_MODE_FIFO_KHR, VK_PRESENT_MODE_MAILBOX_KHR};
        g_mainWindow.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_adapter, surface, requiredPresentModes.data(), requiredPresentModes.size());

        ImGui_ImplVulkanH_CreateOrResizeWindow(g_instance, g_adapter, g_device, &g_mainWindow, g_graphicsQueueFamilyIndex, VK_NULL_HANDLE, width, height, g_minImageBufferCount);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

    ImGui_ImplSDL2_InitForVulkan(window);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = g_instance;
    init_info.PhysicalDevice = g_adapter;
    init_info.Device = g_device;
    init_info.QueueFamily = g_graphicsQueueFamilyIndex;
    init_info.Queue = g_graphicsQueue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = g_descriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = g_minImageBufferCount;
    init_info.ImageCount = g_mainWindow.ImageCount;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = reinterpret_cast<const VkAllocationCallbacks *>(&g_allocCallbacks);
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, g_mainWindow.RenderPass);

    {
        vk::CommandPool pool = g_mainWindow.Frames[g_mainWindow.FrameIndex].CommandPool;
        vk::CommandBuffer buffer = g_mainWindow.Frames[g_mainWindow.FrameIndex].CommandBuffer;

        g_device.resetCommandPool(pool);
        buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        ImGui_ImplVulkan_CreateFontsTexture(buffer);
        buffer.end();

        std::vector<vk::SubmitInfo> submitInfos;
        submitInfos.resize(1);
        submitInfos[0].setCommandBufferCount(1U).setPCommandBuffers(&buffer);
        g_graphicsQueue.submit(submitInfos);
        g_device.waitIdle();

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

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
        }

        if (g_rebuildSwapchain)
        {
            int width, height;
            SDL_GetWindowSize(window, &width, &height);
            if (width > 0 && height > 0)
            {
                ImGui_ImplVulkan_SetMinImageCount(g_minImageBufferCount);
                ImGui_ImplVulkanH_CreateOrResizeWindow(g_instance, g_adapter, g_device, &g_mainWindow, g_graphicsQueueFamilyIndex, reinterpret_cast<const VkAllocationCallbacks *>(&g_allocCallbacks), width, height, g_minImageBufferCount);
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
            vk::ClearValue clearValue = vk::ClearColorValue{.0f, .0f, .0f, 1.f};
            vk::SwapchainKHR swapchain = g_mainWindow.Swapchain;
            ImGui_ImplVulkanH_Frame *fd = &g_mainWindow.Frames[g_mainWindow.FrameIndex];
            vk::Semaphore acquireSemaphore = g_mainWindow.FrameSemaphores[g_mainWindow.FrameIndex].ImageAcquiredSemaphore;
            vk::Semaphore waitSemaphore = g_mainWindow.FrameSemaphores[g_mainWindow.FrameIndex].RenderCompleteSemaphore;
            vk::CommandBuffer buffer = fd->CommandBuffer;
            vk::Fence fence = fd->Fence;
            // render contexts
            {
                auto [status, res] = g_device.acquireNextImageKHR(swapchain, UINT64_MAX, acquireSemaphore, {});
                g_mainWindow.FrameIndex = res;
                if (status == vk::Result::eErrorOutOfDateKHR || status == vk::Result::eSuboptimalKHR)
                    g_rebuildSwapchain = true;
                else
                {
                    {
                        g_device.waitForFences(fence, VK_TRUE, UINT64_MAX);
                        g_device.resetFences(fence);
                    }
                    {
                        vk::CommandPool pool = fd->CommandPool;
                        g_device.resetCommandPool(pool);
                        buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
                    }
                    {
                        vk::RenderPass pass = g_mainWindow.RenderPass;
                        vk::Framebuffer frameBuffer = fd->Framebuffer;
                        vk::Rect2D rect{{0, 0}, {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}};
                        vk::RenderPassBeginInfo beginInfo;
                        beginInfo.setRenderPass(pass)
                            .setFramebuffer(frameBuffer)
                            .setRenderArea(rect)
                            .setClearValues(clearValue);
                        buffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
                    }

                    ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

                    buffer.endRenderPass();
                    buffer.end();
                    {
                        vk::PipelineStageFlags stageFlag = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                        vk::SubmitInfo submitInfo;
                        submitInfo.setCommandBuffers(buffer)
                            .setWaitDstStageMask(stageFlag)
                            .setWaitSemaphores(acquireSemaphore)
                            .setSignalSemaphores(waitSemaphore);
                        g_graphicsQueue.submit(submitInfo, fence);
                    }
                }
            }
            // present contexts
            if (!g_rebuildSwapchain)
            {
                vk::PresentInfoKHR presentInfo;
                presentInfo.setWaitSemaphores(waitSemaphore)
                    .setImageIndices(g_mainWindow.FrameIndex)
                    .setSwapchains(swapchain);
                g_graphicsQueue.presentKHR(presentInfo);
            }
        }
    }

    g_device.waitIdle();
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    ImGui_ImplVulkanH_DestroyWindow(g_instance, g_device, &g_mainWindow, reinterpret_cast<const VkAllocationCallbacks *>(&g_allocCallbacks));
    {
        g_device.destroy(g_descriptorPool, g_allocCallbacks);
#ifdef _DEBUG
        g_instance.destroy(g_debugReporter, g_allocCallbacks);
#endif
        g_device.destroy(g_allocCallbacks);
        g_instance.destroy(g_allocCallbacks);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}