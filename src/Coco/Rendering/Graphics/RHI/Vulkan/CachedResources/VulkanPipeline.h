#pragma once
#include <Coco/Core/Defines.h>
#include "CachedVulkanResource.h"
#include "../../../../Renderpch.h"
#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;
    class VulkanRenderPass;
    class VulkanShader;
    class VulkanDescriptorSetLayout;

    class VulkanPipeline :
        public CachedVulkanResource
    {
    public:
        VulkanPipeline(uint64 id, VulkanGraphicsDevice& device);
        ~VulkanPipeline();

        // Inherited via CachedVulkanResource
        void Use() override;

        static uint64 MakeKey(
            const VulkanRenderPass& renderPass,
            const VulkanShader& shader,
            uint32 subpassIndex,
            const VulkanDescriptorSetLayout& globalLayout);

        uint64 GetVersion() const { return _version; }

        bool NeedsUpdate(const VulkanRenderPass& renderPass, const VulkanShader& shader) const;
        void Update(const VulkanRenderPass& renderPass,
            const VulkanShader& shader,
            uint32 subpassIndex,
            const VulkanDescriptorSetLayout& globalLayout);

        VkPipeline GetPipeline() const { return _pipeline; }
        VkPipelineLayout GetPipelineLayout() const { return _pipelineLayout; }

    private:
        VulkanGraphicsDevice& _device;
        uint64 _version;
        VkPipelineLayout _pipelineLayout;
        VkPipeline _pipeline;
        uint32 _subpassIndex;
        std::set<uint64> _setLayoutIDs;

    private:
        static uint64 MakeVersion(
            const VulkanRenderPass& renderPass,
            const VulkanShader& shader);

        void CreatePipeline(
            const VulkanRenderPass& renderPass,
            const VulkanShader& shader,
            uint32 subpassIndex,
            const VulkanDescriptorSetLayout& globalLayout);

        void DestroyPipeline();
    };
}