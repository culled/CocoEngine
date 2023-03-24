#pragma once

#include <Coco/Rendering/Graphics/Resources/IGraphicsSemaphore.h>

#include "../VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// @brief Vulkan-implementation of a GraphicsSemaphore
	class GraphicsSemaphoreVulkan final : public IGraphicsSemaphore
	{
	private:
		VkSemaphore _semaphore = nullptr;
		GraphicsDeviceVulkan* _device;

	public:
		GraphicsSemaphoreVulkan(GraphicsDevice* device);
		~GraphicsSemaphoreVulkan() final;

		/// @brief Gets the underlying VkSemaphore
		/// @return The underlying Vulkan semaphore
		VkSemaphore GetSemaphore() const noexcept { return _semaphore; }
	};
}