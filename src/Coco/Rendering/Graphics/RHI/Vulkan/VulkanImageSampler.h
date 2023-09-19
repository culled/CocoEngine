#pragma once
#include "../../ImageSampler.h"
#include "../../GraphicsDeviceResource.h"

#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    /// @brief Vulkan-implementation of an ImageSampler
    class VulkanImageSampler :
        public ImageSampler,
        public GraphicsDeviceResource<VulkanGraphicsDevice>
    {
    private:
        ImageSamplerDescription _description;
        VkSampler _sampler;

    public:
        VulkanImageSampler(const GraphicsDeviceResourceID& id, const ImageSamplerDescription& description);
        ~VulkanImageSampler();

        ImageSamplerDescription GetDescription() const final { return _description; }

        /// @brief Gets the Vulkan sampler
        /// @return The Vulkan sampler
        VkSampler GetSampler() const { return _sampler; }
    };
}