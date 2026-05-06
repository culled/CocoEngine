//
// Created by cullen on 3/25/26.
//

#include "VulkanPipeline.h"
#include "../../../RenderGraph/RenderGraph.h"
#include "../VulkanGraphicsPlatform.h"
#include "../Resources/VulkanShaderProgram.h"
#include "Coco/Core/Engine.h"

#include "Coco/Rendering/RHI/Vulkan/VulkanUtils.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"

#undef None
#undef Always

namespace Coco
{
    VulkanPipeline::VulkanPipeline(uint64 key, VulkanGraphicsPlatform* platform, const VulkanShaderProgram& shaderProgram,
        const RenderGraph& graph, const GraphicsPipelineState& pipelineState) :
        _key(key),
        _platform(platform),
        _pipeline(nullptr),
        _lastUsedFrameNumber(platform->GetCurrentFrameNumber())
    {
        auto attachments = graph.GetCurrentPassAttachments();
        Array<VkFormat> colorFormats(nullptr, attachments.size());
        VkFormat depthStencilFormat = VK_FORMAT_UNDEFINED;

        for (const auto& attachment : attachments)
        {
            const auto& imageDesc = attachment.AttachmentImage->GetDescription();
            VkFormat& format = attachment.Type == ImageAttachmentType::Color ? colorFormats.EmplaceBack() : depthStencilFormat;
            format = VulkanUtils::ToVkFormat(imageDesc.PixelFormat, imageDesc.ColorSpace);
        }

        VkPipelineRenderingCreateInfoKHR pipelineCreateInfo { VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR };
        pipelineCreateInfo.pColorAttachmentFormats = colorFormats.Data();
        pipelineCreateInfo.colorAttachmentCount = static_cast<uint32>(colorFormats.GetCount());
        pipelineCreateInfo.depthAttachmentFormat = depthStencilFormat;
        pipelineCreateInfo.stencilAttachmentFormat = depthStencilFormat;

        // Dynamic state
        StackArray<VkDynamicState, 3> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };

        VkPipelineDynamicStateCreateInfo dynamicState{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.pDynamicStates = dynamicStates.Data();
        dynamicState.dynamicStateCount = static_cast<uint32>(dynamicStates.GetCount());

        // TODO: multiple viewports and scissors
        // Only need to specify the number of viewports and scissors since they are dynamic states
        VkPipelineViewportStateCreateInfo viewportState{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // Rasterization state
        VkPipelineRasterizationStateCreateInfo rasterizationState{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizationState.rasterizerDiscardEnable = VK_FALSE;
        rasterizationState.polygonMode = VulkanUtils::ToVkPolygonMode(pipelineState.FillMode);
        rasterizationState.lineWidth = 1.0f;
        rasterizationState.cullMode = VulkanUtils::ToVkCullModeFlags(pipelineState.CullingMode);
        rasterizationState.frontFace = VulkanUtils::ToVkFrontFace(pipelineState.WindingMode);
        rasterizationState.depthClampEnable = pipelineState.EnableDepthClamping;
        rasterizationState.depthBiasEnable = VK_FALSE; // TODO: depth biasing?
        rasterizationState.depthBiasConstantFactor = 0.0f;
        rasterizationState.depthBiasSlopeFactor = 0.0f;
        rasterizationState.depthBiasClamp = 0.0f;

        // Multisample state
        VkPipelineMultisampleStateCreateInfo multisampleState{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampleState.sampleShadingEnable = VK_FALSE;
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // TODO: MSAA
        multisampleState.minSampleShading = 1.0f;
        multisampleState.pSampleMask = nullptr;
        multisampleState.alphaToCoverageEnable = VK_FALSE; // TODO: alpha to coverage
        multisampleState.alphaToOneEnable = VK_FALSE; // TODO: alpha to one

        // Depth/stencil state
        VkPipelineDepthStencilStateCreateInfo depthStencilState{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthStencilState.depthTestEnable = pipelineState.DepthTestMode != DepthTestingMode::Never;
        depthStencilState.depthCompareOp = VulkanUtils::ToVkCompareOp(pipelineState.DepthTestMode);
        depthStencilState.depthWriteEnable = pipelineState.EnableDepthWrite;
        depthStencilState.depthBoundsTestEnable = VK_FALSE;
        depthStencilState.stencilTestEnable = VK_FALSE; // TODO: stencils

        // Attachment blend states
        Array<VkPipelineColorBlendAttachmentState> colorBlendAttachments(nullptr, attachments.size());
        for (const auto& attachment : attachments)
        {
            // Only blend color attachments
            if (attachment.Type != ImageAttachmentType::Color)
                continue;

            VkPipelineColorBlendAttachmentState& blendState = colorBlendAttachments.EmplaceBack();
            blendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

            blendState.blendEnable = pipelineState.BlendState.ColorBlendOperation != BlendOperation::None || pipelineState.BlendState.AlphaBlendOperation != BlendOperation::None;
            blendState.srcColorBlendFactor = VulkanUtils::ToVkBlendFactor(pipelineState.BlendState.ColorSourceFactor);
            blendState.dstColorBlendFactor = VulkanUtils::ToVkBlendFactor(pipelineState.BlendState.ColorDestinationFactor);
            blendState.colorBlendOp = VulkanUtils::ToVkBlendOp(pipelineState.BlendState.ColorBlendOperation);
            blendState.srcAlphaBlendFactor = VulkanUtils::ToVkBlendFactor(pipelineState.BlendState.AlphaSourceFactor);
            blendState.dstAlphaBlendFactor = VulkanUtils::ToVkBlendFactor(pipelineState.BlendState.AlphaDestinationFactor);
            blendState.alphaBlendOp = VulkanUtils::ToVkBlendOp(pipelineState.BlendState.AlphaBlendOperation);
        }

        VkPipelineColorBlendStateCreateInfo colorBlendState{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlendState.pAttachments = colorBlendAttachments.Data();
        colorBlendState.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.GetCount());
        colorBlendState.logicOpEnable = VK_FALSE; // TODO: logic blending?
        colorBlendState.logicOp = VK_LOGIC_OP_COPY;
        colorBlendState.blendConstants[0] = 0.0f;
        colorBlendState.blendConstants[1] = 0.0f;
        colorBlendState.blendConstants[2] = 0.0f;
        colorBlendState.blendConstants[3] = 0.0f;

        // Input assembly state
        VkPipelineInputAssemblyStateCreateInfo inputState{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputState.topology = VulkanUtils::ToVkPrimitiveTopology(pipelineState.TopologyMode);
        inputState.primitiveRestartEnable = VK_FALSE;

        StackArray<VkPipelineShaderStageCreateInfo, 2> shaderStageInfos;
        shaderProgram.GetStageInfos(shaderStageInfos);

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        graphicsPipelineCreateInfo.pStages = shaderStageInfos.Data();
        graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStageInfos.GetCount());

        VkPipelineVertexInputStateCreateInfo vertexInputState = shaderProgram.GetVertexInputStateCreateInfo();

        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputState;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputState;
        graphicsPipelineCreateInfo.pViewportState = &viewportState;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationState;
        graphicsPipelineCreateInfo.pMultisampleState = &multisampleState;
        graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilState;
        graphicsPipelineCreateInfo.pColorBlendState = &colorBlendState;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicState;
        graphicsPipelineCreateInfo.pTessellationState = nullptr; // TODO: tessellation
        graphicsPipelineCreateInfo.layout = shaderProgram.GetPipelineLayout()->PipelineLayout;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex = -1;

        // Since we're using dynamic rendering, no render pass
        graphicsPipelineCreateInfo.renderPass = nullptr;
        graphicsPipelineCreateInfo.pNext = &pipelineCreateInfo;

        AssertVkSuccess(
            vkCreateGraphicsPipelines(
                _platform->GetDevice(),
                nullptr,
                1,
                &graphicsPipelineCreateInfo,
                _platform->GetAllocationCallbacks(),
                &_pipeline
            )
        );

        COCO_ENGINE_LOG_VERBOSE("Created VulkanPipeline %u for shader %u", key, shaderProgram.GetID());
    }

    VulkanPipeline::~VulkanPipeline()
    {
        if (_pipeline)
        {
            _platform->WaitForIdle();
            vkDestroyPipeline(_platform->GetDevice(), _pipeline, _platform->GetAllocationCallbacks());
            _pipeline = nullptr;
        }

        COCO_ENGINE_LOG_VERBOSE("Destroyed VulkanPipeline %u", _key);
    }

    uint64 VulkanPipeline::MakeKey(const VulkanShaderProgram& shaderProgram, const RenderGraph& graph,
        const GraphicsPipelineState& pipelineState)
    {
        return Math::CombineHashes(shaderProgram.GetID(), graph.GetCurrentPassAttachmentHash(), ToHash(pipelineState));
    }

    void VulkanPipeline::MarkUsed()
    {
        _lastUsedFrameNumber = _platform->GetCurrentFrameNumber();
    }
} // Coco