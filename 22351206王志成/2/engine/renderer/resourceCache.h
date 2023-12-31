#pragma once

#include <unordered_map>
#include <mutex>
#include <any>

#include <simple_vulkan_packing.h>
#include <hash.hpp>

//  template<>
//  std::size_t hash(const std::shared_ptr<vk::DescriptorSetLayout>& obj)
//  {
//      std::size_t res = 0U;
//      hash_param(res, (std::size_t)static_cast<VkDescriptorSetLayout>(*obj));
//      return res;
//  }

//  template<>
//  std::size_t hash(const std::shared_ptr<vk::PipelineLayout>& obj)
//  {
//      std::size_t res = 0U;
//      hash_param(res, (std::size_t)static_cast<VkPipelineLayout>(*obj));
//      return res;
//  }

//  template<>
//  std::size_t hash(const std::shared_ptr<vk::RenderPass>& obj)
//  {
//      std::size_t res = 0U;
//      hash_param(res, (std::size_t)static_cast<VkRenderPass>(*obj));
//      return res;
//  }

struct ResourceCacheTable
{
public:
    ResourceCacheTable() = default;
    ~ResourceCacheTable() { destroy(); }

    ResourceCacheTable(const ResourceCacheTable&) = delete;
    ResourceCacheTable(ResourceCacheTable&&) = delete;
    ResourceCacheTable& operator=(const ResourceCacheTable&) = delete;
    ResourceCacheTable& operator=(ResourceCacheTable&&) = delete;

    void init(std::shared_ptr<vk::Device> devicePtr);

    // template<typename T, typename... Args>
    // std::shared_ptr<T> requestResource(const Args&... args);

    std::shared_ptr<vk::PipelineCache> getPipelineCacheHandle() const { return std::make_shared<vk::PipelineCache>(m_pipelineCacheHandle); }

    void destroy();

private:
    // template<typename T>
    // const char* resourceTypeToNameStr() const;

    // template<typename T, typename... Args>
    // T createResource(const Args&... args);

    std::shared_ptr<vk::Device> m_deviceHandle;
    
    std::unordered_map<const char*, std::unordered_map<size_t, std::any>> m_resourceObjectMap;
    std::unordered_map<const char*, std::mutex> m_resourceMutexMap;

    vk::PipelineCache m_pipelineCacheHandle;
};