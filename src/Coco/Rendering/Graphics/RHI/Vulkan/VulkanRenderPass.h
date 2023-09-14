#pragma once

#include "../../GraphicsDeviceResource.h"
#include "VulkanIncludes.h"
#include "../../../Pipeline/CompiledRenderPipeline.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;

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

		/// @brief Indices of attachments to preserve
		std::vector<uint32_t> PreserveAttachments;

		/// @brief The subpass description
		VkSubpassDescription SubpassDescription;

		VulkanSubpassInfo();
	};

	/// @brief A key that can be used to identify a VulkanRenderPass
	using VulkanRenderPassKey = uint64;

	/// @brief A Vulkan render pass created from a CompiledRenderPipeline
	class VulkanRenderPass : GraphicsDeviceResource<VulkanGraphicsDevice>
	{
	private:
		VulkanRenderPassKey _key;
		VkRenderPass _renderPass;
		std::vector<VulkanSubpassInfo> _subpassInfos;
		double _lastUsedTime;

	public:
		VulkanRenderPass(CompiledRenderPipeline& pipeline);
		~VulkanRenderPass();

		/// @brief Makes a key from linked to a given pipeline
		/// @param pipeline The pipeline
		/// @return A key
		static VulkanRenderPassKey MakeKey(CompiledRenderPipeline& pipeline);

		/// @brief Gets this RenderPass's key
		/// @return The key
		VulkanRenderPassKey GetKey() const { return _key; }

		/// @brief Gets the Vulkan render pass
		/// @return The Vulkan render pass
		VkRenderPass GetRenderPass() const { return _renderPass; }

		/// @brief Marks this render pass as used
		void Use();

		/// @brief Determines if this pass is stale and can be purged
		/// @return True if this pass can be purged
		bool IsStale() const;
	};
}