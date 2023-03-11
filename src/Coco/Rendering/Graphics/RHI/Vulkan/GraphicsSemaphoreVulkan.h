#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Rendering/Graphics/IGraphicsSemaphore.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
	class GraphicsDeviceVulkan;

	/// <summary>
	/// Vulkan-implementation of a GraphicsSemaphore
	/// </summary>
	class GraphicsSemaphoreVulkan : public IGraphicsSemaphore
	{
	private:
		VkSemaphore _semaphore = nullptr;
		GraphicsDeviceVulkan* _device;

	public:
		GraphicsSemaphoreVulkan(GraphicsDevice* device);
		virtual ~GraphicsSemaphoreVulkan() override;

		/// <summary>
		/// Gets the underlying VkSemaphore
		/// </summary>
		/// <returns>The underlying Vulkan semaphore</returns>
		VkSemaphore GetSemaphore() const { return _semaphore; }
	};
}