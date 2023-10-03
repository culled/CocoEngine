#pragma once
#include "CachedVulkanResource.h"
#include "../../../../Renderpch.h"
#include "VulkanFramebuffer.h"
#include "VulkanGlobalUniformData.h"
#include "VulkanShaderUniformData.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief A cache for a VulkanRenderContext
	class VulkanRenderContextCache : 
		public CachedVulkanResource
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
		std::unordered_map<uint64, VulkanGlobalUniformData> _globalUniformDatas;
		std::unordered_map<GraphicsDeviceResourceID, VulkanShaderUniformData> _shaderUniformDatas;

	public:
		VulkanRenderContextCache(const GraphicsDeviceResourceID& id);
		~VulkanRenderContextCache();

		/// @brief Gets/creates a framebuffer
		/// @param size The size of the image being rendered
		/// @param renderPass The render pass being used
		/// @param attachmentImages Images being used as attachments
		/// @return A framebuffer
		VulkanFramebuffer& GetOrCreateFramebuffer(const SizeInt& size, VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages);

		/// @brief Gets/creates non-shader specific uniform data
		/// @param layout The global layout of the uniforms
		/// @return The global uniform data
		VulkanGlobalUniformData& GetOrCreateGlobalUniformData(const GlobalShaderUniformLayout& layout);

		/// @brief Gets/creates shader-specific uniform data
		/// @param shader The shader for the uniform data
		/// @return The shader-specific uniform data
		VulkanShaderUniformData& GetOrCreateShaderUniformData(const VulkanRenderPassShader& shader);

		/// @brief Resets this cache for a new frame
		void ResetForNextFrame();

		/// @brief Purges all stale resources
		void PurgeStaleResources();
	};
}