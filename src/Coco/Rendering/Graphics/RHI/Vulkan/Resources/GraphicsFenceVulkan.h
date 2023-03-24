#pragma once

#include <Coco/Rendering/Graphics/Resources/IGraphicsFence.h>

#include "../VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// @brief Vulkan-implementation of a GraphicsFence
	class GraphicsFenceVulkan final : public IGraphicsFence
	{
	private:
		VkFence _fence = nullptr;
		GraphicsDeviceVulkan* _device;

	public:
		GraphicsFenceVulkan(GraphicsDevice* device, bool startSignalled);
		~GraphicsFenceVulkan() final;

		bool IsSignalled() const noexcept final;
		void Reset() final;
		void Wait(uint64_t timeoutNs) final;

		/// @brief Gets the underlying VkFence
		/// @return The underlying fence
		VkFence GetFence() const { return _fence; }
	};
}