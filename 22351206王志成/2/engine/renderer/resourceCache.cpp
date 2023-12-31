#include <exception>

#include "resourceCache.h"

void ResourceCacheTable::init(std::shared_ptr<vk::Device> devicePtr)
{
    m_deviceHandle = devicePtr;

    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo{};
    m_pipelineCacheHandle = m_deviceHandle->createPipelineCache(pipelineCacheCreateInfo, allocationCallbacks);
}

// template <typename T, typename... Args>
// std::shared_ptr<T> ResourceCacheTable::requestResource(const Args &...args)
// {
//     const char *name = resourceTypeToNameStr<T>();
//     auto mutex_iter = m_resourceMutexMap.find(name);
//     if (mutex_iter != m_resourceMutexMap.end())
//     {
//         std::lock_guard lock(mutex_iter->second);

//         std::size_t hashCode = 0U;
//         hash(hashCode, args...);

//         auto obj_iter = m_resourceObjectMap[name].find(hashCode);
//         if (obj_iter != m_resourceObjectMap[name].end())
//             return std::make_shared<T>(std::any_cast<std::shared_ptr<T>>(obj_iter->second));

//         T created_obj = createResource<T>(args...);
//         auto insert_iter = m_resourceObjectMap[name].emplace(hashCode, std::any{create_obj});
//         if (!insert_iter->second)
//             throw std::runtime_error("failed to insert created object");
//         return std::make_shared<T>(std::any_cast<T>(inserted_iter->first->second));
//     }
//     return nullptr;
// }

void ResourceCacheTable::destroy()
{
    if (m_deviceHandle)
    {
        m_deviceHandle->destroyPipelineCache(m_pipelineCacheHandle, allocationCallbacks);
    }
}

// template <typename T>
// const char *ResourceCacheTable::resourceTypeToNameStr() const
// {
//     if constexpr (std::is_same_v<T, vk::>)
//         return "shader_module";
//     if constexpr (std::is_same_v<T, vk::>)
//         return "graphics_pipeline";
//     if constexpr (std::is_same_v<T, vk::>)
//         return "compute_pipeline";
// }

// template <typename T, typename... Args>
// T ResourceCacheTable::createResource(const Args &...args)
// {
// }