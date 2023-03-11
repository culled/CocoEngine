#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Rendering/Graphics/IGraphicsFence.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
	class GraphicsDeviceVulkan;

	/// <summary>
	/// Vulkan-implementation of a GraphicsFence
	/// </summary>
	class GraphicsFenceVulkan : public IGraphicsFence
	{
	private:
		VkFence _fence = nullptr;
		GraphicsDeviceVulkan* _device;

	public:
		GraphicsFenceVulkan(GraphicsDevice* device, bool startSignalled);
		virtual ~GraphicsFenceVulkan() override;

		virtual bool IsSignalled() const override;
		virtual void Reset() override;
		virtual void Wait(uint64_t timeoutNs) override;

		/// <summary>
		/// Gets the underlying VkFence
		/// </summary>
		/// <returns>The underlying fence</returns>
		VkFence GetFence() const { return _fence; }
	};
}