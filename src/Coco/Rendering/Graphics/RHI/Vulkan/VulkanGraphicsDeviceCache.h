#pragma once
#include "../../../Renderpch.h"
#include "CachedResources/VulkanRenderPass.h"
#include "CachedResources/VulkanPipeline.h"
#include "CachedResources/VulkanRenderContextCache.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief A cache for a VulkanGraphicsDevice
	class VulkanGraphicsDeviceCache
	{
	public:
		/// @brief The period between purges in seconds
		static const double sPurgePeriod;

		/// @brief The time since a resource's use that it should be considered stale
		static const double sPurgeThreshold;

	private:
		double _lastPurgeTime;

		std::unordered_map<GraphicsDeviceResourceID, VulkanRenderPass> _renderPasses;
		std::unordered_map<GraphicsDeviceResourceID, VulkanPipeline> _pipelines;
		std::unordered_map<GraphicsDeviceResourceID, VulkanRenderContextCache> _contextCaches;

	public:
		VulkanGraphicsDeviceCache();
		~VulkanGraphicsDeviceCache();

		/// @brief Gets/creates a VulkanRenderPass
		/// @param pipeline The pipeline to use
		/// @param samples The number of MSAA samples
		/// @param resolveAttachmentIndices Indices of attachments in the pipeline that need to be resolved
		/// @return A render pass
		VulkanRenderPass& GetOrCreateRenderPass(const CompiledRenderPipeline& pipeline, MSAASamples samples, std::span<const uint8> resolveAttachmentIndices);

		/// @brief Gets/creates a VulkanPipeline
		/// @param renderPass The render pass
		/// @param subpassIndex The index of the render pass within the pipeline
		/// @param shader The shader
		/// @param globalDescriptorSetLayout The descriptor set layout for the global uniforms, if any
		/// @return The pipeline
		VulkanPipeline& GetOrCreatePipeline(
			const VulkanRenderPass& renderPass,
			uint32 subpassIndex,
			const VulkanShaderVariant& shader,
			const VulkanDescriptorSetLayout* globalDescriptorSetLayout);

		/// @brief Gets/creates a cache for a RenderContext
		/// @param id The ID of the context 
		/// @return The cache
		VulkanRenderContextCache& GetOrCreateContextCache(const GraphicsDeviceResourceID& id);

		/// @brief Resets this cache for a new frame
		void ResetForNextFrame();

		/// @brief Purges all stale resources
		void PurgeStaleResources();
	};
}

