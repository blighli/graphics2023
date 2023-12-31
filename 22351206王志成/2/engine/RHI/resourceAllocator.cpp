#include <vulkan/vulkan.hpp>

#include "barrierHelper.h"
#include "basicResource.h"
#include "resourceAllocator.hpp"
#include "allocationCallbacks.h"

Buffer ResourceAllocator::createBuffer(const vk::BufferCreateInfo &info_,
                                       const vk::MemoryPropertyFlags memUsage_)
{
    Buffer resultBuffer;
    // Create Buffer (can be overloaded)
    CreateBufferEx(info_, resultBuffer.buffer);

    // Find memory requirements
    vk::BufferMemoryRequirementsInfo2 bufferReqs{};
    vk::MemoryDedicatedRequirements dedicatedReqs{};
    vk::MemoryRequirements2 memReqs{};
    memReqs.pNext = &dedicatedReqs;
    bufferReqs.buffer = resultBuffer.buffer;

    m_deviceHandle->getBufferMemoryRequirements2(&bufferReqs, &memReqs);

    // Build up allocation info
    MemoryAllocateInfo allocInfo(memReqs.memoryRequirements, memUsage_, false);

    if (info_.usage & vk::BufferUsageFlagBits::eShaderDeviceAddress)
        allocInfo.setAllocationFlags(vk::MemoryAllocateFlagBits::eDeviceAddress);
    if (dedicatedReqs.requiresDedicatedAllocation)
        allocInfo.setDedicatedBuffer(resultBuffer.buffer);

    // Allocate memory
    resultBuffer.memHandle = AllocateMemory(allocInfo);
    if (resultBuffer.memHandle)
    {
        const auto memInfo = m_memAlloc->getMemoryInfo(resultBuffer.memHandle);
        // Bind memory to buffer
        m_deviceHandle->bindBufferMemory(resultBuffer.buffer, memInfo.memory, memInfo.offset);
    }
    else
        destroy(resultBuffer);

    return resultBuffer;
}

Buffer ResourceAllocator::createBuffer(vk::DeviceSize size_,
                                       vk::BufferUsageFlags usage_,
                                       const vk::MemoryPropertyFlags memUsage_)
{
    vk::BufferCreateInfo info{};
    info.setSize(size_)
        .setUsage(usage_ | vk::BufferUsageFlagBits::eTransferDst);

    return createBuffer(info, memUsage_);
}

// Buffer ResourceAllocator::createBuffer(const vk::CommandBuffer &cmdBuf,
//                                        const vk::DeviceSize &size_,
//                                        const void *data_,
//                                        vk::BufferUsageFlags usage_,
//                                        const vk::MemoryPropertyFlags memUsage_)
// {
//     vk::BufferCreateInfo info{};
//     info.setSize(size_)
//         .setUsage(usage_ | vk::BufferUsageFlagBits::eTransferDst);
//     auto resultBuffer = createBuffer(info, memUsage_);

//     if (data_)
//     {
//         m_staging->cmdToBuffer(cmdBuf, resultBuffer.buffer, 0, size_, data_);
//     }

//     return resultBuffer;
// }

Image ResourceAllocator::createImage(const vk::ImageCreateInfo &info_, const vk::MemoryPropertyFlags memUsage_)
{
    Image resultImage;
    // Create image
    CreateImageEx(info_, resultImage.image);

    // Find memory requirements
    vk::ImageMemoryRequirementsInfo2 imageReqs{};
    vk::MemoryDedicatedRequirements dedicatedReqs{};
    vk::MemoryRequirements2 memReqs{};
    memReqs.pNext = &dedicatedReqs;
    imageReqs.image = resultImage.image;

    m_deviceHandle->getImageMemoryRequirements2(&imageReqs, &memReqs);

    // Build up allocation info
    MemoryAllocateInfo allocInfo(memReqs.memoryRequirements, memUsage_, true);
    if (dedicatedReqs.requiresDedicatedAllocation)
    {
        allocInfo.setDedicatedImage(resultImage.image);
    }

    // Allocate memory
    resultImage.memHandle = AllocateMemory(allocInfo);
    if (resultImage.memHandle)
    {
        const auto memInfo = m_memAlloc->getMemoryInfo(resultImage.memHandle);
        // Bind memory to image
        m_deviceHandle->bindImageMemory(resultImage.image, memInfo.memory, memInfo.offset);
    }
    else
        destroy(resultImage);
    return resultImage;
}

// Image ResourceAllocator::createImage(const vk::CommandBuffer &cmdBuf,
//                                      size_t size_,
//                                      const void *data_,
//                                      const vk::ImageCreateInfo &info_,
//                                      const vk::ImageLayout &layout_)
// {
//     Image resultImage = createImage(info_, vk::MemoryPropertyFlagBits::eDeviceLocal);

//     // Copy the data to staging buffer than to image
//     if (data_ != nullptr)
//     {
//         // Copy buffer to image
//         vk::ImageSubresourceRange subresourceRange{vk::ImageAspectFlagBits::eColor, 0, info_.mipLevels, 0, 1};

//         // doing these transitions per copy is not efficient, should do in bulk for many images
//         cmdBarrierImageLayout(cmdBuf, resultImage.image, vk::ImageLayout::eUndefined,
//                               vk::ImageLayout::eTransferDstOptimal, subresourceRange);

//         VkOffset3D offset = {0};
//         VkImageSubresourceLayers subresource = {0};
//         subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//         subresource.layerCount = 1;

//         m_staging->cmdToImage(cmdBuf, resultImage.image, offset, info_.extent, subresource, size_, data_);

//         // Setting final image layout
//         cmdBarrierImageLayout(cmdBuf, resultImage.image, vk::ImageLayout::eTransferDstOptimal, layout_);
//     }
//     else
//         // Setting final image layout
//         cmdBarrierImageLayout(cmdBuf, resultImage.image, vk::ImageLayout::eUndefined, layout_);

//     return resultImage;
// }

Texture ResourceAllocator::createTexture(const Image &image, const vk::ImageViewCreateInfo &imageViewCreateInfo)
{
    Texture resultTexture;
    resultTexture.image = image.image;
    resultTexture.memHandle = image.memHandle;
    resultTexture.descriptor.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    assert(imageViewCreateInfo.image == image.image);
    resultTexture.descriptor.imageView = m_deviceHandle->createImageView(imageViewCreateInfo, allocationCallbacks);

    return resultTexture;
}

Texture ResourceAllocator::createTexture(const Image &image, const vk::ImageViewCreateInfo &imageViewCreateInfo, const vk::SamplerCreateInfo &samplerCreateInfo)
{
    Texture resultTexture = createTexture(image, imageViewCreateInfo);
    resultTexture.descriptor.sampler = m_samplerPool.acquireSampler(samplerCreateInfo);

    return resultTexture;
}

// Texture ResourceAllocator::createTexture(const vk::CommandBuffer &cmdBuf,
//                                          size_t size_,
//                                          const void *data_,
//                                          const vk::ImageCreateInfo &info_,
//                                          const vk::SamplerCreateInfo &samplerCreateInfo,
//                                          const vk::ImageLayout &layout_,
//                                          bool isCube)
// {
//     Image image = createImage(cmdBuf, size_, data_, info_, layout_);

//     vk::ImageViewCreateInfo viewInfo{};
//     viewInfo.setImage(image.image)
//         .setFormat(info_.format)
//         .setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS});
//     switch (info_.imageType)
//     {
//     case vk::ImageType::e1D:
//         viewInfo.viewType = (info_.arrayLayers > 1 ? vk::ImageViewType::e1DArray : vk::ImageViewType::e1D);
//         break;
//     case vk::ImageType::e2D:
//         viewInfo.viewType = isCube ? vk::ImageViewType::eCube : (info_.arrayLayers > 1 ? vk::ImageViewType::e2DArray : vk::ImageViewType::e2D);
//         break;
//     case vk::ImageType::e3D:
//         viewInfo.viewType = vk::ImageViewType::e3D;
//         break;
//     default:
//         assert(0);
//     }

//     Texture resultTexture = createTexture(image, viewInfo, samplerCreateInfo);
//     resultTexture.descriptor.imageLayout = layout_;
//     return resultTexture;
// }

void ResourceAllocator::destroy(Buffer &b_)
{
    m_deviceHandle->destroyBuffer(b_.buffer, allocationCallbacks);
    m_memAlloc->freeMemory(b_.memHandle);

    b_ = Buffer();
}

void ResourceAllocator::destroy(Image &i_)
{
    m_deviceHandle->destroyImage(i_.image, allocationCallbacks);
    m_memAlloc->freeMemory(i_.memHandle);
    i_ = Image();
}

void ResourceAllocator::destroy(Texture &t_)
{
    m_deviceHandle->destroyImageView(t_.descriptor.imageView, allocationCallbacks);
    m_deviceHandle->destroyImage(t_.image, allocationCallbacks);
    m_memAlloc->freeMemory(t_.memHandle);

    if (t_.descriptor.sampler)
    {
        m_samplerPool.releaseSampler(t_.descriptor.sampler);
    }

    t_ = Texture();
}

void *ResourceAllocator::map(const Buffer &buffer)
{
    void *pData = m_memAlloc->map(buffer.memHandle).value;
    return pData;
}
void ResourceAllocator::unmap(const Buffer &buffer)
{
    m_memAlloc->unmap(buffer.memHandle);
}
void *ResourceAllocator::map(const Image &image)
{
    void *pData = m_memAlloc->map(image.memHandle).value;
    return pData;
}
void ResourceAllocator::unmap(const Image &image)
{
    m_memAlloc->unmap(image.memHandle);
}

MemoryHandle ResourceAllocator::AllocateMemory(const MemoryAllocateInfo &allocateInfo)
{
    return m_memAlloc->allocMemory(allocateInfo).value;
}

void ResourceAllocator::CreateBufferEx(const vk::BufferCreateInfo &info_, vk::Buffer &buffer)
{
    buffer = m_deviceHandle->createBuffer(info_, allocationCallbacks);
}

void ResourceAllocator::CreateImageEx(const vk::ImageCreateInfo &info_, vk::Image &image)
{
    image = m_deviceHandle->createImage(info_, allocationCallbacks);
}