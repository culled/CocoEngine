//
// Created by cullen on 3/25/26.
//

#include "VulkanResourceCache.h"

#include "VulkanGraphicsPlatform.h"
#include "CachedResources/VulkanPipeline.h"

namespace Coco
{
    VulkanResourceCache::VulkanResourceCache(VulkanGraphicsPlatform* platform) :
        _platform(platform),
        _pipelines()
    {}

    VulkanResourceCache::~VulkanResourceCache()
    {
        _pipelines.Clear();
    }

    VulkanPipeline* VulkanResourceCache::GetOrCreatePipeline(const VulkanShaderProgram& shaderProgram,
        const RenderGraph& graph, const GraphicsPipelineState& pipelineState)
    {
        uint64 key = VulkanPipeline::MakeKey(shaderProgram, graph, pipelineState);
        VulkanPipeline* existing = _pipelines.TryGetValue(key);
        if (!existing)
        {
            existing = &_pipelines.Emplace(key, key, _platform, shaderProgram, graph, pipelineState);
        }
        else
        {
            existing->MarkUsed();
        }

        return existing;
    }
} // Coco