#pragma once

#include <vector>

struct GraphicsPipelineState
{
    // Initialize the state to common values: triangle list topology, depth test enabled,
    // dynamic viewport and scissor, one render target, blending disabled
    GraphicsPipelineState()
    {
        rasterizationState.setDepthClampEnable(VK_FALSE)
            .setRasterizerDiscardEnable(VK_FALSE)
            .setPolygonMode(vk::PolygonMode::eFill)
            .setCullMode(vk::CullModeFlagBits::eBack)
            .setFrontFace(vk::FrontFace::eCounterClockwise)
            .setDepthClampEnable(VK_FALSE)
            .setDepthBiasConstantFactor(0)
            .setDepthBiasClamp(0)
            .setDepthBiasSlopeFactor(0)
            .setLineWidth(1.0);

        inputAssemblyState.setTopology(vk::PrimitiveTopology::eTriangleList)
            .setPrimitiveRestartEnable(VK_FALSE);

        colorBlendState.setLogicOpEnable(VK_FALSE)
            .setLogicOp(vk::LogicOp::eClear)
            .setAttachmentCount(0U)
            .setPAttachments(nullptr)
            .setBlendConstants({0, 0, 0, 0});

        dynamicState.setDynamicStateCount(0U)
            .setPDynamicStates(nullptr);

        vertexInputState.setVertexAttributeDescriptionCount(0U)
            .setPVertexAttributeDescriptions(nullptr)
            .setVertexBindingDescriptionCount(0U)
            .setPVertexBindingDescriptions(nullptr);

        viewportState.setViewportCount(0U)
            .setPViewports(nullptr)
            .setScissorCount(0U)
            .setPScissors(nullptr);

        depthStencilState.setDepthTestEnable(VK_TRUE)
            .setDepthWriteEnable(VK_TRUE)
            .setDepthCompareOp(vk::CompareOp::eLessOrEqual)
            .setDepthBoundsTestEnable(VK_FALSE)
            .setStencilTestEnable(VK_FALSE);

        multisampleState.setRasterizationSamples(vk::SampleCountFlagBits::e1);
    }

    GraphicsPipelineState(const GraphicsPipelineState &src) = default;

    // Attach the pointer values of the structures to the internal arrays
    void update()
    {
        colorBlendState.attachmentCount = (uint32_t)m_blendAttachmentStates.size();
        colorBlendState.pAttachments = m_blendAttachmentStates.data();

        dynamicState.dynamicStateCount = (uint32_t)m_dynamicStateEnables.size();
        dynamicState.pDynamicStates = m_dynamicStateEnables.data();

        vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attributeDescriptions.size());
        vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(m_bindingDescriptions.size());
        vertexInputState.pVertexBindingDescriptions = m_bindingDescriptions.data();
        vertexInputState.pVertexAttributeDescriptions = m_attributeDescriptions.data();

        if (m_viewports.empty())
        {
            viewportState.viewportCount = 1;
            viewportState.pViewports = nullptr;
        }
        else
        {
            viewportState.viewportCount = (uint32_t)m_viewports.size();
            viewportState.pViewports = m_viewports.data();
        }

        if (m_scissors.empty())
        {
            viewportState.scissorCount = 1;
            viewportState.pScissors = nullptr;
        }
        else
        {
            viewportState.scissorCount = (uint32_t)m_scissors.size();
            viewportState.pScissors = m_scissors.data();
        }
    }

    static inline vk::PipelineColorBlendAttachmentState makePipelineColorBlendAttachmentState(
        vk::ColorComponentFlags colorWriteMask_ = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
        VkBool32 blendEnable_ = VK_FALSE,
        vk::BlendFactor srcColorBlendFactor_ = vk::BlendFactor::eZero,
        vk::BlendFactor dstColorBlendFactor_ = vk::BlendFactor::eZero,
        vk::BlendOp colorBlendOp_ = vk::BlendOp::eAdd,
        vk::BlendFactor srcAlphaBlendFactor_ = vk::BlendFactor::eZero,
        vk::BlendFactor dstAlphaBlendFactor_ = vk::BlendFactor::eZero,
        vk::BlendOp alphaBlendOp_ = vk::BlendOp::eAdd)
    {
        vk::PipelineColorBlendAttachmentState res{};
        res.setBlendEnable(blendEnable_)
            .setSrcAlphaBlendFactor(srcColorBlendFactor_)
            .setDstColorBlendFactor(dstColorBlendFactor_)
            .setColorBlendOp(colorBlendOp_)
            .setSrcAlphaBlendFactor(srcAlphaBlendFactor_)
            .setDstAlphaBlendFactor(dstAlphaBlendFactor_)
            .setAlphaBlendOp(alphaBlendOp_)
            .setColorWriteMask(colorWriteMask_);
        return res;
    }

    static inline vk::VertexInputBindingDescription makeVertexInputBinding(uint32_t binding, uint32_t stride, vk::VertexInputRate rate = vk::VertexInputRate::eVertex)
    {
        vk::VertexInputBindingDescription vertexBinding{};
        vertexBinding.setBinding(binding).setInputRate(rate).setStride(stride);
        return vertexBinding;
    }

    static inline vk::VertexInputAttributeDescription makeVertexInputAttribute(uint32_t location, uint32_t binding, vk::Format format, uint32_t offset)
    {
        vk::VertexInputAttributeDescription attrib{};
        attrib.setBinding(binding).setLocation(location).setFormat(format).setOffset(offset);
        return attrib;
    }

    void clearBlendAttachmentStates() { m_blendAttachmentStates.clear(); }
    void setBlendAttachmentCount(uint32_t attachmentCount) { m_blendAttachmentStates.resize(attachmentCount); }

    void setBlendAttachmentState(uint32_t attachment, const vk::PipelineColorBlendAttachmentState &blendState)
    {
        assert(attachment < m_blendAttachmentStates.size());
        if (attachment <= m_blendAttachmentStates.size())
        {
            m_blendAttachmentStates[attachment] = blendState;
        }
    }

    uint32_t addBlendAttachmentState(const vk::PipelineColorBlendAttachmentState &blendState)
    {
        m_blendAttachmentStates.push_back(blendState);
        return (uint32_t)(m_blendAttachmentStates.size() - 1);
    }

    void clearDynamicStateEnables() { m_dynamicStateEnables.clear(); }
    void setDynamicStateEnablesCount(uint32_t dynamicStateCount) { m_dynamicStateEnables.resize(dynamicStateCount); }

    void setDynamicStateEnable(uint32_t state, vk::DynamicState dynamicState)
    {
        assert(state < m_dynamicStateEnables.size());
        if (state <= m_dynamicStateEnables.size())
        {
            m_dynamicStateEnables[state] = dynamicState;
        }
    }

    uint32_t addDynamicStateEnable(vk::DynamicState dynamicState)
    {
        m_dynamicStateEnables.push_back(dynamicState);
        return (uint32_t)(m_dynamicStateEnables.size() - 1);
    }

    void clearBindingDescriptions() { m_bindingDescriptions.clear(); }
    void setBindingDescriptionsCount(uint32_t bindingDescriptionCount)
    {
        m_bindingDescriptions.resize(bindingDescriptionCount);
    }
    void setBindingDescription(uint32_t binding, vk::VertexInputBindingDescription bindingDescription)
    {
        assert(binding < m_bindingDescriptions.size());
        if (binding <= m_bindingDescriptions.size())
        {
            m_bindingDescriptions[binding] = bindingDescription;
        }
    }

    uint32_t addBindingDescription(const vk::VertexInputBindingDescription &bindingDescription)
    {
        m_bindingDescriptions.push_back(bindingDescription);
        return (uint32_t)(m_bindingDescriptions.size() - 1);
    }

    void addBindingDescriptions(const std::vector<vk::VertexInputBindingDescription> &bindingDescriptions_)
    {
        m_bindingDescriptions.insert(m_bindingDescriptions.end(), bindingDescriptions_.begin(), bindingDescriptions_.end());
    }

    void clearAttributeDescriptions() { m_attributeDescriptions.clear(); }
    void setAttributeDescriptionsCount(uint32_t attributeDescriptionCount)
    {
        m_attributeDescriptions.resize(attributeDescriptionCount);
    }

    void setAttributeDescription(uint32_t attribute, const vk::VertexInputAttributeDescription &attributeDescription)
    {
        assert(attribute < m_attributeDescriptions.size());
        if (attribute <= m_attributeDescriptions.size())
        {
            m_attributeDescriptions[attribute] = attributeDescription;
        }
    }

    uint32_t addAttributeDescription(const vk::VertexInputAttributeDescription &attributeDescription)
    {
        m_attributeDescriptions.push_back(attributeDescription);
        return (uint32_t)(m_attributeDescriptions.size() - 1);
    }

    void addAttributeDescriptions(const std::vector<vk::VertexInputAttributeDescription> &attributeDescriptions_)
    {
        m_attributeDescriptions.insert(m_attributeDescriptions.end(), attributeDescriptions_.begin(), attributeDescriptions_.end());
    }

    void clearViewports() { m_viewports.clear(); }
    void setViewportsCount(uint32_t viewportCount) { m_viewports.resize(viewportCount); }
    void setViewport(uint32_t attribute, vk::Viewport viewport)
    {
        assert(attribute < m_viewports.size());
        if (attribute <= m_viewports.size())
        {
            m_viewports[attribute] = viewport;
        }
    }
    uint32_t addViewport(vk::Viewport viewport)
    {
        m_viewports.push_back(viewport);
        return (uint32_t)(m_viewports.size() - 1);
    }

    void clearScissors() { m_scissors.clear(); }
    void setScissorsCount(uint32_t scissorCount) { m_scissors.resize(scissorCount); }
    void setScissor(uint32_t attribute, vk::Rect2D scissor)
    {
        assert(attribute < m_scissors.size());
        if (attribute <= m_scissors.size())
        {
            m_scissors[attribute] = scissor;
        }
    }
    uint32_t addScissor(vk::Rect2D scissor)
    {
        m_scissors.push_back(scissor);
        return (uint32_t)(m_scissors.size() - 1);
    }

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    vk::PipelineRasterizationStateCreateInfo rasterizationState{};
    vk::PipelineMultisampleStateCreateInfo multisampleState{};
    vk::PipelineDepthStencilStateCreateInfo depthStencilState{};
    vk::PipelineViewportStateCreateInfo viewportState{};
    vk::PipelineDynamicStateCreateInfo dynamicState{};
    vk::PipelineColorBlendStateCreateInfo colorBlendState{};
    vk::PipelineVertexInputStateCreateInfo vertexInputState{};

private:
    std::vector<vk::PipelineColorBlendAttachmentState> m_blendAttachmentStates{makePipelineColorBlendAttachmentState()};
    std::vector<vk::DynamicState> m_dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

    std::vector<vk::VertexInputBindingDescription> m_bindingDescriptions{};
    std::vector<vk::VertexInputAttributeDescription> m_attributeDescriptions{};

    std::vector<vk::Viewport> m_viewports{};
    std::vector<vk::Rect2D> m_scissors{};
};

class GraphicsPipelineGenerator
{
public:
    GraphicsPipelineGenerator(GraphicsPipelineState &pipelineState_)
        : pipelineState(pipelineState_)
    {
        init();
    }

    GraphicsPipelineGenerator(const GraphicsPipelineGenerator &src)
        : createInfo(src.createInfo), m_deviceHandle(src.m_deviceHandle), m_pipelineCacheHandle(src.m_pipelineCacheHandle), pipelineState(src.pipelineState)
    {
        init();
    }

    GraphicsPipelineGenerator(std::shared_ptr<vk::Device> device_, const vk::PipelineLayout &layout, const vk::RenderPass &renderPass, GraphicsPipelineState &pipelineState_)
        : m_deviceHandle(device_), pipelineState(pipelineState_)
    {
        createInfo.layout = layout;
        createInfo.renderPass = renderPass;
        init();
    }

    // For VK_KHR_dynamic_rendering
    using PipelineRenderingCreateInfo = vk::PipelineRenderingCreateInfo;

    GraphicsPipelineGenerator(std::shared_ptr<vk::Device> device_,
                              const vk::PipelineLayout &layout,
                              const PipelineRenderingCreateInfo &pipelineRenderingCreateInfo,
                              GraphicsPipelineState &pipelineState_)
        : m_deviceHandle(device_), pipelineState(pipelineState_)
    {
        createInfo.layout = layout;
        setPipelineRenderingCreateInfo(pipelineRenderingCreateInfo);
        init();
    }

    const GraphicsPipelineGenerator &operator=(const GraphicsPipelineGenerator &src)
    {
        m_deviceHandle = src.m_deviceHandle;
        pipelineState = src.pipelineState;
        createInfo = src.createInfo;
        m_pipelineCacheHandle = src.m_pipelineCacheHandle;

        init();
        return *this;
    }

    void setDevice(std::shared_ptr<vk::Device> device_) { m_deviceHandle = device_; }
    void setPipelineCache(std::shared_ptr<vk::PipelineCache> cache_) { m_pipelineCacheHandle = cache_; }

    void setRenderPass(vk::RenderPass renderPass)
    {
        createInfo.renderPass = renderPass;
        createInfo.pNext = nullptr;
    }

    void setPipelineRenderingCreateInfo(const PipelineRenderingCreateInfo &pipelineRenderingCreateInfo)
    {
        // Deep copy
        assert(pipelineRenderingCreateInfo.pNext == nullptr); // Update deep copy if needed.
        dynamicRenderingInfo = pipelineRenderingCreateInfo;
        if (dynamicRenderingInfo.colorAttachmentCount != 0)
        {
            dynamicRenderingColorFormats.assign(dynamicRenderingInfo.pColorAttachmentFormats,
                                                dynamicRenderingInfo.pColorAttachmentFormats + dynamicRenderingInfo.colorAttachmentCount);
            dynamicRenderingInfo.pColorAttachmentFormats = dynamicRenderingColorFormats.data();
        }

        // Set VkGraphicsPipelineCreateInfo::pNext to point to deep copy of extension struct.
        // NB: Will have to change if more than 1 extension struct needs to be supported.
        createInfo.pNext = &dynamicRenderingInfo;
    }

    void setLayout(vk::PipelineLayout layout) { createInfo.layout = layout; }

    ~GraphicsPipelineGenerator() { destroyShaderModules(); }

    VkPipelineShaderStageCreateInfo &addShader(const std::string &code,
                                               vk::ShaderStageFlagBits stage,
                                               const char *entryPoint = "main")
    {
        std::vector<char> v;
        std::copy(code.begin(), code.end(), std::back_inserter(v));
        return addShader(v, stage, entryPoint);
    }

    template <typename T>
    VkPipelineShaderStageCreateInfo &addShader(const std::vector<T> &code,
                                               vk::ShaderStageFlagBits stage,
                                               const char *entryPoint = "main")

    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.codeSize = sizeof(T) * code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
        vk::ShaderModule shaderModule;
        shaderModule = m_deviceHandle->createShaderModule(createInfo);
        temporaryModules.push_back(shaderModule);

        return addShader(shaderModule, stage, entryPoint);
    }
    VkPipelineShaderStageCreateInfo &addShader(vk::ShaderModule shaderModule,
                                               vk::ShaderStageFlagBits stage,
                                               const char *entryPoint = "main")
    {
        vk::PipelineShaderStageCreateInfo shaderStage{};
        shaderStage.setStage(stage)
            .setModule(shaderModule)
            .setPName(entryPoint);

        shaderStages.push_back(shaderStage);
        return shaderStages.back();
    }

    void clearShaders()
    {
        shaderStages.clear();
        destroyShaderModules();
    }

    VkShaderModule getShaderModule(size_t index) const
    {
        if (index < shaderStages.size())
            return shaderStages[index].module;
        return VK_NULL_HANDLE;
    }

    vk::Pipeline createPipeline(const vk::PipelineCache &cache)
    {
        update();
        auto [state, res] = m_deviceHandle->createGraphicsPipeline(cache, createInfo, allocationCallbacks);
        vk::resultCheck(state, "[Vulkan] Failed to create graphics pipeline.\n");
        return res;
    }

    vk::Pipeline createPipeline() { return createPipeline(*m_pipelineCacheHandle); }

    void destroyShaderModules()
    {
        for (const auto &shaderModule : temporaryModules)
        {
            m_deviceHandle->destroyShaderModule(shaderModule, allocationCallbacks);
        }
        temporaryModules.clear();
    }
    void update()
    {
        createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        createInfo.pStages = shaderStages.data();
        pipelineState.update();
    }

    vk::GraphicsPipelineCreateInfo createInfo{};

private:
    std::shared_ptr<vk::Device> m_deviceHandle;
    std::shared_ptr<vk::PipelineCache> m_pipelineCacheHandle;

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    std::vector<vk::ShaderModule> temporaryModules;
    std::vector<vk::Format> dynamicRenderingColorFormats;
    GraphicsPipelineState &pipelineState;
    PipelineRenderingCreateInfo dynamicRenderingInfo{};

    void init()
    {
        createInfo.pRasterizationState = &pipelineState.rasterizationState;
        createInfo.pInputAssemblyState = &pipelineState.inputAssemblyState;
        createInfo.pColorBlendState = &pipelineState.colorBlendState;
        createInfo.pMultisampleState = &pipelineState.multisampleState;
        createInfo.pViewportState = &pipelineState.viewportState;
        createInfo.pDepthStencilState = &pipelineState.depthStencilState;
        createInfo.pDynamicState = &pipelineState.dynamicState;
        createInfo.pVertexInputState = &pipelineState.vertexInputState;
    }
};

class GraphicsPipelineHelper : public GraphicsPipelineState, public GraphicsPipelineGenerator
{
public:
    GraphicsPipelineHelper(std::shared_ptr<vk::Device> device_, const vk::PipelineLayout &layout, const vk::RenderPass &renderPass)
        : GraphicsPipelineState(), GraphicsPipelineGenerator(device_, layout, renderPass, *this)
    {
    }
};