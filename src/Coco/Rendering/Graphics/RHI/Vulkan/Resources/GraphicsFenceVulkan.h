#pragma once

#include <Coco/Rendering/Graphics/Resources/GraphicsFence.h>
#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>

#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// @brief Vulkan-implementation of a GraphicsFence
	class GraphicsFenceVulkan final : public GraphicsResource<GraphicsDeviceVulkan, GraphicsFence>
	{
	private:
		VkFence _fence = nullptr;

	public:
		GraphicsFenceVulkan(const ResourceID& id, const string& name, bool startSignalled);
		~GraphicsFenceVulkan() final;

		DefineResourceType(GraphicsFenceVulkan)

		bool IsSignalled() const noexcept final;
		void Reset() final;
		void Wait(uint64_t timeoutNs) final;

		/// @brief Gets the underlying VkFence
		/// @return The underlying fence
		VkFence GetFence() { return _fence; }
	};
}