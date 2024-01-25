#pragma once

#include "../VulkanRenderFrame.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanRenderFramePool
	{
	public:
		VulkanRenderFramePool(VulkanGraphicsDevice& device);
		~VulkanRenderFramePool();

		SharedRef<VulkanRenderFrame> Get();

	private:
		VulkanGraphicsDevice& _device;

		std::vector<SharedRef<VulkanRenderFrame>> _frames;
	};
}