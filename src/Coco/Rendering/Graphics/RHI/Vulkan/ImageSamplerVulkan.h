#pragma once

#include <Coco/Rendering/Graphics/ImageSampler.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
	class GraphicsDeviceVulkan;

	class COCOAPI ImageSamplerVulkan : public ImageSampler
	{
	private:
		GraphicsDeviceVulkan* _device;
		VkSampler _sampler = nullptr;

	public:
		ImageSamplerVulkan(GraphicsDevice* device, Rendering::FilterMode filterMode, Rendering::RepeatMode repeatMode, uint maxAnisotropy);
		virtual ~ImageSamplerVulkan() override;

		VkSampler GetSampler() const { return _sampler; }
	};
}