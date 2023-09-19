#pragma once
#include "../../../Renderpch.h"
#include "CachedResources/VulkanRenderPass.h"
#include "CachedResources/VulkanRenderPassShader.h"
#include "CachedResources/VulkanPipeline.h"

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

		/// @brief The time since a resource's use that it should be considered stale
		static const double sPurgeThreshold;

	private:
		std::unordered_map<GraphicsDeviceResourceID, VulkanRenderPass> _renderPasses;
		std::unordered_map<GraphicsDeviceResourceID, VulkanRenderPassShader> _shaders;
		std::unordered_map<GraphicsDeviceResourceID, VulkanPipeline> _pipelines;
		TickListener _tickListener;

	public:
		VulkanGraphicsDeviceCache();
		~VulkanGraphicsDeviceCache();

		/// @brief Gets/creates a VulkanRenderPass
		/// @param pipeline The pipeline to use
		/// @return A render pass
		VulkanRenderPass& GetOrCreateRenderPass(CompiledRenderPipeline& pipeline);

		/// @brief Gets/creates a VulkanRenderPassShader 
		/// @param shaderInfo The shader info
		/// @return The shader
		VulkanRenderPassShader& GetOrCreateShader(const RenderPassShader& shaderInfo);

		/// @brief Gets/creates a VulkanPipeline
		/// @param renderPass The render pass
		/// @param subpassIndex The index of the render pass within the pipeline
		/// @param shader The shader
		/// @return The pipeline
		VulkanPipeline& GetOrPipeline(
			const VulkanRenderPass& renderPass,
			uint32 subpassIndex,
			const VulkanRenderPassShader& shader);

		/// @brief Purges all stale resources
		void PurgeStaleResources();

	private:
		/// @brief The tick callback
		/// @param currentTick Info about the current tick
		void TickCallback(const TickInfo& currentTick);
	};
}

