#include "ImageSamplerVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include "VulkanUtilities.h"

namespace Coco::Rendering
{
	ImageSamplerVulkan::ImageSamplerVulkan(GraphicsDevice* device, Rendering::FilterMode filterMode, Rendering::RepeatMode repeatMode, uint maxAnisotropy) : 
		ImageSampler(filterMode, repeatMode, maxAnisotropy), _device(static_cast<GraphicsDeviceVulkan*>(device))
	{
		VkSamplerCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.magFilter = ToVkFilter(FilterMode);
		createInfo.minFilter = ToVkFilter(FilterMode);

		// TODO: individual repeat modes?
		createInfo.addressModeU = ToVkSamplerAddressMode(RepeatMode);
		createInfo.addressModeV = ToVkSamplerAddressMode(RepeatMode);
		createInfo.addressModeW = ToVkSamplerAddressMode(RepeatMode);

		createInfo.anisotropyEnable = maxAnisotropy > 0;
		createInfo.maxAnisotropy = static_cast<float>(maxAnisotropy);

		createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		createInfo.unnormalizedCoordinates = VK_FALSE;
		createInfo.compareEnable = VK_TRUE;
		createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		createInfo.mipmapMode = ToVkSamplerMipmapMode(FilterMode);
		createInfo.mipLodBias = 0.0f;
		createInfo.minLod = 0.0f;
		createInfo.maxLod = 0.0f;

		AssertVkResult(vkCreateSampler(_device->GetDevice(), &createInfo, nullptr, &_sampler));
	}

	ImageSamplerVulkan::~ImageSamplerVulkan()
	{
		if (_sampler != nullptr)
			vkDestroySampler(_device->GetDevice(), _sampler, nullptr);

		_sampler = nullptr;
	}
}
