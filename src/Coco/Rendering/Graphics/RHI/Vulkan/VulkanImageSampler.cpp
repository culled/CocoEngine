#include "Renderpch.h"
#include "VulkanImageSampler.h"

#include "VulkanGraphicsDevice.h"
#include "VulkanUtils.h"

namespace Coco::Rendering::Vulkan
{
	VulkanImageSampler::VulkanImageSampler(const GraphicsResourceID& id, VulkanGraphicsDevice& device, const ImageSamplerDescription description) :
		ImageSampler(id),
		_device(device),
		_description(description)
	{
		const GraphicsDeviceFeatures& features = _device.GetFeatures();
		_description.MaxAnisotropy = Math::Min(_description.MaxAnisotropy, features.MaxAnisotropicLevel);

		VkSamplerCreateInfo createInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		createInfo.magFilter = ToVkFilter(_description.MagnifyFilter);
		createInfo.minFilter = ToVkFilter(_description.MinimizeFilter);

		createInfo.addressModeU = ToVkSamplerAddressMode(_description.RepeatModeU);
		createInfo.addressModeV = ToVkSamplerAddressMode(_description.RepeatModeV);
		createInfo.addressModeW = ToVkSamplerAddressMode(_description.RepeatModeW);

		createInfo.anisotropyEnable = _description.MaxAnisotropy > 0;
		createInfo.maxAnisotropy = static_cast<float>(_description.MaxAnisotropy);

		createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		createInfo.unnormalizedCoordinates = VK_FALSE;
		createInfo.compareEnable = VK_TRUE;
		createInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		createInfo.mipmapMode = ToVkSamplerMipmapMode(_description.MipMapFilter);
		createInfo.mipLodBias = static_cast<float>(_description.LODBias);
		createInfo.minLod = static_cast<float>(_description.MinLOD);
		createInfo.maxLod = static_cast<float>(_description.MaxLOD);

		AssertVkSuccess(vkCreateSampler(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &_sampler));
	}

	VulkanImageSampler::~VulkanImageSampler()
	{
		if (_sampler)
		{
			_device.WaitForIdle();
			vkDestroySampler(_device.GetDevice(), _sampler, _device.GetAllocationCallbacks());
			_sampler = nullptr;
		}
	}
}