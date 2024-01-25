#pragma once
#include "../../ImageSampler.h"

#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    class VulkanImageSampler :
        public ImageSampler
    {
    public:
        VulkanImageSampler(const GraphicsResourceID& id, VulkanGraphicsDevice& device, const ImageSamplerDescription description);
        ~VulkanImageSampler();

        // Inherited via ImageSampler
        const ImageSamplerDescription& GetDescription() const override { return _description; }

        /// @brief Gets the Vulkan sampler
        /// @return The Vulkan sampler
        VkSampler GetSampler() const { return _sampler; }

    private:
        VulkanGraphicsDevice& _device;
        ImageSamplerDescription _description;
        VkSampler _sampler;
    };
}