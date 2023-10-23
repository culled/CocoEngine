#pragma once

#include "CachedVulkanResource.h"
#include "../VulkanIncludes.h"

namespace Coco::Rendering
{
	struct CompiledRenderPipeline;
}

namespace Coco::Rendering::Vulkan
{
	/// @brief Information for a VulkanRenderPass subpass
	struct VulkanSubpassInfo
	{
		/// @brief The color attachment descriptions
		std::vector<AttachmentFormat> ColorAttachments;

		/// @brief The color attachment references
		std::vector<VkAttachmentReference> ColorAttachmentReferences;

		/// @brief The depth-stencil attachment description
		std::optional<AttachmentFormat> DepthStencilAttachment;

		/// @brief The depth-stencil attachment reference
		std::optional<VkAttachmentReference> DepthStencilAttachmentReference;

		/// @brief The resolve attachment references
		std::vector<VkAttachmentReference> ResolveAttachmentReferences;

		/// @brief Indices of attachments to preserve
		std::vector<uint32_t> PreserveAttachments;

		/// @brief The subpass description
		VkSubpassDescription SubpassDescription;

		VulkanSubpassInfo();
	};

	/// @brief A Vulkan render pass created from a CompiledRenderPipeline
	class VulkanRenderPass : 
		public CachedVulkanResource
	{
	private:
		uint64 _version;
		VkRenderPass _renderPass;
		std::vector<VulkanSubpassInfo> _subpassInfos;
		MSAASamples _samples;

	public:
		VulkanRenderPass(const CompiledRenderPipeline& pipeline, MSAASamples samples);
		~VulkanRenderPass();

		/// @brief Makes a key from linked to a given pipeline
		/// @param pipeline The pipeline
		/// @param samples The number of MSAA samples
		/// @return A key
		static GraphicsDeviceResourceID MakeKey(const CompiledRenderPipeline& pipeline, MSAASamples samples);

		/// @brief Gets this render pass's version
		/// @return The version
		uint64 GetVersion() const { return _version; }

		/// @brief Gets the Vulkan render pass
		/// @return The Vulkan render pass
		VkRenderPass GetRenderPass() const { return _renderPass; }

		/// @brief Gets the number of MSAA samples for this pass
		/// @return The number of MSAA samples
		MSAASamples GetMSAASamples() const { return _samples; }

		/// @brief Gets the information for a subpass in the pipeline
		/// @param index The index of the pass in the pipeline
		/// @return The pass info
		const VulkanSubpassInfo& GetSubpassInfo(uint64 index) const;

		/// @brief Determines if this render pass needs to be updated
		/// @param pipeline The pipeline
		/// @param samples The number of MSAA samples
		/// @return True if this render pass needs to be updated
		bool NeedsUpdate(const CompiledRenderPipeline& pipeline, MSAASamples samples) const;

		/// @brief Updates this render pass from the given pipeline
		/// @param pipeline The pipeline
		/// @param samples The number of MSAA samples
		/// @param resolveAttachmentIndices Indices of extra attachments that pipeline attachments resolve to
		void Update(const CompiledRenderPipeline& pipeline, MSAASamples samples, std::span<const uint8> resolveAttachmentIndices);

	private:
		/// @brief Adjusts the given MSAA samples to fit the device and pipeline requirements
		/// @param pipeline The pipeline
		/// @param samples The samples
		/// @return The adjusted samples
		MSAASamples GetAdjustedSamples(const CompiledRenderPipeline& pipeline, MSAASamples samples) const;

		/// @brief Creates the render pass objects
		/// @param pipeline The pipeline to use
		/// @param samples The number of MSAA samples
		/// @param resolveAttachmentIndices Indices of extra attachments that pipeline attachments resolve to
		void CreateRenderPass(const CompiledRenderPipeline& pipeline, MSAASamples samples, std::span<const uint8> resolveAttachmentIndices);

		/// @brief Destroys the render pass objects
		void DestroyRenderPass();
	};
}