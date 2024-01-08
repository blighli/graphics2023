#define IMGUI_DEFINE_MATH_OPERATORS
#define _USE_MATH_DEFINES
#include <nvh/cameramanipulator.hpp>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "application.h"

static void onErrorCallback(int error, const char *description)
{
    fprintf(stderr, "ImGUI(GLFW) Error %d: %s\n", error, description);
}

int main(int argc, char **argv)
{
    // init window
    glfwSetErrorCallback(onErrorCallback);
    if (glfwInit() == GLFW_FALSE)
        return 1;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(renderWidth, renderHeight, PROJECT_NAME, nullptr, nullptr);
    if (glfwVulkanSupported() == GLFW_FALSE)
    {
        printf("GLFW: Vulkan is not supported on this device.\n");
        return 1;
    }

    // setup camera
    Scene::getInstance().getMainCamera().setLookat({.0f, .0f, -5.f}, {.0f, .0f, .0f}, {.0f, 1.f, .0f});
    Scene::getInstance().getMainCamera().setClipPlanes({.01f, 1000.f});
    Scene::getInstance().getMainCamera().setFov(90.f);
    Scene::getInstance().getMainCamera().setWindowSize(renderWidth, renderHeight);

    // get required surface extensions
    uint32_t extCount{0};
    auto extensions = glfwGetRequiredInstanceExtensions(&extCount);

#ifdef _DEBUG
    nvvk::ContextCreateInfo deviceInfo;
#else
    nvvk::ContextCreateInfo deviceInfo(false);
#endif
    deviceInfo.setVersion(1, 3);
    for (uint32_t i = 0; i < extCount; ++i)
        deviceInfo.addInstanceExtension(extensions[i]);
    deviceInfo.addDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    nvvk::Context context;
    context.init(deviceInfo);

    Application app;
    nvvk::ProfilerVK profiler;
    profiler.init(context.m_device, context.m_physicalDevice, context.m_queueGCT);

    // setup surface
    const VkSurfaceKHR surface = app.getVkSurface(context.m_instance, window);
    context.setGCTQueueWithPresent(surface);
    app.setupGlfwCallbacks(window);

    // setup application
    app.setup(context);
    app.createSwapchain(surface, renderWidth, renderHeight);
    app.createDepthBuffer();
    app.createRenderPass();
    app.createFrameBuffers();

    // init ImGUI & render
    app.initGUI();
    app.createRenderer();
    ImGui_ImplGlfw_InitForVulkan(window, true);

    // window main loop
    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        glfwPollEvents();
        if (app.isMinimized())
            continue;

        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        profiler.beginFrame();
        app.prepareFrame();

        auto curFrame = app.getCurFrame();
        const VkCommandBuffer &cmdBuffer = app.getCommandBuffers()[curFrame];

        VkCommandBufferBeginInfo cmdBufferBeginInfo = nvvk::make<VkCommandBufferBeginInfo>();
        cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);

        app.renderGUI(profiler);

        app.updateBuffers(cmdBuffer);

        // rendering...
        {
            auto sec = profiler.timeRecurring("rendering", cmdBuffer);
            app.render(cmdBuffer, profiler);
        }

        // final blit in swapchain framebuffer
        {
            auto sec = profiler.timeRecurring("final blit", cmdBuffer);

            std::array<VkClearValue, 2> clearValues;
            clearValues[0].color = {{.0f, .0f, .0f, .0f}};
            clearValues[1].depthStencil = {1.f, 0};

            VkRenderPassBeginInfo renderpassBeginInfo = nvvk::make<VkRenderPassBeginInfo>();
            renderpassBeginInfo.clearValueCount = 2;
            renderpassBeginInfo.pClearValues = clearValues.data();
            renderpassBeginInfo.renderPass = app.getRenderPass();
            renderpassBeginInfo.framebuffer = app.getFramebuffers()[curFrame];
            renderpassBeginInfo.renderArea = {{}, app.getSize()};

            vkCmdBeginRenderPass(cmdBuffer, &renderpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            app.finalBlit(cmdBuffer, profiler);

            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);

            vkCmdEndRenderPass(cmdBuffer);
        }

        profiler.endFrame();

        vkEndCommandBuffer(cmdBuffer);
        app.submitFrame();

        CameraManip.updateAnim();
    }

    vkDeviceWaitIdle(app.getDevice());
    app.destroyResources();
    app.destroy();
    profiler.deinit();
    context.deinit();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}