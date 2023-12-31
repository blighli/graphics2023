#pragma once

#include <initializer_list>

#include <imgui.h>

#include <renderContext.h>

class ImGuiWindow;

class ImGuiRenderer
{
public:
    ImGuiRenderer(RenderContext &renderContext, vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e1);
    ~ImGuiRenderer();

    void initPresenter(const std::initializer_list<vk::Format> &requiredFormats,
                       const std::initializer_list<vk::PresentModeKHR> &requiredPresentModes,
                       const vk::ColorSpaceKHR &requiredColorSpace);
    void recreatePresenter(const uint32_t &width, const uint32_t &height, uint32_t minBufferCount = 0U);

    void render();

    friend class ImGuiWindow;

private:
    void clearFrameData();

    RenderContext *m_renderContextHandle{nullptr};

    /* presenting */
    vk::SurfaceKHR m_surface{VK_NULL_HANDLE};
    vk::SurfaceFormatKHR m_surfaceFormat{};
    vk::PresentModeKHR m_surfacePresentMode{};

    vk::SwapchainKHR m_swapchain{VK_NULL_HANDLE};

    vk::RenderPass m_renderPass{VK_NULL_HANDLE};

    uint32_t m_renderWidth{0U};
    uint32_t m_renderHeight{0U};
    uint32_t m_frameIndex{0U};
    std::vector<vk::CommandPool> m_commandPools{};
    std::vector<vk::CommandBuffer> m_commandBuffers{};
    std::vector<vk::Fence> m_fences{};
    std::vector<vk::Image> m_backBuffers{};
    std::vector<vk::ImageView> m_backBufferViews{};
    std::vector<vk::Framebuffer> m_framebuffers{};
    std::vector<vk::Semaphore> m_imageAcquireSemaphores{};
    std::vector<vk::Semaphore> m_renderCompleteSemaphores{};
    bool m_dirtySwapchain{false};

    /* imgui content rendering */
    vk::Sampler m_fontSampler{VK_NULL_HANDLE};
    vk::Image m_fontImage{VK_NULL_HANDLE};
    vk::ImageView m_fontImageView{VK_NULL_HANDLE};
    vk::DescriptorSetLayout m_descSetLayout{VK_NULL_HANDLE};
    vk::PipelineLayout m_pipelineLayout{VK_NULL_HANDLE};
    vk::Pipeline m_pipeline{VK_NULL_HANDLE};
    vk::SampleCountFlagBits m_msaaSamples{vk::SampleCountFlagBits::e1};
};