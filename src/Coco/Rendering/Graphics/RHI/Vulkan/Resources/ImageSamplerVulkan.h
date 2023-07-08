#pragma once

#include <Coco/Rendering/Graphics/Resources/ImageSampler.h>
#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>

#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// @brief Vulkan-implementation of an image sampler
	class ImageSamplerVulkan final : public GraphicsResource<GraphicsDeviceVulkan, ImageSampler>
	{
	private:
		VkSampler _sampler = nullptr;

	public:
		ImageSamplerVulkan(ResourceID id, const string& name, const ImageSamplerProperties& properties);
		~ImageSamplerVulkan() final;

		DefineResourceType(ImageSamplerVulkan)

		/// @brief Gets the underlying VkSampler
		/// @return The underlying Vulkan sampler
		VkSampler GetSampler() const noexcept { return _sampler; }
	};
}