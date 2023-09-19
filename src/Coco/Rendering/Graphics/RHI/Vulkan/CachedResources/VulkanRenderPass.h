#pragma once

#include "../../../GraphicsDeviceResource.h"
#include "../VulkanIncludes.h"
#include "../../../../Pipeline/CompiledRenderPipeline.h"

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

	/// @brief A Vulkan render pass created from a CompiledRenderPipeline
	class VulkanRenderPass : public GraphicsDeviceResource<VulkanGraphicsDevice>
	{
	private:
		uint64 _version;
		VkRenderPass _renderPass;
		std::vector<AttachmentFormat> _attachments;
		std::vector<VulkanSubpassInfo> _subpassInfos;
		MSAASamples _multisamplingMode;
		double _lastUsedTime;

	public:
		VulkanRenderPass(const CompiledRenderPipeline& pipeline);
		~VulkanRenderPass();

		/// @brief Makes a key from linked to a given pipeline
		/// @param pipeline The pipeline
		/// @return A key
		static GraphicsDeviceResourceID MakeKey(const CompiledRenderPipeline& pipeline);

		/// @brief Gets this render pass's version
		/// @return The version
		uint64 GetVersion() const { return _version; }

		/// @brief Gets the Vulkan render pass
		/// @return The Vulkan render pass
		VkRenderPass GetRenderPass() const { return _renderPass; }

		/// @brief Gets the multisampling mode of this render pass
		/// @return The multisampling mode
		MSAASamples GetMultisamplingMode() const { return _multisamplingMode; }

		/// @brief Gets the information for a subpass in the pipeline
		/// @param index The index of the pass in the pipeline
		/// @return The pass info
		const VulkanSubpassInfo& GetSubpassInfo(uint64 index) const;

		/// @brief Determines if this render pass needs to be updated
		/// @param pipeline The pipeline
		/// @return True if this render pass needs to be updated
		bool NeedsUpdate(const CompiledRenderPipeline& pipeline) const;

		/// @brief Updates this render pass from the given pipeline
		/// @param pipeline The pipeline
		void Update(const CompiledRenderPipeline& pipeline);

		/// @brief Marks this render pass as used
		void Use();

		/// @brief Determines if this pass is stale and can be purged
		/// @return True if this pass can be purged
		bool IsStale() const;

	private:
		/// @brief Creates the render pass objects
		/// @param pipeline The pipeline to use
		void CreateRenderPass(const CompiledRenderPipeline& pipeline);

		/// @brief Destroys the render pass objects
		void DestroyRenderPass();
	};
}