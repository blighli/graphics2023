#include <exception>

#include "renderContext.h"

#ifdef NDEBUG
#if defined(VK_EXT_debug_utils)
PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                              const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                              const VkAllocationCallbacks *pAllocator,
                                                              VkDebugUtilsMessengerEXT *pMessenger)
{
    return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}
VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, VkAllocationCallbacks const *pAllocator)
{
    return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}
#elif defined(VK_EXT_debug_report)
PFN_vkCreateDebugReportCallbackEXT pfnVkCreateDebugReportCallbackEXT;
PFN_vkDestroyDebugReportCallbackEXT pfnVkDestroyDebugReportCallbackEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(VkInstance instance,
                                                              const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
                                                              const VkAllocationCallbacks *pAllocator,
                                                              VkDebugReportCallbackEXT *pCallback)
{
    return pfnVkCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
}
VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(VkInstance instance,
                                                           VkDebugReportCallbackEXT callback,
                                                           const VkAllocationCallbacks *pAllocator)
{
    return pfnVkDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
}
#endif
#endif

RenderContext::RenderContext(const std::vector<const char *> &instanceExtensions, const std::vector<const char *> &instanceLayers,
                             const std::vector<const char *> &deviceExtensions, const std::vector<const char *> &deviceLayers)
{
    init(instanceExtensions, instanceLayers, deviceExtensions, deviceLayers);
}

void RenderContext::init(const std::vector<const char *> &instanceExtensions, const std::vector<const char *> &instanceLayers,
                         const std::vector<const char *> &deviceExtensions, const std::vector<const char *> &deviceLayers)
{
    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_API_VERSION_1_3);
    vk::InstanceCreateInfo instanceCreateInfo;
    std::vector<const char *> _instanceExtensions{instanceExtensions};
    std::vector<const char *> _instanceLayers{instanceLayers};

#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
    auto instanceExtProperties = vk::enumerateInstanceExtensionProperties();
    for (const auto &property : instanceExtProperties)
    {
        if (property.extensionName == VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)
        {
            instanceCreateInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
            _instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        }
    }
#endif
#ifdef NDEBUG
#if defined(VK_EXT_debug_utils)
    _instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    _instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
#else if defined(VK_EXT_debug_report)
    _instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    _instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif
#endif
    instanceCreateInfo.setPApplicationInfo(&appInfo)
        .setPEnabledExtensionNames(_instanceExtensions)
        .setPEnabledLayerNames(_instanceLayers);
    m_instanceHandle = std::make_shared<vk::Instance>(vk::createInstance(instanceCreateInfo, allocationCallbacks));

#ifdef NDEBUG
#if defined(VK_EXT_debug_utils)
    pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(m_instanceHandle->getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (!pfnVkCreateDebugUtilsMessengerEXT)
    {
        std::cerr << "GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function." << std::endl;
        abort();
    }

    pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(m_instanceHandle->getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (!pfnVkDestroyDebugUtilsMessengerEXT)
    {
        std::cerr << "GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function." << std::endl;
        abort();
    }

    vk::DebugUtilsMessengerCreateInfoEXT messengerCreateInfo;
    messengerCreateInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    messengerCreateInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    messengerCreateInfo.setPfnUserCallback(debug_message);
    messengerCreateInfo.setPUserData(nullptr);
    m_debugMessenger = m_instanceHandle->createDebugUtilsMessengerEXT(messengerCreateInfo, allocationCallbacks);
#else if defined(VK_EXT_debug_report)
    pfnVkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(m_instanceHandle->getProcAddr("vkCreateDebugReportCallbackEXT"));
    if (!pfnVkCreateDebugReportCallbackEXT)
    {
        std::cerr << "GetInstanceProcAddr: Unable to find pfnVkCreateDebugReportCallbackEXT function." << std::endl;
        abort();
    }

    pfnVkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(m_instanceHandle->getProcAddr("vkDestroyDebugReportCallbackEXT"));
    if (!pfnVkDestroyDebugReportCallbackEXT)
    {
        std::cerr << "GetInstanceProcAddr: Unable to find pfnVkDestroyDebugReportCallbackEXT function." << std::endl;
        abort();
    }

    vk::DebugReportCallbackCreateInfoEXT callbackCreateInfo;
    callbackCreateInfo.setFlags(vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning | vk::DebugReportFlagBitsEXT::eError);
    callbackCreateInfo.setPfnCallback(debug_report);
    callbackCreateInfo.setPUserData(nullptr);
    m_debugReporter = m_instanceHandle->createDebugReportCallbackEXT(callbackCreateInfo, allocationCallbacks);
#endif
#endif

    auto adapters = m_instanceHandle->enumeratePhysicalDevices();
    for (const auto &adapter : adapters)
    {
        auto property = adapter.getProperties2();
        if (property.properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            m_adapterHandle = std::make_shared<vk::PhysicalDevice>(adapter);
    }
    if (!adapters.empty() && !m_adapterHandle)
        m_adapterHandle = std::make_shared<vk::PhysicalDevice>(adapters[0]);

    auto queueFamilyProperties = m_adapterHandle->getQueueFamilyProperties2();
    for (auto i = 0; i < queueFamilyProperties.size(); ++i)
    {
        if (queueFamilyProperties[i].queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics)
            m_graphicsQueueHandle.queue_family_index = i;
        if (m_graphicsQueueHandle.queue_family_index != i &&
            (queueFamilyProperties[i].queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute))
            m_computeQueueHandle->queue_family_index = i;
        if (m_graphicsQueueHandle.queue_family_index != i &&
            m_computeQueueHandle->queue_family_index != i &&
            (queueFamilyProperties[i].queueFamilyProperties.queueFlags & vk::QueueFlagBits::eTransfer))
            m_transferQueueHandle->queue_family_index = i;
    }

    std::vector<const char *> _deviceExtensions{deviceExtensions};
    std::vector<const char *> _deviceLayers{deviceLayers};
    _deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
    auto deviceExtProperties = m_adapterHandle->enumerateDeviceExtensionProperties();
    for (const auto &property : deviceExtProperties)
    {
        if (property.extensionName == VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)
        {
            _deviceExtensions.emplace_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
        }
    }
#endif

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};
    queueCreateInfos.resize(1 + m_computeQueueHandle.has_value() + m_transferQueueHandle.has_value());
    queueCreateInfos[0].setQueueCount(1U).setQueueFamilyIndex(m_graphicsQueueHandle.queue_family_index).setQueuePriorities(m_graphicsQueueHandle.queue_priority);
    if (m_computeQueueHandle.has_value())
        queueCreateInfos[1].setQueueCount(1U).setQueueFamilyIndex(m_computeQueueHandle->queue_family_index).setQueuePriorities(m_computeQueueHandle->queue_priority);
    if (m_transferQueueHandle.has_value())
        queueCreateInfos[2].setQueueCount(1U).setQueueFamilyIndex(m_transferQueueHandle->queue_family_index).setQueuePriorities(m_transferQueueHandle->queue_priority);
    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfos)
        .setPEnabledExtensionNames(_deviceExtensions);
    if (!_deviceLayers.empty())
        deviceCreateInfo.setPEnabledLayerNames(_deviceLayers);
    m_deviceHandle = std::make_shared<vk::Device>(m_adapterHandle->createDevice(deviceCreateInfo, allocationCallbacks));
    m_graphicsQueueHandle.queue_handle = std::make_shared<vk::Queue>(m_deviceHandle->getQueue(m_graphicsQueueHandle.queue_family_index, 0U));
    if (m_computeQueueHandle.has_value())
        m_computeQueueHandle->queue_handle = std::make_shared<vk::Queue>(m_deviceHandle->getQueue(m_computeQueueHandle->queue_family_index, 0U));
    if (m_transferQueueHandle.has_value())
        m_transferQueueHandle->queue_handle = std::make_shared<vk::Queue>(m_deviceHandle->getQueue(m_transferQueueHandle->queue_family_index, 0U));

    vk::CommandPoolCreateInfo poolCreateInfo{};
    poolCreateInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(getQueueInstanceHandle(vk::QueueFlagBits::eTransfer, false)->queue_family_index);
    m_internalTransferPool = m_deviceHandle->createCommandPool(poolCreateInfo, allocationCallbacks);

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.setCommandPool(m_internalTransferPool).setCommandBufferCount(1U).setLevel(vk::CommandBufferLevel::ePrimary);
    m_internalTransferBuffer = m_deviceHandle->allocateCommandBuffers(allocInfo).front();

    vk::FenceCreateInfo fenceCreateInfo{};
    m_internalTransferFence = m_deviceHandle->createFence(fenceCreateInfo, allocationCallbacks);

    m_resourceCacheTable.init(m_deviceHandle);
    m_resourceAllocator.init(m_instanceHandle, m_adapterHandle, m_deviceHandle);
}

Buffer RenderContext::createVertexBuffer(const std::vector<float> &vertices)
{
    auto stagingBuffer = m_resourceAllocator.createBuffer(sizeof(float) * vertices.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    auto mapped = m_resourceAllocator.map(stagingBuffer);
    memcpy(mapped, vertices.data(), sizeof(float) * vertices.size());
    m_resourceAllocator.unmap(stagingBuffer);

    auto buffer = m_resourceAllocator.createBuffer(sizeof(float) * vertices.size(), vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst);
    {
        m_deviceHandle->resetCommandPool(m_internalTransferPool);
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        m_internalTransferBuffer.begin(beginInfo);

        vk::BufferCopy region{};
        region.setSize(sizeof(float) * vertices.size());
        m_internalTransferBuffer.copyBuffer(stagingBuffer.buffer, buffer.buffer, region);

        m_internalTransferBuffer.end();
        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(m_internalTransferBuffer);
        getQueueInstanceHandle(vk::QueueFlagBits::eTransfer, false)->queue_handle->submit(submitInfo, m_internalTransferFence);
        m_deviceHandle->waitForFences(m_internalTransferFence, VK_TRUE, UINT64_MAX);
    }
    m_resourceAllocator.destroy(stagingBuffer);

    return buffer;
}

Buffer RenderContext::createIndexBuffer(const std::vector<uint32_t> &indices)
{
    auto stagingBuffer = m_resourceAllocator.createBuffer(sizeof(uint32_t) * indices.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    auto mapped = m_resourceAllocator.map(stagingBuffer);
    memcpy(mapped, indices.data(), sizeof(uint32_t) * indices.size());
    m_resourceAllocator.unmap(stagingBuffer);

    auto buffer = m_resourceAllocator.createBuffer(sizeof(uint32_t) * indices.size(), vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst);
    {
        m_deviceHandle->resetCommandPool(m_internalTransferPool);
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        m_internalTransferBuffer.begin(beginInfo);

        vk::BufferCopy region{};
        region.setSize(sizeof(uint32_t) * indices.size());
        m_internalTransferBuffer.copyBuffer(stagingBuffer.buffer, buffer.buffer, region);

        m_internalTransferBuffer.end();
        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(m_internalTransferBuffer);
        getQueueInstanceHandle(vk::QueueFlagBits::eTransfer, false)->queue_handle->submit(submitInfo, m_internalTransferFence);
        m_deviceHandle->waitForFences(m_internalTransferFence, VK_TRUE, UINT64_MAX);
    }
    m_resourceAllocator.destroy(stagingBuffer);

    return buffer;
}

Texture RenderContext::createTexture(ImageDescriptor desc)
{
    // auto mipLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(desc.width, desc.height)))) + 1;
    auto mipLevel = 1;
    vk::ImageCreateInfo imageCreateInfo{};
    imageCreateInfo.setImageType(vk::ImageType::e2D)
        .setFormat(desc.format)
        .setExtent(vk::Extent3D(desc.width, desc.height, 1))
        .setMipLevels(mipLevel)
        .setArrayLayers(1)
        .setUsage(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst);
    auto image = m_resourceAllocator.createImage(imageCreateInfo);

    // copy data by staging buffer
    auto stagingBuffer = m_resourceAllocator.createBuffer(desc.size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    auto mapped = m_resourceAllocator.map(stagingBuffer);
    memcpy(mapped, desc.data, desc.size);
    m_resourceAllocator.unmap(stagingBuffer);
    {
        m_deviceHandle->resetCommandPool(m_internalTransferPool);
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        m_internalTransferBuffer.begin(beginInfo);
        cmdBarrierImageLayout(m_internalTransferBuffer, image.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        m_internalTransferBuffer.end();
        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(m_internalTransferBuffer);
        getQueueInstanceHandle(vk::QueueFlagBits::eTransfer, false)->queue_handle->submit(submitInfo, m_internalTransferFence);
        m_deviceHandle->waitForFences(m_internalTransferFence, VK_TRUE, UINT64_MAX);

        m_deviceHandle->resetCommandPool(m_internalTransferPool);
        m_internalTransferBuffer.begin(beginInfo);
        vk::BufferImageCopy region{};
        region.setImageSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1))
            .setImageExtent(vk::Extent3D(desc.width, desc.height, 1));
        m_internalTransferBuffer.copyBufferToImage(stagingBuffer.buffer, image.image, vk::ImageLayout::eTransferDstOptimal, region);
        m_internalTransferBuffer.end();
        getQueueInstanceHandle(vk::QueueFlagBits::eTransfer, false)->queue_handle->submit(submitInfo, m_internalTransferFence);
        m_deviceHandle->waitForFences(m_internalTransferFence, VK_TRUE, UINT64_MAX);
    }
    m_resourceAllocator.destroy(stagingBuffer);

    // // generate mipmaps
    // auto barrier = makeImageMemoryBarrier(image.image, vk::AccessFlagBits2::eTransferWrite, vk::AccessFlagBits2::eTransferRead, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, vk::ImageAspectFlagBits::eColor);
    // vk::DependencyInfo depInfo{};
    // depInfo.setImageMemoryBarriers(barrier);
    // vk::ImageBlit blit{};
    // blit.setSrcSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1))
    //     .setDstSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1));
    // {
    //     m_deviceHandle->resetCommandPool(m_internalTransferPool);
    //     vk::CommandBufferBeginInfo beginInfo{};
    //     beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    //     m_internalTransferBuffer.begin(beginInfo);

    //     auto mipWidth = desc.width, mipHeight = desc.height;
    //     for (auto i = 1; i < mipLevel; ++i)
    //     {
    //         barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
    //             .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
    //             .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
    //             .setDstAccessMask(vk::AccessFlagBits2::eTransferRead);
    //         barrier.subresourceRange.baseMipLevel = i - 1;
    //         m_internalTransferBuffer.pipelineBarrier2(depInfo);

    //         blit.srcOffsets[1] = vk::Offset3D{mipWidth, mipHeight, 1};
    //         blit.srcSubresource.mipLevel = i - 1;
    //         blit.dstOffsets[1] = vk::Offset3D{mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
    //         blit.dstSubresource.mipLevel = i;

    //         m_internalTransferBuffer.blitImage(image.image, vk::ImageLayout::eTransferSrcOptimal,
    //                                            image.image, vk::ImageLayout::eTransferDstOptimal,
    //                                            blit, vk::Filter::eLinear);

    //         barrier.setOldLayout(vk::ImageLayout::eTransferSrcOptimal)
    //             .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
    //             .setSrcAccessMask(vk::AccessFlagBits2::eTransferRead)
    //             .setDstAccessMask(vk::AccessFlagBits2::eTransferWrite);
    //         m_internalTransferBuffer.pipelineBarrier2(depInfo);

    //         if (mipWidth > 1)
    //             mipWidth >>= 1;
    //         if (mipHeight > 1)
    //             mipHeight >>= 1;
    //     }
    //     barrier.subresourceRange.baseMipLevel = mipLevel - 1;
    //     barrier.setOldLayout(vk::ImageLayout::eTransferSrcOptimal)
    //         .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
    //         .setSrcAccessMask(vk::AccessFlagBits2::eTransferRead)
    //         .setDstAccessMask(vk::AccessFlagBits2::eShaderRead);
    //     m_internalTransferBuffer.pipelineBarrier2(depInfo);

    //     m_internalTransferBuffer.end();
    //     vk::SubmitInfo submitInfo{};
    //     submitInfo.setCommandBuffers(m_internalTransferBuffer);
    //     getQueueInstanceHandle(vk::QueueFlagBits::eTransfer, false)->queue_handle->submit(submitInfo, m_internalTransferFence);
    //     m_deviceHandle->waitForFences(m_internalTransferFence, VK_TRUE, UINT64_MAX);
    // }

    vk::ImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.setImage(image.image)
        .setFormat(desc.format)
        .setViewType(vk::ImageViewType::e2D)
        .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 0, 1));
    vk::SamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.setMagFilter(vk::Filter::eLinear)
        .setMinFilter(vk::Filter::eLinear)
        .setMipmapMode(vk::SamplerMipmapMode::eLinear)
        .setMaxLod(mipLevel)
        .setBorderColor(vk::BorderColor::eFloatOpaqueBlack);
    return m_resourceAllocator.createTexture(image, imageViewCreateInfo, samplerCreateInfo);
}

std::shared_ptr<QueueInstance> RenderContext::getQueueInstanceHandle(vk::QueueFlagBits type, bool mustSeparate) const
{
    switch (type)
    {
    case vk::QueueFlagBits::eGraphics:
        return std::make_shared<QueueInstance>(m_graphicsQueueHandle);
    case vk::QueueFlagBits::eCompute:
        if (m_computeQueueHandle.has_value())
            return std::make_shared<QueueInstance>(m_computeQueueHandle.value());
        else if (!mustSeparate)
            return std::make_shared<QueueInstance>(m_graphicsQueueHandle);
        else
            throw std::logic_error("Failed to get compute queue instance: It has not been initialized!");
    case vk::QueueFlagBits::eTransfer:
        if (m_transferQueueHandle.has_value())
            return std::make_shared<QueueInstance>(m_transferQueueHandle.value());
        else if (!mustSeparate)
            return std::make_shared<QueueInstance>(m_graphicsQueueHandle);
        else
            throw std::logic_error("Failed to get transfer queue instance: It has not been initialized!");
    default:
        throw std::logic_error("Failed to get queue instance: Currently not supported queue type!");
    }
}

void RenderContext::destroy()
{
    if (m_deviceHandle)
    {
        m_deviceHandle->destroyFence(m_internalTransferFence, allocationCallbacks);
        m_deviceHandle->freeCommandBuffers(m_internalTransferPool, m_internalTransferBuffer);
        m_deviceHandle->destroyCommandPool(m_internalTransferPool, allocationCallbacks);
        m_deviceHandle->destroy(allocationCallbacks);
    }
#ifdef _DEBUG
#if defined(VK_EXT_debug_utils)
    m_instanceHandle->destroy(m_debugMessenger, allocationCallbacks);
#else if defined(VK_EXT_debug_report)
    m_instanceHandle->destroy(m_debugReporter, allocationCallbacks);
#endif
#endif
    if (m_instanceHandle)
        m_instanceHandle->destroy(allocationCallbacks);
}