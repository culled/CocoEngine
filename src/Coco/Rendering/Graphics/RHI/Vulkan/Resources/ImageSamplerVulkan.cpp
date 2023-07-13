#include "ImageSamplerVulkan.h"

#include "../GraphicsDeviceVulkan.h"
#include "../VulkanUtilities.h"

namespace Coco::Rendering::Vulkan
{
	ImageSamplerVulkan::ImageSamplerVulkan(const ResourceID& id, const string& name, const ImageSamplerProperties& properties) :
		GraphicsResource<GraphicsDeviceVulkan, ImageSampler>(id, name, properties)
	{
		VkSamplerCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.magFilter = ToVkFilter(_properties.FilterMode);
		createInfo.minFilter = ToVkFilter(_properties.FilterMode);

		// TODO: individual repeat modes?
		createInfo.addressModeU = ToVkSamplerAddressMode(_properties.RepeatMode);
		createInfo.addressModeV = ToVkSamplerAddressMode(_properties.RepeatMode);
		createInfo.addressModeW = ToVkSamplerAddressMode(_properties.RepeatMode);

		createInfo.anisotropyEnable = _properties.MaxAnisotropy > 0;
		createInfo.maxAnisotropy = static_cast<float>(_properties.MaxAnisotropy);

		createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		createInfo.unnormalizedCoordinates = VK_FALSE;
		createInfo.compareEnable = VK_TRUE;
		createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		createInfo.mipmapMode = ToVkSamplerMipmapMode(_properties.FilterMode);
		createInfo.mipLodBias = 0.0f;
		createInfo.minLod = 0.0f;
		createInfo.maxLod = 0.0f;

		AssertVkResult(vkCreateSampler(_device->GetDevice(), &createInfo, nullptr, &_sampler));
	}

	ImageSamplerVulkan::~ImageSamplerVulkan()
	{
		_device->WaitForIdle();

		if (_sampler != nullptr)
			vkDestroySampler(_device->GetDevice(), _sampler, nullptr);

		_sampler = nullptr;
	}
}
