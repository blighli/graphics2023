#pragma once

#include <cstdalign>

static const char *gVulkanInternalMemoryPoolNames[] = {
    "vulkan::command(internal)",
    "vulkan::object(internal)",
    "vulkan::cache(internal)",
    "vulkan::device(internal)",
    "vulkan::instance(internal)"};

static const char *gVulkanMemoryPoolNames[] = {
    "vulkan::command",
    "vulkan::object",
    "vulkan::cache",
    "vulkan::device",
    "vulkan::instance"};

struct AllocHeader
{
    int16_t padding;
    int16_t scope;
    uint32_t alignment;
};

#define _FIND_ALIGNED_SIZE(size, alignment) ((size + alignment - 1) & (~(alignment - 1)))

static void VKAPI_PTR vulkanInternalAllocNotify(void *pUserData, size_t size, VkInternalAllocationType allocType, VkSystemAllocationScope allocScope)
{
    if (allocScope > 4)
    {
        printf("WARNING: Try allocating memory on unknown scope.\n");
        return;
    }
    if (allocType != VK_INTERNAL_ALLOCATION_TYPE_EXECUTABLE)
    {
        printf("WARNING: Try allocating memory on unknown position instead of host.\n");
        return;
    }
    printf("Allocated %llu bytes memory under scope %s on host.\n", size, gVulkanInternalMemoryPoolNames[allocScope]);
}

static void VKAPI_PTR vulkanInternalFreeNotify(void *pUserData, size_t size, VkInternalAllocationType allocType, VkSystemAllocationScope allocScope)
{
    if (allocScope > 4)
    {
        printf("WARNING: Try allocating memory on unknown scope.\n");
        return;
    }
    if (allocType != VK_INTERNAL_ALLOCATION_TYPE_EXECUTABLE)
    {
        printf("WARNING: Try allocating memory on unknown position instead of host.\n");
        return;
    }
    printf("Free %llu bytes memory under scope %s on host.\n", size, gVulkanInternalMemoryPoolNames[allocScope]);
}

static void *VKAPI_PTR vulkanAlloc(void *pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocScope)
{
    if (size == 0)
        return nullptr;

    char *ptr = nullptr;
    alignment = (alignment > 0) ? alignment : alignof(AllocHeader);
    size_t alignedSize = _FIND_ALIGNED_SIZE(size, alignment);
    size_t padding = _FIND_ALIGNED_SIZE(sizeof(AllocHeader), alignment);
    ptr = (char *)_aligned_malloc(alignedSize + padding, alignment);
    if (allocScope <= 4)
        printf("Allocated %llu bytes memory under scope %s(address = 0x%llx).\n", size, gVulkanMemoryPoolNames[allocScope], (size_t)ptr);
    else
        printf("WARNING: Try Allocating %llu bytes memory under unknown scope(address = 0x%llx).\n", size, (size_t)ptr);
    // | AllocHeader | AllocatedMemory | <--- ptr
    // AllocatedMemory <--- res
    char *res = (char *)(ptr + padding);
    AllocHeader *pHeader = (AllocHeader *)(res - sizeof(AllocHeader));
    pHeader->padding = (int16_t)padding;
    pHeader->alignment = (uint32_t)alignment;
    pHeader->scope = (int16_t)allocScope;

    return res;
}

static void VKAPI_PTR vulkanFree(void *pUserData, void *pMemory)
{
    if (pMemory == nullptr)
        return;

    AllocHeader *pHeader = (AllocHeader *)((char *)pMemory - sizeof(AllocHeader));
    if (pHeader->scope <= 4)
        printf("Free memory under scope %s(address = 0x%llx).\n", gVulkanMemoryPoolNames[pHeader->scope], size_t((char *)pMemory - pHeader->padding));
    else
        printf("WARNING: Try Freeing memory under unknown scope(address = 0x%llx).\n", size_t((char *)pMemory - pHeader->padding));
    _aligned_free((void *)((char *)pMemory - pHeader->padding));
}

static void *VKAPI_PTR vulkanRealloc(void *pUserData, void *pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocScope)
{
    if (pOriginal == nullptr || size == 0)
        return nullptr;

    AllocHeader *pHeader = (AllocHeader *)((char *)pOriginal - sizeof(AllocHeader));
    alignment = (alignment > 0) ? alignment : alignof(AllocHeader);
    size_t alignedSize = _FIND_ALIGNED_SIZE(size, alignment);
    size_t padding = _FIND_ALIGNED_SIZE(sizeof(AllocHeader), alignment);
    char *ptr = (char *)_aligned_realloc((char *)pOriginal - pHeader->padding, alignedSize + padding, alignment);
    if (allocScope <= 4)
        printf("Reallocated %llu bytes memory under scope %s(address = 0x%llx).\n", size, gVulkanMemoryPoolNames[allocScope], (size_t)ptr);
    else
        printf("WARNING: Try Reallocating %llu bytes memory under unknown scope(address = 0x%llx).\n", size, (size_t)ptr);
    // | AllocHeader | AllocatedMemory | <--- ptr
    // AllocatedMemory <--- res
    char *res = (char *)(ptr + padding);
    pHeader = (AllocHeader *)(res - sizeof(AllocHeader));
    pHeader->padding = (int16_t)padding;
    pHeader->alignment = (uint32_t)alignment;
    pHeader->scope = (int16_t)allocScope;

    return res;
}

static vk::AllocationCallbacks allocationCallbacks{nullptr, &vulkanAlloc, &vulkanRealloc, &vulkanFree, &vulkanInternalAllocNotify, &vulkanInternalFreeNotify};