//
// Created by cullen on 3/25/26.
//

#ifndef COCOENGINE_VULKANRESOURCECACHE_H
#define COCOENGINE_VULKANRESOURCECACHE_H
#include "Coco/Core/Types/Map.h"

namespace Coco
{
    class VulkanGraphicsPlatform;
    struct GraphicsPipelineState;
    class RenderGraph;
    class VulkanShaderProgram;
    class VulkanPipeline;

    class VulkanResourceCache
    {
    public:
        VulkanResourceCache(VulkanGraphicsPlatform* platform);
        ~VulkanResourceCache();

        VulkanPipeline* GetOrCreatePipeline(const VulkanShaderProgram& shaderProgram, const RenderGraph& graph, const GraphicsPipelineState& pipelineState);

    private:
        VulkanGraphicsPlatform* _platform;
        Map<uint64, VulkanPipeline> _pipelines;
    };
} // Coco

#endif //COCOENGINE_VULKANRESOURCECACHE_H