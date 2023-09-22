#pragma once
#include "../../../../Renderpch.h"
#include "VulkanFramebuffer.h"
#include "VulkanUniformData.h"

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
		std::unordered_map<GraphicsDeviceResourceID, VulkanFramebuffer> _framebuffers;
		std::unordered_map<GraphicsDeviceResourceID, VulkanUniformData> _uniformDatas;
		TickListener _tickListener;
		double _lastUsedTime;

	public:
		VulkanRenderContextCache();
		~VulkanRenderContextCache();

		/// @brief Gets/creates a framebuffer
		/// @param size The size of the image being rendered
		/// @param renderPass The render pass being used
		/// @param attachmentImages Images being used as attachments
		/// @return A framebuffer
		VulkanFramebuffer& GetOrCreateFramebuffer(const SizeInt& size, VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages);

		/// @brief Gets/creates a uniform data container for a shader
		/// @param shader The shader
		/// @return The uniform data container
		VulkanUniformData& GetOrCreateUniformData(const VulkanRenderPassShader& shader);

		/// @brief Resets this cache for a new frame
		void ResetForNextFrame();

		/// @brief Purges all stale resources
		void PurgeStaleResources();

		/// @brief Marks this framebuffer as used
		void Use();

		/// @brief Determines if this framebuffer is stale and can be purged
		/// @return True if this framebuffer can be purged
		bool IsStale() const;

	private:
		/// @brief The tick callback
		/// @param currentTick Info about the current tick
		void TickCallback(const TickInfo& currentTick);
	};
}