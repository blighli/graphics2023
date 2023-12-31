#pragma once

struct QueueInstance
{
    std::shared_ptr<vk::Queue> queue_handle;
    uint32_t queue_family_index{~0U};
    uint32_t queue_index{~0U};
    float queue_priority{1.f};
};