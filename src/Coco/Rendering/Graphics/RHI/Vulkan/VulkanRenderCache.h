#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/Optional.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDeviceVulkan;
	class RenderPipeline;

	/// <summary>
	/// Information for a RenderPass Subpass
	/// </summary>
	struct SubpassInfo
	{
		List<VkAttachmentReference> ColorAttachmentReferences;
		Optional<VkAttachmentReference> DepthStencilAttachmentReference;
		List<uint32_t> PreserveAttachments;

		VkSubpassDescription SubpassDescription = {};
	};

	/// <summary>
	/// A cache for Vulkan pipeline objects
	/// </summary>
	class VulkanRenderCache
	{
	private:
		GraphicsDeviceVulkan* _device;

		Map<Ref<RenderPipeline>, VkRenderPass> _renderPassCache;

	public:
		VulkanRenderCache(GraphicsDeviceVulkan* device);
		~VulkanRenderCache();

		/// <summary>
		/// Clears all cached objects
		/// </summary>
		void Invalidate();

		/// <summary>
		/// Gets or creates a VkRenderPass for a RenderPipeline
		/// </summary>
		/// <param name="renderPipeline">The render pipeline</param>
		/// <returns>The renderpass to use</returns>
		VkRenderPass GetOrCreateRenderPass(const Ref<RenderPipeline>& renderPipeline);

	private:
		/// <summary>
		/// Creates a VkRenderPass for a RenderPipeline
		/// </summary>
		/// <param name="renderPipeline">The render pipeline</param>
		/// <returns>The renderpass to use</returns>
		VkRenderPass CreateRenderPass(const Ref<RenderPipeline>& renderPipeline);
	};
}