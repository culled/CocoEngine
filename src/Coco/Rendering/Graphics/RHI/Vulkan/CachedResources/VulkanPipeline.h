#pragma once
#include "CachedVulkanResource.h"
#include "../VulkanIncludes.h"
#include "VulkanRenderPass.h"
#include "VulkanRenderPassShader.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;

	/// @brief A Vulkan framebuffer
	class VulkanPipeline : 
		public CachedVulkanResource
	{
	private:
		uint64 _version;
		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;
		uint32 _subpassIndex;

	public:
		VulkanPipeline(
			const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader,
			uint32 subpassIndex);

		~VulkanPipeline();

		/// @brief Makes a key from a render pass, shader, and subpass index
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @param subpassIndex The subpass index
		/// @return A key created from the given items
		static GraphicsDeviceResourceID MakeKey(
			const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader,
			uint32 subpassIndex);

		/// @brief Gets this pipeline's version
		/// @return The version
		uint64 GetVersion() const { return _version; }

		/// @brief Gets the Vulkan pipeline
		/// @return The Vulkan pipeline
		VkPipeline GetPipeline() const { return _pipeline; }

		/// @brief Gets the Vulkan pipeline layout
		/// @return The Vulkan pipeline layout
		VkPipelineLayout GetPipelineLayout() const { return _pipelineLayout; }

		/// @brief Determines if this pipeline needs to be updated
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @return True if this pipeline should be updated
		bool NeedsUpdate(
			const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader);

		/// @brief Updates this pipeline
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @param subpassIndex The index of this pipeline within the RenderPipeline
		void Update(
			const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader,
			uint32 subpassIndex);

	private:
		/// @brief Creates a version from a render pass and shader
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @return A combined version
		static uint64 MakeVersion(
			const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader);

		/// @brief Creates the pipeline
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @param subpassIndex The index of this pipeline within the RenderPipeline
		void CreatePipeline(
			const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader,
			uint32 subpassIndex);

		/// @brief Destroys the pipeline
		void DestroyPipeline();
	};
}