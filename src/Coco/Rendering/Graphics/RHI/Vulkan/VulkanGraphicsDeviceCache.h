#pragma once
#include "../../../Renderpch.h"
#include "VulkanRenderPass.h"

#include <Coco/Core/MainLoop/TickListener.h>

namespace Coco::Rendering::Vulkan
{
	/// @brief A cache for a VulkanGraphicsDevice
	class VulkanGraphicsDeviceCache
	{
	public:
		/// @brief The priority of this cache's purge tick listener
		static const int sPurgePriority;

		/// @brief The period between purges in seconds
		static const double sPurgePeriod;

	private:
		std::unordered_map<VulkanRenderPassKey, VulkanRenderPass> _renderPasses;
		TickListener _tickListener;

	public:
		VulkanGraphicsDeviceCache();
		~VulkanGraphicsDeviceCache();

		/// @brief Gets/creates a render pass
		/// @param pipeline The pipeline to use
		/// @return A render pass
		VulkanRenderPass& GetOrCreateRenderPass(CompiledRenderPipeline& pipeline);

		/// @brief Purges all stale resources
		void PurgeStaleResources();

	private:
		/// @brief The tick callback
		/// @param currentTick Info about the current tick
		void TickCallback(const TickInfo& currentTick);
	};
}

