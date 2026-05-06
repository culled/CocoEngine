//
// Created by cullen on 3/28/26.
//

#include "VulkanImageSampler.h"

#include "Coco/Core/Engine.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanGraphicsPlatform.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanUtils.h"

namespace Coco
{
    VulkanImageSampler::VulkanImageSampler(uint64 id, VulkanGraphicsPlatform* platform,
        const ImageSamplerDescription& samplerDescription) :
        ImageSampler(id),
        _platform(platform),
        _description(samplerDescription),
        _sampler(nullptr)
    {
        VkSamplerCreateInfo createInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        createInfo.minFilter = VulkanUtils::ToVkFilter(_description.MinimizeFilter);
        createInfo.magFilter = VulkanUtils::ToVkFilter(_description.MagnifyFilter);

        createInfo.addressModeU = VulkanUtils::ToVkSamplerAddressMode(_description.RepeatModeU);
        createInfo.addressModeV = VulkanUtils::ToVkSamplerAddressMode(_description.RepeatModeV);
        createInfo.addressModeW = VulkanUtils::ToVkSamplerAddressMode(_description.RepeatModeW);

        createInfo.anisotropyEnable = _description.MaxAnisotropy > 0;
        createInfo.maxAnisotropy = static_cast<float>(_description.MaxAnisotropy);

        createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        createInfo.unnormalizedCoordinates = VK_FALSE;
        createInfo.compareEnable = VK_TRUE;
        createInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        createInfo.mipmapMode = VulkanUtils::ToVkSamplerMipmapMode(_description.MipMapFilter);
        createInfo.mipLodBias = _description.LODBias;
        createInfo.minLod = static_cast<float>(_description.MinLOD);
        createInfo.maxLod = static_cast<float>(_description.MaxLOD);

        AssertVkSuccess(vkCreateSampler(_platform->GetDevice(), &createInfo, _platform->GetAllocationCallbacks(), &_sampler));

        COCO_ENGINE_LOG_VERBOSE("Created VulkanImageSampler %u", id);
    }

    VulkanImageSampler::~VulkanImageSampler()
    {
        if (_sampler)
        {
            vkDestroySampler(_platform->GetDevice(), _sampler, _platform->GetAllocationCallbacks());
            _sampler = nullptr;
        }

        COCO_ENGINE_LOG_VERBOSE("Destroyed VulkanImageSampler %u", GetID());
    }
} // Coco