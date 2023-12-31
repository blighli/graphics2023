#pragma once

#include <memory>

#include <vma/vk_mem_alloc.h>

class MemoryHandleBase
{
public:
    MemoryHandleBase() = default;
    MemoryHandleBase(const MemoryHandleBase &) = default;
    MemoryHandleBase(MemoryHandleBase &&) = default;

    VmaAllocation getAllocation() const { return m_allocation; }

private:
    MemoryHandleBase(VmaAllocation allocation) : m_allocation(allocation) {}

    friend class MemoryAllocator;

    VmaAllocation m_allocation;
};
using MemoryHandle = MemoryHandleBase *;

class MemoryAllocateInfo
{
public:
    MemoryAllocateInfo(const vk::MemoryRequirements &memReqs,                                       // determine size, alignment and memory type
                       vk::MemoryPropertyFlags memProps = vk::MemoryPropertyFlagBits::eDeviceLocal, // determine device_local, host_visible, host coherent etc...
                       bool isTilingOptimal = false                                                 // determine if the alocation is going to be used for an VK_IMAGE_TILING_OPTIMAL image
                       ) : m_memReqs(memReqs), m_memProps(memProps), m_isTilingOptimal(isTilingOptimal)
    {
    }

    // Convenience constructures that infer the allocation information from the buffer object directly
    MemoryAllocateInfo(std::shared_ptr<vk::Device> device, vk::Buffer buffer, vk::MemoryPropertyFlags memProps = vk::MemoryPropertyFlagBits::eDeviceLocal)
    {
        vk::BufferMemoryRequirementsInfo2 bufferReqs{};
        vk::MemoryDedicatedRequirements dedicatedReqs{};
        vk::MemoryRequirements2 memReqs{};
        memReqs.pNext = &dedicatedReqs;
        device->getBufferMemoryRequirements2(&bufferReqs, &memReqs);

        m_memReqs = memReqs.memoryRequirements;
        m_memProps = memProps;

        if (dedicatedReqs.requiresDedicatedAllocation)
            setDedicatedBuffer(buffer);
        setTilingOptimal(false);
    }
    // Convenience constructures that infer the allocation information from the image object directly.
    // If the driver _prefers_ a dedicated allocation for this particular image and allowDedicatedAllocation is true, a dedicated allocation will be requested.
    // If the driver _requires_ a dedicated allocation, a dedicated allocation will be requested regardless of 'allowDedicatedAllocation'.
    MemoryAllocateInfo(std::shared_ptr<vk::Device> device, vk::Image image, vk::MemoryPropertyFlags memProps = vk::MemoryPropertyFlagBits::eDeviceLocal, bool allowDedicatedAllocation = true)
    {
        vk::ImageMemoryRequirementsInfo2 imageReqs{};
        vk::MemoryDedicatedRequirements dedicatedReqs{};
        vk::MemoryRequirements2 memReqs{};
        memReqs.pNext = &dedicatedReqs;
        device->getImageMemoryRequirements2(&imageReqs, &memReqs);

        m_memReqs = memReqs.memoryRequirements;
        m_memProps = memProps;

        if (dedicatedReqs.requiresDedicatedAllocation || (dedicatedReqs.prefersDedicatedAllocation && allowDedicatedAllocation))
            setDedicatedImage(image);
        setTilingOptimal(true);
    }

    // Determines which heap to allocate from
    MemoryAllocateInfo &setMemoryProperties(vk::MemoryPropertyFlags flags)
    {
        m_memProps = flags;
        return *this;
    }
    // Determines size and alignment
    MemoryAllocateInfo &setMemoryRequirements(vk::MemoryRequirements requirements)
    {
        m_memReqs = requirements;
        return *this;
    }
    // TilingOptimal should be set for images. The allocator may choose to separate linear and tiling allocations
    MemoryAllocateInfo &setTilingOptimal(bool isTilingOptimal)
    {
        m_isTilingOptimal = isTilingOptimal;
        return *this;
    }
    // The allocation will be dedicated for the given image
    MemoryAllocateInfo &setDedicatedImage(vk::Image image)
    {
        assert(!m_dedicatedImage);
        m_dedicatedImage = image;
        return *this;
    }
    // The allocation will be dedicated for the given buffer
    MemoryAllocateInfo &setDedicatedBuffer(vk::Buffer buffer)
    {
        assert(!m_dedicatedBuffer);
        m_dedicatedBuffer = buffer;
        return *this;
    }
    // Set additional allocation flags
    MemoryAllocateInfo &setAllocationFlags(vk::MemoryAllocateFlags flags)
    {
        m_allocateFlags |= flags;
        return *this;
    }
    // Set the device mask for the allocation, redirect allocations to specific device(s) in the device group
    MemoryAllocateInfo &setDeviceMask(uint32_t mask)
    {
        m_deviceMask = mask;
        return *this;
    }
    // Set a name for the allocation (only useful for dedicated allocations or allocators)
    MemoryAllocateInfo &setDebugName(const std::string &name)
    {
        m_debugName = name;
        return *this;
    }
    // Make the allocation exportable
    MemoryAllocateInfo &setExportable(bool exportable)
    {
        m_isExportable = exportable;
        return *this;
    }
    // Prioritize the allocation (values 0.0 - 1.0); this may guide eviction strategies
    MemoryAllocateInfo &setPriority(const float priority = 0.5f)
    {
        m_priority = priority;
        return *this;
    }

    vk::Image getDedicatedImage() const { return m_dedicatedImage; }
    vk::Buffer getDedicatedBuffer() const { return m_dedicatedBuffer; }
    vk::MemoryAllocateFlags getAllocationFlags() const { return m_allocateFlags; }
    uint32_t getDeviceMask() const { return m_deviceMask; }
    bool getTilingOptimal() const { return m_isTilingOptimal; }
    const vk::MemoryRequirements &getMemoryRequirements() const { return m_memReqs; }
    const vk::MemoryPropertyFlags &getMemoryProperties() const { return m_memProps; }
    std::string getDebugName() const { return m_debugName; }
    bool getExportable() const { return m_isExportable; }
    float getPriority() const { return m_priority; }

private:
    vk::Buffer m_dedicatedBuffer{};
    vk::Image m_dedicatedImage{};
    vk::MemoryAllocateFlags m_allocateFlags{0};
    uint32_t m_deviceMask{0};
    vk::MemoryRequirements m_memReqs{0, 0, 0};
    vk::MemoryPropertyFlags m_memProps{0};
    float m_priority{0.5f};

    std::string m_debugName;

    bool m_isTilingOptimal{false};
    bool m_isExportable{false};
};

static inline VmaMemoryUsage vkToVmaMemoryUsage(vk::MemoryPropertyFlags flags)
{
    if ((flags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal)
        return VMA_MEMORY_USAGE_GPU_ONLY;
    else if ((flags & vk::MemoryPropertyFlagBits::eHostCoherent) == vk::MemoryPropertyFlagBits::eHostCoherent)
        return VMA_MEMORY_USAGE_CPU_ONLY;
    else if ((flags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible)
        return VMA_MEMORY_USAGE_CPU_TO_GPU;
    return VMA_MEMORY_USAGE_UNKNOWN;
}

class MemoryAllocator
{
public:
    struct MemoryInfo
    {
        vk::DeviceMemory memory;
        vk::DeviceSize offset;
        vk::DeviceSize size;
    };

public:
    MemoryAllocator() = default;
    inline explicit MemoryAllocator(std::shared_ptr<vk::PhysicalDevice> adapter, std::shared_ptr<vk::Device> device, VmaAllocator allocator)
        : m_adapterHandle(adapter), m_deviceHandle(device), m_allocator(allocator) {}
    inline ~MemoryAllocator() { m_allocator = nullptr; }

    // Allocate a piece of memory according to the requirements of allocInfo.
    inline vk::ResultValue<MemoryHandle> allocMemory(const MemoryAllocateInfo &allocInfo)
    {
        VmaAllocationCreateInfo createInfo{};
        createInfo.usage = vkToVmaMemoryUsage(allocInfo.getMemoryProperties());
        if (allocInfo.getDedicatedBuffer() || allocInfo.getDedicatedImage())
            createInfo.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        createInfo.priority = allocInfo.getPriority();

        // Not supported by VMA
        assert(!allocInfo.getExportable());
        assert(!allocInfo.getDeviceMask());

        VmaAllocationInfo allocationDetail;
        VmaAllocation allocation = nullptr;

        vk::Result result = static_cast<vk::Result>(vmaAllocateMemory(m_allocator, reinterpret_cast<const VkMemoryRequirements *>(&allocInfo.getMemoryRequirements()), &createInfo, &allocation, &allocationDetail));
#ifdef NDEBUG
        // !! VMA leaks finder!!
        // Call findLeak with the value showing in the leak report.
        // Add : #define VMA_DEBUG_LOG(format, ...) do { printf(format, __VA_ARGS__); printf("\n"); } while(false)
        //  - in the app where VMA_IMPLEMENTATION is defined, to have a leak report
        static uint64_t counter{0};
        if (counter == m_leakIndex)
        {
            bool stop_here = true;
#if defined(_MSVC_LANG)
            __debugbreak();
#elif defined(LINUX)
            raise(SIGTRAP);
#endif
        }
        if (result == vk::Result::eSuccess)
        {
            std::string allocID = std::to_string(counter++);
            vmaSetAllocationName(m_allocator, allocation, allocID.c_str());
        }
#endif
        vk::resultCheck(result, __FILE__);
        return vk::ResultValue<MemoryHandle>(result, new MemoryHandleBase(allocation));
    }

    // Free the memory backing 'memHandle'.
    // memHandle may be nullptr;
    inline void freeMemory(MemoryHandle memHandle)
    {
        if (memHandle == nullptr)
            return;
        vmaFreeMemory(m_allocator, memHandle->getAllocation());
    }

    // Retrieve detailed information about 'memHandle'
    inline MemoryInfo getMemoryInfo(MemoryHandle memHandle) const
    {
        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(m_allocator, memHandle->getAllocation(), &allocInfo);

        MemoryInfo res{};
        res.memory = allocInfo.deviceMemory;
        res.offset = allocInfo.offset;
        res.size = allocInfo.size;

        return res;
    }

    // Maps device memory to system memory.
    // If 'memHandle' already refers to a suballocation 'offset' will be applied on top of the
    // suballocation's offset inside the device memory.
    inline vk::ResultValue<void *> map(MemoryHandle memHandle, vk::DeviceSize offset = 0, vk::DeviceSize size = VK_WHOLE_SIZE)
    {
        void *ptr;
        vk::Result res = static_cast<vk::Result>(vmaMapMemory(m_allocator, memHandle->getAllocation(), &ptr));
        vk::resultCheck(res, __FILE__);
        return vk::ResultValue<void *>(res, ptr);
    }

    // Unmap memHandle
    inline void unmap(MemoryHandle memHandle)
    {
        vmaUnmapMemory(m_allocator, memHandle->getAllocation());
    }

    // Convenience function to allow mapping straight to a typed pointer.
    template <class T>
    vk::ResultValue<T *> mapT(MemoryHandle memHandle)
    {
        return static_cast<vk::ResultValue<T *>>(map(memHandle));
    }

    std::shared_ptr<vk::Device> getDeviceHandle() const { return m_deviceHandle; }

private:
    VmaAllocator m_allocator{nullptr};
    std::shared_ptr<vk::Device> m_deviceHandle;
    std::shared_ptr<vk::PhysicalDevice> m_adapterHandle;
    uint64_t m_leakIndex{~0U};
};