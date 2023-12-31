#pragma once

#include <vector>
#include <unordered_map>

#include <hash.hpp>

#include "allocationCallbacks.h"

class SamplerPool
{
public:
    SamplerPool(SamplerPool const &) = delete;
    SamplerPool &operator=(SamplerPool const &) = delete;

    SamplerPool() {}
    SamplerPool(std::shared_ptr<vk::Device> device) : m_deviceHandle(device) {}
    ~SamplerPool()
    {
        if (!m_deviceHandle)
            return;

        for (auto it : m_entries)
            if (it.sampler)
                m_deviceHandle->destroySampler(it.sampler, allocationCallbacks);

        m_freeIndex = ~0;
        m_entries.clear();
        m_samplerMap.clear();
        m_stateMap.clear();
    }

    void init(std::shared_ptr<vk::Device> device) { m_deviceHandle = device; }

    // creates a new sampler or re-uses an existing one with ref-count
    // createInfo may contain VkSamplerReductionModeCreateInfo and VkSamplerYcbcrConversionCreateInfo
    VkSampler acquireSampler(const vk::SamplerCreateInfo &createInfo)
    {
        SamplerState state;
        state.createInfo = createInfo;

        const Chain *ext = (const Chain *)createInfo.pNext;
        while (ext)
        {
            switch (ext->sType)
            {
            case VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO:
                state.reduction = *(const VkSamplerReductionModeCreateInfo *)ext;
                break;
            case VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO:
                state.ycbr = *(const VkSamplerYcbcrConversionCreateInfo *)ext;
                break;
            default:
                assert(0 && "unsupported sampler create");
            }
            ext = ext->pNext;
        }
        // always remove pointers for comparison lookup
        state.createInfo.pNext = nullptr;
        state.reduction.pNext = nullptr;
        state.ycbr.pNext = nullptr;

        auto it = m_stateMap.find(state);
        if (it == m_stateMap.end())
        {
            uint32_t index = 0;
            if (m_freeIndex != ~0)
            {
                index = m_freeIndex;
                m_freeIndex = m_entries[index].nextFreeIndex;
            }
            else
            {
                index = (uint32_t)m_entries.size();
                m_entries.resize(m_entries.size() + 1);
            }

            vk::Sampler sampler = m_deviceHandle->createSampler(createInfo, allocationCallbacks);

            m_entries[index].refCount = 1;
            m_entries[index].sampler = sampler;
            m_entries[index].state = state;

            m_stateMap.insert({state, index});
            m_samplerMap.insert({sampler, index});

            return sampler;
        }
        else
        {
            m_entries[it->second].refCount++;
            return m_entries[it->second].sampler;
        }
    }

    // decrements ref-count and destroys sampler if possible
    void releaseSampler(vk::Sampler sampler)
    {
        auto it = m_samplerMap.find(sampler);
        assert(it != m_samplerMap.end());

        uint32_t index = it->second;
        Entry &entry = m_entries[index];

        assert(entry.sampler == sampler);
        assert(entry.refCount);

        entry.refCount--;

        if (!entry.refCount)
        {
            m_deviceHandle->destroySampler(sampler, allocationCallbacks);
            entry.sampler = nullptr;
            entry.nextFreeIndex = m_freeIndex;
            m_freeIndex = index;

            m_stateMap.erase(entry.state);
            m_samplerMap.erase(sampler);
        }
    }

private:
    struct SamplerState
    {
        vk::SamplerCreateInfo createInfo{};
        vk::SamplerReductionModeCreateInfo reduction{};
        vk::SamplerYcbcrConversionCreateInfo ycbr{};

        SamplerState() = default;

        bool operator==(const SamplerState &other) const { return memcmp(this, &other, sizeof(SamplerState)) == 0; }
    };

    struct Chain
    {
        VkStructureType sType;
        const Chain *pNext;
    };

    struct Entry
    {
        vk::Sampler sampler{};
        uint32_t nextFreeIndex{~0U};
        uint32_t refCount{0U};
        SamplerState state;
    };

    std::shared_ptr<vk::Device> m_deviceHandle;
    uint32_t m_freeIndex{~0U};
    std::vector<Entry> m_entries;

    std::unordered_map<SamplerState, uint32_t, Aligned32Hasher<SamplerState>> m_stateMap;
    std::unordered_map<vk::Sampler, uint32_t, std::hash<VkSampler>> m_samplerMap;
};