#pragma once

#include <Coco/Rendering/Graphics/Resources/ImageSampler.h>

#include "../VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// @brief Vulkan-implementation of an image sampler
	class ImageSamplerVulkan final : public ImageSampler
	{
	private:
		GraphicsDeviceVulkan* _device;
		VkSampler _sampler = nullptr;

	public:
		ImageSamplerVulkan(GraphicsDevice* device, Rendering::FilterMode filterMode, Rendering::RepeatMode repeatMode, uint maxAnisotropy);
		~ImageSamplerVulkan() final;

		/// @brief Gets the underlying VkSampler
		/// @return The underlying Vulkan sampler
		VkSampler GetSampler() const noexcept { return _sampler; }
	};
}