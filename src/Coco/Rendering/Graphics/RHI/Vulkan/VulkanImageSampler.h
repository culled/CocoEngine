#pragma once
#include "../../ImageSampler.h"
#include "../../GraphicsDeviceResource.h"

#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

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

        VkSampler GetSampler() const { return _sampler; }
    };
}