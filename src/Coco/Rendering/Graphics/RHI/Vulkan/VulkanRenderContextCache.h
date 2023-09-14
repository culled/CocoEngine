#pragma once
#include "../../../Renderpch.h"
#include "VulkanFramebuffer.h"

#include <Coco/Core/MainLoop/TickListener.h>

namespace Coco::Rendering::Vulkan
{
	/// @brief A cache for a VulkanRenderContext
	class VulkanRenderContextCache
	{
	public:
		/// @brief The priority of this cache's purge tick listener
		static const int sPurgePriority;

		/// @brief The period between purges in seconds
		static const double sPurgePeriod;

		/// @brief The time since a resource's use that it should be considered stale
		static const double sPurgeThreshold;

	private:
		std::unordered_map<VulkanFramebufferKey, VulkanFramebuffer> _framebuffers;
		TickListener _tickListener;

	public:
		VulkanRenderContextCache();
		~VulkanRenderContextCache();

		/// @brief Gets/creates a framebuffer
		/// @param size The size of the image being rendered
		/// @param renderPass The render pass being used
		/// @param attachmentImages Images being used as attachments
		/// @return A framebuffer
		VulkanFramebuffer& GetOrCreateFramebuffer(const SizeInt& size, VulkanRenderPass& renderPass, const std::vector<VulkanImage*>& attachmentImages);

		/// @brief Purges all stale resources
		void PurgeStaleResources();

	private:
		/// @brief The tick callback
		/// @param currentTick Info about the current tick
		void TickCallback(const TickInfo& currentTick);
	};
}