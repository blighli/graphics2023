#pragma once

// Return the access flag for an image layout
static inline vk::AccessFlags2 accessFlagsForImageLayout(vk::ImageLayout layout)
{
    switch (layout)
    {
    case vk::ImageLayout::ePreinitialized:
        return vk::AccessFlagBits2::eHostWrite;
    case vk::ImageLayout::eTransferDstOptimal:
        return vk::AccessFlagBits2::eTransferWrite;
    case vk::ImageLayout::eTransferSrcOptimal:
        return vk::AccessFlagBits2::eTransferRead;
    case vk::ImageLayout::eColorAttachmentOptimal:
        return vk::AccessFlagBits2::eColorAttachmentWrite;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        return vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
        return vk::AccessFlagBits2::eShaderRead;
    default:
        return vk::AccessFlags2{};
    }
}

static inline vk::PipelineStageFlags2 pipelineStageForLayout(vk::ImageLayout layout)
{
    switch (layout)
    {
    case vk::ImageLayout::eTransferDstOptimal:
    case vk::ImageLayout::eTransferSrcOptimal:
        return vk::PipelineStageFlagBits2::eTransfer;
    case vk::ImageLayout::eColorAttachmentOptimal:
        return vk::PipelineStageFlagBits2::eColorAttachmentOutput;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        return vk::PipelineStageFlagBits2::eAllCommands; // We do this to allow queue other than graphic
                                                         // return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
        return vk::PipelineStageFlagBits2::eAllCommands; // We do this to allow queue other than graphic
                                                         // return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    case vk::ImageLayout::ePreinitialized:
        return vk::PipelineStageFlagBits2::eHost;
    case vk::ImageLayout::eUndefined:
        return vk::PipelineStageFlagBits2::eTopOfPipe;
    default:
        return vk::PipelineStageFlagBits2::eBottomOfPipe;
    }
}

static inline vk::ImageMemoryBarrier2 makeImageMemoryBarrier(vk::Image img,
                                                             vk::AccessFlags2 srcAccess,
                                                             vk::AccessFlags2 dstAccess,
                                                             vk::ImageLayout oldLayout,
                                                             vk::ImageLayout newLayout,
                                                             vk::ImageAspectFlags aspectMask)
{
    vk::ImageMemoryBarrier2 barrier{};
    barrier.setSrcAccessMask(srcAccess)
        .setDstAccessMask(dstAccess)
        .setOldLayout(oldLayout)
        .setNewLayout(newLayout)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setImage(img)
        .setSubresourceRange(vk::ImageSubresourceRange{aspectMask, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS});

    return barrier;
}

static inline void cmdBarrierImageLayout(vk::CommandBuffer cmdbuffer,
                                         vk::Image image,
                                         vk::ImageLayout oldImageLayout,
                                         vk::ImageLayout newImageLayout,
                                         const vk::ImageSubresourceRange &subresourceRange)
{
    // Create an image barrier to change the layout
    vk::ImageMemoryBarrier2 imageMemoryBarrier{};
    imageMemoryBarrier.setOldLayout(oldImageLayout)
        .setNewLayout(newImageLayout)
        .setImage(image)
        .setSubresourceRange(subresourceRange)
        .setSrcAccessMask(accessFlagsForImageLayout(oldImageLayout))
        .setDstAccessMask(accessFlagsForImageLayout(newImageLayout))
        .setSrcStageMask(pipelineStageForLayout(oldImageLayout))
        .setDstStageMask(pipelineStageForLayout(newImageLayout))
        // Fix for a validation issue - should be needed when VkImage sharing mode is VK_SHARING_MODE_EXCLUSIVE
        // and the values of srcQueueFamilyIndex and dstQueueFamilyIndex are equal, no ownership transfer is performed,
        // and the barrier operates as if they were both set to VK_QUEUE_FAMILY_IGNORED.
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    vk::DependencyInfo depInfo{};
    depInfo.setImageMemoryBarrierCount(1U).setPImageMemoryBarriers(&imageMemoryBarrier);
    cmdbuffer.pipelineBarrier2(&depInfo);
}

static inline void cmdBarrierImageLayout(vk::CommandBuffer cmdbuffer,
                                         vk::Image image,
                                         vk::ImageLayout oldImageLayout,
                                         vk::ImageLayout newImageLayout,
                                         vk::ImageAspectFlags aspectMask)
{
    cmdBarrierImageLayout(cmdbuffer, image, oldImageLayout, newImageLayout, {aspectMask, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS});
}

static inline void cmdBarrierImageLayout(vk::CommandBuffer cmdbuffer,
                                         vk::Image image,
                                         vk::ImageLayout oldImageLayout,
                                         vk::ImageLayout newImageLayout)
{
    cmdBarrierImageLayout(cmdbuffer, image, oldImageLayout, newImageLayout, vk::ImageAspectFlagBits::eColor);
}