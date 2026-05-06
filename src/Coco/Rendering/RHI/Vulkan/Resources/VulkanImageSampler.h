//
// Created by cullen on 3/28/26.
//

#ifndef COCOENGINE_VULKANIMAGESAMPLER_H
#define COCOENGINE_VULKANIMAGESAMPLER_H
#include "Coco/Rendering/Graphics/Resources/ImageSampler.h"
#include "Coco/Rendering/Graphics/Resources/ImageSamplerTypes.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"

namespace Coco
{
    class VulkanGraphicsPlatform;

    class VulkanImageSampler : public ImageSampler
    {
    public:
        VulkanImageSampler(uint64 id, VulkanGraphicsPlatform* platform, const ImageSamplerDescription& samplerDescription);
        ~VulkanImageSampler();

        VkSampler GetSampler() const { return _sampler; }

    private:
        VulkanGraphicsPlatform* _platform;
        ImageSamplerDescription _description;
        VkSampler _sampler;
    };
} // Coco

#endif //COCOENGINE_VULKANIMAGESAMPLER_H