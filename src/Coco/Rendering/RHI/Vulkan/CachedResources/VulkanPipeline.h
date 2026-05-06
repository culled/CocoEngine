//
// Created by cullen on 3/25/26.
//

#ifndef COCOENGINE_VULKANPIPELINE_H
#define COCOENGINE_VULKANPIPELINE_H

#include "Coco/Core/Types/CoreTypes.h"
#include "../VulkanForwardDeclarations.h"

namespace Coco
{
    struct GraphicsPipelineState;
    class VulkanGraphicsPlatform;
    class RenderGraph;
    class VulkanShaderProgram;

    class VulkanPipeline
    {
    public:
        VulkanPipeline(uint64 key, VulkanGraphicsPlatform* platform, const VulkanShaderProgram& shaderProgram, const RenderGraph& graph, const GraphicsPipelineState& pipelineState);
        ~VulkanPipeline();

        VulkanPipeline(const VulkanPipeline&) = delete;
        VulkanPipeline& operator=(const VulkanPipeline&) = delete;

        static uint64 MakeKey(const VulkanShaderProgram& shaderProgram, const RenderGraph& graph, const GraphicsPipelineState& pipelineState);

        VkPipeline GetPipeline() const { return _pipeline; }
        void MarkUsed();

    private:
        uint64 _key;
        VulkanGraphicsPlatform* _platform;
        VkPipeline _pipeline;
        uint64 _lastUsedFrameNumber;
    };
} // Coco

#endif //COCOENGINE_VULKANPIPELINE_H