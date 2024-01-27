#pragma once
#include "CachedVulkanResource.h"

#include "../../../ShaderUniformLayout.h"
#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    class VulkanDescriptorSetLayout :
        public CachedVulkanResource
    {
    public:
        VulkanDescriptorSetLayout(VulkanGraphicsDevice& device, const ShaderUniformLayout& layout, bool includeDataUniforms);
        ~VulkanDescriptorSetLayout();

        static uint64 MakeKey(const ShaderUniformLayout& layout, bool includeDataUniforms);

        std::vector<VkWriteDescriptorSet> GetDescriptorWrites() const;
        bool HasUniformsOfType(bool dataUniforms, bool textureUniforms) const;
        bool IsEmpty() const { return _layoutBindings.size() == 0; }

        const ShaderUniformLayout& GetUniformLayout() const { return _uniformLayout; }
        VkDescriptorSetLayout GetLayout() const { return _layout; }
        std::span<const VkDescriptorSetLayoutBinding> GetBindings() const { return _layoutBindings; }

    private:
        VulkanGraphicsDevice& _device;
        ShaderUniformLayout _uniformLayout;
        bool _includesDataUniforms;
        VkDescriptorSetLayout _layout;
        std::vector<VkDescriptorSetLayoutBinding> _layoutBindings;
    };
}