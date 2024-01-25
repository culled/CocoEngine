#pragma once

#include "CachedVulkanResource.h"
#include "../../../../Renderpch.h"
#include "../../../GraphicsPipelineTypes.h"
#include <Coco/Core/Resources/ResourceTypes.h>
#include "../VulkanIncludes.h"

namespace Coco::Rendering
{
    struct CompiledRenderPipeline;
}

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    struct VulkanSubPassInfo
    {
        VkSubpassDescription Description;
        std::vector<uint32> PreserveAttachmentIndices;
        std::vector<VkAttachmentReference> ColorAttachmentReferences;
        std::optional<VkAttachmentReference> DepthStencilAttachmentReference;
        std::vector<VkAttachmentReference> ResolveAttachmentReferences;
        std::vector<VkAttachmentReference> InputAttachmentReferences;
    };

    class VulkanRenderPass :
        public CachedVulkanResource
    {
    public:
        VulkanRenderPass(uint64 id, VulkanGraphicsDevice& device);
        ~VulkanRenderPass();

        static uint64 MakeKey(const CompiledRenderPipeline& pipeline, MSAASamples samples);

        bool NeedsUpdate(const CompiledRenderPipeline& pipeline, MSAASamples samples);
        void Update(const CompiledRenderPipeline& pipeline, MSAASamples samples, std::span<const uint32> resolveAttachmentIndices);

        uint64 GetVersion() const { return _pipelineVersion; }
        VkRenderPass GetRenderPass() const { return _renderPass; }
        MSAASamples GetSampleCount() const { return _sampleCount; }

    private:
        VulkanGraphicsDevice& _device;
        VkRenderPass _renderPass;
        MSAASamples _sampleCount;
        ResourceVersion _pipelineVersion;

    private:
        MSAASamples GetAdjustedSampleCount(MSAASamples samples) const;
        void CreateRenderPass(const CompiledRenderPipeline& pipeline, MSAASamples samples, std::span<const uint32> resolveAttachmentIndices);
        void DestroyRenderPass();
    };
}