#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Rendering/Graphics/GraphicsFence.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDeviceVulkan;

	/// <summary>
	/// Vulkan-implementation of a GraphicsFence
	/// </summary>
	class GraphicsFenceVulkan : public GraphicsFence
	{
	private:
		VkFence _fence = nullptr;
		GraphicsDeviceVulkan* _device;

	public:
		GraphicsFenceVulkan(GraphicsDeviceVulkan* owningDevice, bool startSignalled);
		virtual ~GraphicsFenceVulkan() override;

		virtual bool IsSignalled() const override;
		virtual void Reset() override;
		virtual void Wait(unsigned long long timeoutNs) override;
		virtual void* GetResource() const override { return _fence; }

		/// <summary>
		/// Gets the underlying VkFence
		/// </summary>
		/// <returns>The underlying fence</returns>
		VkFence GetFence() const { return _fence; }
	};
}