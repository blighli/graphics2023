#pragma once

#include "memoryAllocator.hpp"
#include "samplerPool.hpp"
// #include "stagingHelper.hpp"

struct Buffer;
struct Image;
struct Texture;

class ResourceAllocator
{
public:
    ResourceAllocator(ResourceAllocator const &) = delete;
    ResourceAllocator &operator=(ResourceAllocator const &) = delete;

    ResourceAllocator() = default;
    ResourceAllocator(std::shared_ptr<vk::Instance> instance,
                      std::shared_ptr<vk::PhysicalDevice> adapter,
                      std::shared_ptr<vk::Device> device,
                      vk::DeviceSize stagingBlockSize = vk::DeviceSize(64) * 1024 * 1024)
    {
        init(instance, adapter, device, stagingBlockSize);
    }

    void init(std::shared_ptr<vk::Instance> instance,
              std::shared_ptr<vk::PhysicalDevice> adapter,
              std::shared_ptr<vk::Device> device,
              vk::DeviceSize stagingBlockSize = vk::DeviceSize(64) * 1024 * 1024)
    {
        m_adapterHandle = adapter;
        m_deviceHandle = device;
        m_samplerPool.init(m_deviceHandle);

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = *m_adapterHandle;
        allocatorInfo.device = *m_deviceHandle;
        allocatorInfo.instance = *instance;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vmaCreateAllocator(&allocatorInfo, &m_vma);

        m_memAlloc = std::make_unique<MemoryAllocator>(adapter, device, m_vma);
        m_adapterHandle->getMemoryProperties(&m_memoryProperties);

        // m_staging = std::make_unique<StagingHelper>(m_memAlloc, stagingBlockSize);
    }

    // All staging buffers must be cleared before
    ~ResourceAllocator()
    {
        // m_staging.reset();
        m_memAlloc.reset();
        vmaDestroyAllocator(m_vma);
        m_vma = nullptr;
    }

    MemoryAllocator *getMemoryAllocator() { return m_memAlloc.get(); }

    //--------------------------------------------------------------------------------------------------
    // Basic buffer creation
    Buffer createBuffer(const vk::BufferCreateInfo &info_,
                        const vk::MemoryPropertyFlags memUsage_ = vk::MemoryPropertyFlagBits::eDeviceLocal);

    //--------------------------------------------------------------------------------------------------
    // Simple buffer creation
    // implicitly sets VK_BUFFER_USAGE_TRANSFER_DST_BIT
    Buffer createBuffer(vk::DeviceSize size_ = 0,
                        vk::BufferUsageFlags usage_ = vk::BufferUsageFlags(),
                        const vk::MemoryPropertyFlags memUsage_ = vk::MemoryPropertyFlagBits::eDeviceLocal);

    // //--------------------------------------------------------------------------------------------------
    // // Simple buffer creation with data uploaded through staging manager
    // // implicitly sets VK_BUFFER_USAGE_TRANSFER_DST_BIT
    // Buffer createBuffer(const vk::CommandBuffer &cmdBuf,
    //                     const vk::DeviceSize &size_,
    //                     const void *data_,
    //                     vk::BufferUsageFlags usage_,
    //                     const vk::MemoryPropertyFlags memUsage_ = vk::MemoryPropertyFlagBits::eDeviceLocal);

    // //--------------------------------------------------------------------------------------------------
    // // Simple buffer creation with data uploaded through staging manager
    // // implicitly sets VK_BUFFER_USAGE_TRANSFER_DST_BIT
    // template <typename T>
    // Buffer createBuffer(const vk::CommandBuffer &cmdBuf,
    //                     const std::vector<T> &data_,
    //                     vk::BufferUsageFlags usage_,
    //                     vk::MemoryPropertyFlags memUsage_ = vk::MemoryPropertyFlagBits::eDeviceLocal)
    // {
    //     return createBuffer(cmdBuf, sizeof(T) * data_.size(), data_.data(), usage_, memProps_);
    // }

    //--------------------------------------------------------------------------------------------------
    // Basic image creation
    Image createImage(const vk::ImageCreateInfo &info_, const vk::MemoryPropertyFlags memUsage_ = vk::MemoryPropertyFlagBits::eDeviceLocal);

    // //--------------------------------------------------------------------------------------------------
    // // Create an image with data uploaded through staging manager
    // Image createImage(const vk::CommandBuffer &cmdBuf,
    //                   size_t size_,
    //                   const void *data_,
    //                   const vk::ImageCreateInfo &info_,
    //                   const vk::ImageLayout &layout_ = vk::ImageLayout::eShaderReadOnlyOptimal);

    //--------------------------------------------------------------------------------------------------
    // other variants could exist with a few defaults but we already have nvvk::makeImage2DViewCreateInfo()
    // we could always override viewCreateInfo.image
    Texture createTexture(const Image &image, const vk::ImageViewCreateInfo &imageViewCreateInfo);
    Texture createTexture(const Image &image, const vk::ImageViewCreateInfo &imageViewCreateInfo, const vk::SamplerCreateInfo &samplerCreateInfo);

    // //--------------------------------------------------------------------------------------------------
    // // shortcut that creates the image for the texture
    // // - creates the image
    // // - creates the texture part by associating image and sampler
    // //
    // Texture createTexture(const vk::CommandBuffer &cmdBuf,
    //                       size_t size_,
    //                       const void *data_,
    //                       const vk::ImageCreateInfo &info_,
    //                       const vk::SamplerCreateInfo &samplerCreateInfo,
    //                       const vk::ImageLayout &layout_ = vk::ImageLayout::eShaderReadOnlyOptimal,
    //                       bool isCube = false);

    //--------------------------------------------------------------------------------------------------
    // Acquire a sampler with the provided information
    // Every acquire must have an appropriate release for appropriate internal reference counting
    vk::Sampler acquireSampler(const vk::SamplerCreateInfo &info) { return m_samplerPool.acquireSampler(info); }
    void releaseSampler(vk::Sampler sampler) { m_samplerPool.releaseSampler(sampler); }

    //--------------------------------------------------------------------------------------------------
    // implicit staging operations triggered by create are managed here
    void finalizeStaging(vk::Fence fence = {});
    void finalizeAndReleaseStaging(vk::Fence fence = {});
    void releaseStaging();

    // StagingHelper *getStaging() { m_staging.get(); }
    // const StagingHelper *getStaging() const { m_staging.get(); }

    //--------------------------------------------------------------------------------------------------
    // Destroy
    //
    void destroy(Buffer &b_);
    void destroy(Image &i_);
    void destroy(Texture &t_);

    //--------------------------------------------------------------------------------------------------
    // Other
    //
    void *map(const Buffer &buffer);
    void unmap(const Buffer &buffer);
    void *map(const Image &image);
    void unmap(const Image &image);

protected:
    // If necessary, these can be overriden to specialize the allocation, for instance to
    // enforce allocation of exportable
    MemoryHandle AllocateMemory(const MemoryAllocateInfo &allocateInfo);
    void CreateBufferEx(const vk::BufferCreateInfo &info_, vk::Buffer &buffer);
    void CreateImageEx(const vk::ImageCreateInfo &info_, vk::Image &image);

    //--------------------------------------------------------------------------------------------------
    // Finding the memory type for memory allocation
    //
    uint32_t getMemoryType(uint32_t typeBits, const VkMemoryPropertyFlags &properties);

    std::shared_ptr<vk::Device> m_deviceHandle;
    std::shared_ptr<vk::PhysicalDevice> m_adapterHandle;
    vk::PhysicalDeviceMemoryProperties m_memoryProperties{};
    VmaAllocator m_vma{nullptr};
    std::unique_ptr<MemoryAllocator> m_memAlloc;
    // std::unique_ptr<StagingHelper> m_staging;
    SamplerPool m_samplerPool;
};