#pragma once

#include <cstdint>

#include "memoryAllocator.hpp"

class StagingHelper
{
public:
    static const uint32_t INVALID_ID_INDEX = ~0;

    //////////////////////////////////////////////////////////////////////////
    class SetID
    {
        friend StagingHelper;

    private:
        uint32_t index = INVALID_ID_INDEX;
    };

    StagingHelper(StagingHelper const &) = delete;
    StagingHelper &operator=(StagingHelper const &) = delete;

    StagingHelper() { m_debugName = "StagingHelper:" + std::to_string((uint64_t)this); }
    StagingHelper(MemoryAllocator *memAllocator, VkDeviceSize stagingBlockSize = vk::DeviceSize(64) * 1024 * 1024)
    {
        init(memAllocator, stagingBlockSize);
    }

    virtual ~StagingHelper() { deinit(); }

    void init(MemoryAllocator *memAllocator, VkDeviceSize stagingBlockSize = vk::DeviceSize(64) * 1024 * 1024)
    {
        assert(!m_deviceHandle);
        m_deviceHandle = memAllocator->getDeviceHandle();

        m_subToDevice.init(memAllocator, stagingBlockSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);
        m_subFromDevice.init(memAllocator, stagingBlockSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
                             true);

        m_freeStagingIndex = INVALID_ID_INDEX;
        m_stagingIndex = newStagingIndex();

        setFreeUnusedOnRelease(true);
    }

    void deinit()
    {
        if (!m_deviceHandle)
            return;

        free(false);

        m_subFromDevice.deinit();
        m_subToDevice.deinit();

        m_sets.clear();
    }

    void setDebugName(const std::string &name) { m_debugName = name; }

    // if true (default) we free the memory completely when released
    // otherwise we would keep blocks for re-use around, unless freeUnused() is called
    void setFreeUnusedOnRelease(bool state)
    {
        m_subToDevice.setKeepLastBlockOnFree(!state);
        m_subFromDevice.setKeepLastBlockOnFree(!state);
    }

    // test if there is enough space in current allocations
    bool fitsInAllocated(vk::DeviceSize size, bool toDevice = true) const
    {
        return toDevice ? m_subToDevice.fitsInAllocated(size) : m_subFromDevice.fitsInAllocated(size);
    }

    // if data != nullptr memcpies to mapping and returns nullptr
    // otherwise returns temporary mapping (valid until "complete" functions)
    void *cmdToImage(vk::CommandBuffer cmd,
                     vk::Image image,
                     const VkOffset3D &offset,
                     const VkExtent3D &extent,
                     const VkImageSubresourceLayers &subresource,
                     VkDeviceSize size,
                     const void *data,
                     VkImageLayout layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    template <class T>
    T *cmdToImageT(VkCommandBuffer cmd,
                   VkImage image,
                   const VkOffset3D &offset,
                   const VkExtent3D &extent,
                   const VkImageSubresourceLayers &subresource,
                   VkDeviceSize size,
                   const void *data,
                   VkImageLayout layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        return (T *)cmdToImage(cmd, image, offset, extent, subresource, size, data, layout);
    }

    // pointer can be used after cmd execution but only valid until associated resources haven't been released
    const void *cmdFromImage(VkCommandBuffer cmd,
                             VkImage image,
                             const VkOffset3D &offset,
                             const VkExtent3D &extent,
                             const VkImageSubresourceLayers &subresource,
                             VkDeviceSize size,
                             VkImageLayout layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    template <class T>
    const T *cmdFromImageT(VkCommandBuffer cmd,
                           VkImage image,
                           const VkOffset3D &offset,
                           const VkExtent3D &extent,
                           const VkImageSubresourceLayers &subresource,
                           VkDeviceSize size,
                           VkImageLayout layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        return (const T *)cmdFromImage(cmd, image, offset, extent, subresource, size, layout);
    }

    // if data != nullptr memcpies to mapping and returns nullptr
    // otherwise returns temporary mapping (valid until appropriate release)
    void *cmdToBuffer(VkCommandBuffer cmd, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size, const void *data);

    template <class T>
    T *cmdToBufferT(VkCommandBuffer cmd, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size)
    {
        return (T *)cmdToBuffer(cmd, buffer, offset, size, nullptr);
    }

    // pointer can be used after cmd execution but only valid until associated resources haven't been released
    const void *cmdFromBuffer(VkCommandBuffer cmd, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size);

    template <class T>
    const T *cmdFromBufferT(VkCommandBuffer cmd, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size)
    {
        return (const T *)cmdFromBuffer(cmd, buffer, offset, size);
    }

    // closes the batch of staging resources since last finalize call
    // and associates it with a fence for later release.
    void finalizeResources(VkFence fence = VK_NULL_HANDLE);

    // releases the staging resources whose fences have completed
    // and those who had no fence at all, skips resourceSets.
    void releaseResources();

    // closes the batch of staging resources since last finalize call
    // and returns a resource set handle that can be used to release them
    SetID finalizeResourceSet();

    // releases the staging resources from this particular
    // resource set.
    void releaseResourceSet(SetID setid) { releaseResources(setid.index); }

    // frees staging memory no longer in use
    void freeUnused() { free(true); }

    float getUtilization(VkDeviceSize &allocatedSize, VkDeviceSize &usedSize) const;

protected:
    // The implementation uses two major arrays:
    // - Block stores VkBuffers that we sub-allocate the staging space from
    // - StagingSet stores all such sub-allocations that were used
    //   in one batch of operations. Each batch is closed with
    //   finalizeResources, and typically associated with a fence.
    //   As such the resources are given by for recycling if the fence completed.

    // To recycle StagingSet structures within the arrays
    // we use a linked list of array indices. The "index" element
    // in the struct refers to the next free list item, or itself
    // when in use.

    struct Entry
    {
        BufferSubAllocator::Handle handle;
        bool toDevice;
    };

    struct StagingSet
    {
        uint32_t index = INVALID_ID_INDEX;
        VkFence fence = VK_NULL_HANDLE;
        bool manualSet = false;
        std::vector<Entry> entries;
    };

    std::shared_ptr<vk::Device> m_deviceHandle;

    BufferSubAllocator m_subToDevice;
    BufferSubAllocator m_subFromDevice;

    std::vector<StagingSet> m_sets;

    // active staging Index, must be valid at all items
    uint32_t m_stagingIndex;
    // linked-list to next free staging set
    uint32_t m_freeStagingIndex;

    std::string m_debugName;

    uint32_t setIndexValue(uint32_t &index, uint32_t newValue)
    {
        uint32_t oldValue = index;
        index = newValue;
        return oldValue;
    }

    void free(bool unusedOnly);

    uint32_t newStagingIndex();

    void *getStagingSpace(VkDeviceSize size, VkBuffer &buffer, VkDeviceSize &offset, bool toDevice);

    void releaseResources(uint32_t stagingID);
};