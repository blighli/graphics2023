#pragma once

#include <iostream>

#ifdef NDEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags,
                                                   VkDebugReportObjectTypeEXT objectType,
                                                   uint64_t object,
                                                   size_t location,
                                                   int32_t messageCode,
                                                   const char *pLayerPrefix,
                                                   const char *pMessage,
                                                   void *pUserData)
{
    std::cerr << "[Vulkan] Debug report message: " << pMessage << "(from object type: " << objectType << ")\n";
    return VK_FALSE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_message(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                    void *pUserData)
{
    std::string_view severityStr = [&]()
    {
        switch (messageSeverity)
        {
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            return "Verbose";
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            return "Info";
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            return "Warning";
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            return "Error";
        default:
            return "Unknown";
        }
    }();
    std::string_view typeStr = [&]()
    {
        std::string res;
        if (messageTypes & VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
        {
            if (!res.empty())
                res += " | ";
            res += "General";
        }
        if (messageTypes & VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
        {
            if (!res.empty())
                res += " | ";
            res += "Performance";
        }
        if (messageTypes & VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT)
        {
            if (!res.empty())
                res += " | ";
            res += "Device address binding";
        }
        if (messageTypes & VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
        {
            if (!res.empty())
                res += " | ";
            res += "Validation";
        }
        return res;
    }();
    std::cerr << "[vulkan] Debug message(" << severityStr << ", " << typeStr << "): ";
    std::cerr << pCallbackData->pMessage << "(id: " << pCallbackData->messageIdNumber << ", name: " << pCallbackData->pMessageIdName << "). ";
    std::cerr << "From commandbuffers ";
    for (auto i = 0; i < pCallbackData->cmdBufLabelCount; ++i)
    {
        const auto info = pCallbackData->pCmdBufLabels[i];
        std::cerr << info.pLabelName;
        if (i != pCallbackData->cmdBufLabelCount - 1)
            std::cerr << ", ";
    }
    std::cerr << " in queue ";
    for (auto i = 0; i < pCallbackData->queueLabelCount; ++i)
    {
        const auto info = pCallbackData->pQueueLabels[i];
        std::cerr << info.pLabelName;
        if (i != pCallbackData->queueLabelCount - 1)
            std::cerr << ", ";
    }
    std::cerr << "Associated objects: ";
    for (auto i = 0; i < pCallbackData->objectCount; ++i)
    {
        const auto info = pCallbackData->pObjects[i];
        std::cerr << info.objectType << " " << info.pObjectName << " " << info.objectHandle;
        if (i != pCallbackData->objectCount - 1)
            std::cerr << ", ";
    }
    return VK_FALSE;
}
#endif