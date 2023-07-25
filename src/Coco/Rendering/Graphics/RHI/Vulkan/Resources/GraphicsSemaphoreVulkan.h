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
		friend class ManagedRef<GraphicsSemaphoreVulkan>;

	private:
		VkSemaphore _semaphore = nullptr;

	protected:
		GraphicsSemaphoreVulkan(const ResourceID& id, const string& name);

	public:
		~GraphicsSemaphoreVulkan() final;

		DefineResourceType(GraphicsSemaphoreVulkan)

		/// @brief Gets the underlying VkSemaphore
		/// @return The underlying Vulkan semaphore
		VkSemaphore GetSemaphore() noexcept { return _semaphore; }
	};
}