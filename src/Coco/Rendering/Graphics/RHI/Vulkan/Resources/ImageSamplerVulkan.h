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
		friend class ManagedRef<ImageSamplerVulkan>;

	private:
		VkSampler _sampler = nullptr;

	protected:
		ImageSamplerVulkan(const ResourceID& id, const string& name, const ImageSamplerProperties& properties);

	public:
		~ImageSamplerVulkan() final;

		DefineResourceType(ImageSamplerVulkan)

		/// @brief Gets the underlying VkSampler
		/// @return The underlying Vulkan sampler
		VkSampler GetSampler() noexcept { return _sampler; }
	};
}