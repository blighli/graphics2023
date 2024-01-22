#pragma once

#include <nvvk/commands_vk.hpp>
#include <nvvk/memallocator_vma_vk.hpp>
#include <nvvk/descriptorsets_vk.hpp>
#include <nvvk/structs_vk.hpp>
#include <nvvk/images_vk.hpp>

#include "mesh.hpp"
#include "material.hpp"
#include "texture.hpp"

class Application;

class Scene
{
protected:
    using objectGroup = std::vector<Mesh>;

public:
    Scene(const Scene &) = delete;
    Scene(Scene &&) = delete;
    Scene &operator=(const Scene &) = delete;
    Scene &operator=(Scene &&) = delete;

    static Scene &getInstance()
    {
        static Scene instance;
        return instance;
    }

    auto dirty() const { return m_dirty; }
    auto getSceneHierarchy() const
    {
        std::unordered_map<const char *, std::vector<const char *>> res;
        auto groupIndex = 0ULL;
        for (const auto &group : m_objects)
        {
            // auto groupName = "group " + std::to_string(groupIndex);
            // res.insert({groupName.c_str(), {}});
            res.insert({"group", {}});
            for (const auto &object : group)
                res["group"].emplace_back(object.name.c_str());
        }
        return res;
    }

    void addObjectGroup(const objectGroup &group)
    {
        m_objects.emplace_back(group);
        m_dirty = true;
    }

    void addTexture(const Texture &texture) { m_textures.emplace_back(texture); }

    void addMaterial(const Material &material) { m_materials.emplace_back(material); }

    void prepareToDraw()
    {
        if (!m_descPool)
        {
            std::vector<VkDescriptorPoolSize> poolSizes{};
            poolSizes.push_back({VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8U});
            poolSizes.push_back({VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 64U});
            VkDescriptorPoolCreateInfo createInfo = nvvk::make<VkDescriptorPoolCreateInfo>();
            createInfo.flags = VkDescriptorPoolCreateFlagBits::VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
            createInfo.maxSets = 4U;
            createInfo.poolSizeCount = poolSizes.size();
            createInfo.pPoolSizes = poolSizes.data();
            NVVK_CHECK(vkCreateDescriptorPool(m_deviceHandle, &createInfo, VK_NULL_HANDLE, &m_descPool));
        }
        if (m_geometryBinding.empty())
        {
            m_geometryBinding.addBinding(0, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT);
            m_geometryBinding.addBinding(1, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT);
            m_geometryBinding.addBinding(2, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT);
            m_geometryBinding.addBinding(3, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT);
            m_geometryBinding.setBindingFlags(0, VkDescriptorBindingFlagBits::VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
            m_geometryBinding.setBindingFlags(1, VkDescriptorBindingFlagBits::VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
            m_geometryBinding.setBindingFlags(2, VkDescriptorBindingFlagBits::VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
            m_geometryBinding.setBindingFlags(3, VkDescriptorBindingFlagBits::VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);

            m_geometrySetLayout = m_geometryBinding.createLayout(m_deviceHandle, VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT, nvvk::DescriptorSupport::CORE_1_2);
            m_geometrySet = nvvk::allocateDescriptorSet(m_deviceHandle, m_descPool, m_geometrySetLayout);
        }
        if (m_textureBinding.empty())
        {
            const uint32_t textureSetMaxVariableCount = 32;
            m_textureBinding.addBinding(0, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, textureSetMaxVariableCount, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT);
            m_textureBinding.setBindingFlags(0, VkDescriptorBindingFlagBits::VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VkDescriptorBindingFlagBits::VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT | VkDescriptorBindingFlagBits::VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

            m_textureSetLayout = m_textureBinding.createLayout(m_deviceHandle, VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT, nvvk::DescriptorSupport::INDEXING_EXT);

            VkDescriptorSetAllocateInfo allocInfo = nvvk::make<VkDescriptorSetAllocateInfo>();
            allocInfo.descriptorPool = m_descPool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &m_textureSetLayout;
            VkDescriptorSetVariableDescriptorCountAllocateInfo variableInfo = nvvk::make<VkDescriptorSetVariableDescriptorCountAllocateInfo>();
            variableInfo.descriptorSetCount = 1;
            variableInfo.pDescriptorCounts = &textureSetMaxVariableCount;
            allocInfo.pNext = &variableInfo;
            NVVK_CHECK(vkAllocateDescriptorSets(m_deviceHandle, &allocInfo, &m_textureSet));
        }

        if (!m_dirty)
            return;

        m_allocatorHandle.destroy(m_vertexBuffer);
        m_allocatorHandle.destroy(m_triangleBuffer);
        m_allocatorHandle.destroy(m_materialBuffer);
        m_allocatorHandle.destroy(m_indexBuffer);

        std::vector<VertexAttribute> totalVertexData{};
        std::vector<FaceAttribute> totalTriangleData{};
        std::vector<MaterialAttribute> totalMaterialData{};
        std::vector<uint32_t> totalIndexData{};

        auto offset = 0U;
        for (const auto &group : m_objects)
        {
            for (const auto &object : group)
            {
                totalVertexData.insert(totalVertexData.end(), object.vertices.begin(), object.vertices.end());
                totalTriangleData.insert(totalTriangleData.end(), object.faces.begin(), object.faces.end());
                auto beginIndex = totalIndexData.size();
                totalIndexData.insert(totalIndexData.end(), object.indices.begin(), object.indices.end());
                std::for_each(totalIndexData.begin() + beginIndex, totalIndexData.end(), [&](uint32_t &elem)
                              { elem += offset; });
                offset = totalVertexData.size();
            }
        }
        m_totalVertexCount = totalTriangleData.size() * 3;
        totalMaterialData.reserve(m_materials.size());
        for (const auto &material : m_materials)
            totalMaterialData.emplace_back(material.properties);

        {
            nvvk::ScopeCommandBuffer scopedBuffer(m_deviceHandle, m_transferQueueFamilyIndex, m_transferQueue);

            m_vertexBuffer = m_allocatorHandle.createBuffer(scopedBuffer, totalVertexData, VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
            m_triangleBuffer = m_allocatorHandle.createBuffer(scopedBuffer, totalTriangleData, VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
            m_materialBuffer = m_allocatorHandle.createBuffer(scopedBuffer, totalMaterialData, VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
            m_indexBuffer = m_allocatorHandle.createBuffer(scopedBuffer, totalIndexData, VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

            for (auto &texture : m_textures)
                if (texture.gpuHandle.memHandle == nullptr)
                    texture.gpuHandle = m_allocatorHandle.createTexture(scopedBuffer, texture.width * texture.height * 4, texture.cpuHandle,
                                                                        nvvk::makeImage2DCreateInfo({static_cast<uint32_t>(texture.width), static_cast<uint32_t>(texture.height)}, texture.format),
                                                                        nvvk::makeSamplerCreateInfo());

            std::vector<VkImageMemoryBarrier> barriers{};
            for (auto &texture : m_textures)
                barriers.emplace_back(nvvk::makeImageMemoryBarrier(texture.gpuHandle.image, nvvk::accessFlagsForImageLayout(VK_IMAGE_LAYOUT_UNDEFINED), nvvk::accessFlagsForImageLayout(texture.gpuHandle.descriptor.imageLayout), VK_IMAGE_LAYOUT_UNDEFINED, texture.gpuHandle.descriptor.imageLayout));
            vkCmdPipelineBarrier(scopedBuffer, nvvk::pipelineStageForLayout(VK_IMAGE_LAYOUT_UNDEFINED), nvvk::pipelineStageForLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL), 0, 0, nullptr, 0, nullptr, barriers.size(), barriers.data());
        }

        std::vector<VkWriteDescriptorSet> writeDescs{};
        VkDescriptorBufferInfo vertexInfo{m_vertexBuffer.buffer, 0, VK_WHOLE_SIZE};
        writeDescs.emplace_back(m_geometryBinding.makeWrite(m_geometrySet, 0, &vertexInfo));
        VkDescriptorBufferInfo triangleInfo{m_triangleBuffer.buffer, 0, VK_WHOLE_SIZE};
        writeDescs.emplace_back(m_geometryBinding.makeWrite(m_geometrySet, 1, &triangleInfo));
        VkDescriptorBufferInfo materialInfo{m_materialBuffer.buffer, 0, VK_WHOLE_SIZE};
        writeDescs.emplace_back(m_geometryBinding.makeWrite(m_geometrySet, 2, &materialInfo));
        VkDescriptorBufferInfo indexInfo{m_indexBuffer.buffer, 0, VK_WHOLE_SIZE};
        writeDescs.emplace_back(m_geometryBinding.makeWrite(m_geometrySet, 3, &indexInfo));
        for (auto i = 0; i < m_textures.size(); ++i)
            writeDescs.emplace_back(m_textureBinding.makeWrite(m_textureSet, 0, &m_textures[i].gpuHandle.descriptor, i));
        {
            nvvk::ScopeCommandBuffer scopedBuffer(m_deviceHandle, m_transferQueueFamilyIndex, m_transferQueue);
            vkUpdateDescriptorSets(m_deviceHandle, writeDescs.size(), writeDescs.data(), 0, nullptr);
        }

        m_dirty = false;
    }

    void deinit()
    {
        for (auto &texture : m_textures)
        {
            if (texture.gpuHandle.memHandle != nullptr)
                m_allocatorHandle.destroy(texture.gpuHandle);
            delete static_cast<unsigned char *>(texture.cpuHandle);
        }

        if (m_vertexBuffer.buffer)
            m_allocatorHandle.destroy(m_vertexBuffer);
        if (m_triangleBuffer.buffer)
            m_allocatorHandle.destroy(m_triangleBuffer);
        if (m_materialBuffer.buffer)
            m_allocatorHandle.destroy(m_materialBuffer);
        if (m_indexBuffer.buffer)
            m_allocatorHandle.destroy(m_indexBuffer);

        if (m_geometrySetLayout)
            vkDestroyDescriptorSetLayout(m_deviceHandle, m_geometrySetLayout, VK_NULL_HANDLE);
        if (m_textureSetLayout)
            vkDestroyDescriptorSetLayout(m_deviceHandle, m_textureSetLayout, VK_NULL_HANDLE);
        if (m_descPool)
            vkDestroyDescriptorPool(m_deviceHandle, m_descPool, VK_NULL_HANDLE);

        m_allocatorHandle.deinit();
    }

    friend class Application;

protected:
    Scene() {}
    ~Scene() {}

    VkDevice m_deviceHandle{};
    nvvk::ResourceAllocatorVma m_allocatorHandle{};
    uint32_t m_transferQueueFamilyIndex{~0U};
    VkQueue m_transferQueue{};

    std::vector<objectGroup> m_objects{};
    std::vector<Texture> m_textures{};
    std::vector<Material> m_materials{};

    // merge all vertices and indices into few large buffers
    // and use offset to draw different objects
    // and use lazy update to merge objects' draw data
    nvvk::Buffer m_vertexBuffer{};
    nvvk::Buffer m_triangleBuffer{};
    nvvk::Buffer m_materialBuffer{};
    nvvk::Buffer m_indexBuffer{};
    size_t m_totalVertexCount{};
    bool m_dirty{false};

    // for convience, directly hold descriptors
    VkDescriptorPool m_descPool{};
    nvvk::DescriptorSetBindings m_geometryBinding{};
    VkDescriptorSetLayout m_geometrySetLayout{};
    VkDescriptorSet m_geometrySet{};
    nvvk::DescriptorSetBindings m_textureBinding{};
    VkDescriptorSetLayout m_textureSetLayout{};
    VkDescriptorSet m_textureSet{};

    /* texture maps */
    /* basic */
    // nvvk::Texture diffuseMap{};  // RGB->diffuse, A->alpha(need rearrange)
    // nvvk::Texture specularMap{}; // RGB->specular color, A->shiness(need rearrange)
    // nvvk::Texture normalMap{};   // need convert bump map to normal map
    // nvvk::Texture displacementMap{};
    // nvvk::Texture reflectionMap{};
    // nvvk::Texture ambientMap{};
    // /* PBR's */
    // nvvk::Texture pbrBasicMap{}; // R->metallic, G->roughness, B->AO(unused now), A->emissive(need rearrange)
    // nvvk::Texture pbrExtraMap{}; // only sheen now
};