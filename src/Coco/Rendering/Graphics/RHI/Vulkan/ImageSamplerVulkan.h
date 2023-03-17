#pragma once

#include <Coco/Rendering/Graphics/ImageSampler.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// <summary>
	/// Vulkan-implementation of an image sampler
	/// </summary>
	class ImageSamplerVulkan final : public ImageSampler
	{
	private:
		GraphicsDeviceVulkan* _device;
		VkSampler _sampler = nullptr;

	public:
		ImageSamplerVulkan(GraphicsDevice* device, Rendering::FilterMode filterMode, Rendering::RepeatMode repeatMode, uint maxAnisotropy);
		~ImageSamplerVulkan() final;

		/// <summary>
		/// Gets the underlying Vulkan sampler
		/// </summary>
		/// <returns>The underlying Vulkan sampler</returns>
		VkSampler GetSampler() const noexcept { return _sampler; }
	};
}