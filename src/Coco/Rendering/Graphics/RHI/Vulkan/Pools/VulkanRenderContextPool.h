#pragma once

#include "../VulkanRenderContext.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanRenderContextPool
	{
	public:
		VulkanRenderContextPool(VulkanGraphicsDevice& device);
		~VulkanRenderContextPool();

		Ref<VulkanRenderContext> Get();

	private:
		VulkanGraphicsDevice& _device;

		std::vector<Ref<VulkanRenderContext>> _contexts;
	};
}