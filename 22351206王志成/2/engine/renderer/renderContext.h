#pragma once

#include <unordered_map>
#include <vector>
#include <optional>
#include <iostream>

#include <simple_vulkan_packing.h>
#include <imageLoader.hpp>
#include "resourceCache.h"

struct RenderContext
{
public:
    RenderContext() = default;
    RenderContext(const std::vector<const char *> &instanceExtensions, const std::vector<const char *> &instanceLayers,
                  const std::vector<const char *> &deviceExtensions, const std::vector<const char *> &deviceLayers);
    ~RenderContext() { destroy(); }

    void init(const std::vector<const char *> &instanceExtensions, const std::vector<const char *> &instanceLayers,
              const std::vector<const char *> &deviceExtensions, const std::vector<const char *> &deviceLayers);

    Buffer createVertexBuffer(const std::vector<float> &vertices);
    Buffer createIndexBuffer(const std::vector<uint32_t> &indices);
    Texture createTexture(ImageDescriptor desc);

    std::shared_ptr<vk::Instance> getInstanceHandle() const noexcept { return m_instanceHandle; }
    std::shared_ptr<vk::PhysicalDevice> getAdapterHandle() const noexcept { return m_adapterHandle; }
    std::shared_ptr<vk::Device> getDeviceHandle() const noexcept { return m_deviceHandle; }
    std::shared_ptr<QueueInstance> getQueueInstanceHandle(vk::QueueFlagBits type, bool mustSeparate = true) const;
    std::shared_ptr<vk::PipelineCache> getPipelineCacheHandle() const noexcept { return m_resourceCacheTable.getPipelineCacheHandle(); }

    void destroyBuffer(Buffer &obj) { m_resourceAllocator.destroy(obj); }
    void destroyTexture(Texture &obj) { m_resourceAllocator.destroy(obj); }

    void destroy();

private:
    std::shared_ptr<vk::Instance> m_instanceHandle;
    std::shared_ptr<vk::PhysicalDevice> m_adapterHandle;
    std::shared_ptr<vk::Device> m_deviceHandle;

    QueueInstance m_graphicsQueueHandle;
    std::optional<QueueInstance> m_computeQueueHandle{};
    std::optional<QueueInstance> m_transferQueueHandle{};

    vk::CommandPool m_internalTransferPool{};
    vk::CommandBuffer m_internalTransferBuffer{};
    vk::Fence m_internalTransferFence{};

    ResourceAllocator m_resourceAllocator;
    ResourceCacheTable m_resourceCacheTable;

#ifdef NDEBUG
#if defined(VK_EXT_debug_utils)
    vk::DebugUtilsMessengerEXT m_debugMessenger{VK_NULL_HANDLE};
#else if defined(VK_EXT_debug_report)
    vk::DebugReportCallbackEXT m_debugReporter{VK_NULL_HANDLE};
#endif
#endif
};