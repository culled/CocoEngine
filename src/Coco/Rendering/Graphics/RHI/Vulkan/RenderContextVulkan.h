#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Rendering/Graphics/RenderContext.h>

#include "VulkanIncludes.h"

#include "CommandBufferVulkan.h"

namespace Coco::Rendering
{
	class RenderPipeline;
	class GraphicsDeviceVulkan;

	/// <summary>
	/// Vulkan-implementation of a RenderContext
	/// </summary>
	class RenderContextVulkan : public RenderContext
	{
	private:
		GraphicsDeviceVulkan* _device;
		VkFramebuffer _framebuffer = nullptr;
		VkRenderPass _renderPass = nullptr;
		CommandBufferVulkan* _commandBuffer;

	public:
		RenderContextVulkan(Rendering::RenderView* renderView, GraphicsDeviceVulkan* device, const Ref<RenderPipeline>& pipeline, List<VkImageView> views);
		virtual ~RenderContextVulkan() override;
	};
}