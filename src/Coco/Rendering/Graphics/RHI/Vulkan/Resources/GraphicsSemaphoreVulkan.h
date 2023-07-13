#pragma once

#include <Coco/Rendering/Graphics/Resources/GraphicsSemaphore.h>
#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>

#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// @brief Vulkan-implementation of a GraphicsSemaphore
	class GraphicsSemaphoreVulkan final : public GraphicsResource<GraphicsDeviceVulkan, GraphicsSemaphore>
	{
	private:
		VkSemaphore _semaphore = nullptr;

	public:
		GraphicsSemaphoreVulkan(const ResourceID& id, const string& name);
		~GraphicsSemaphoreVulkan() final;

		DefineResourceType(GraphicsSemaphoreVulkan)

		/// @brief Gets the underlying VkSemaphore
		/// @return The underlying Vulkan semaphore
		VkSemaphore GetSemaphore() noexcept { return _semaphore; }
	};
}