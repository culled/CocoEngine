#pragma once

#include <Coco/Rendering/RenderingResource.h>
#include <Coco/Core/Resources/CachedResource.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Rendering/Graphics/AttachmentDescription.h>
#include "../../VulkanIncludes.h"

namespace Coco::Rendering
{
	class RenderPipeline;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// @brief Information for a VulkanRenderPass subpass
	struct SubpassInfo
	{
		/// @brief The color attachment descriptions
		List<AttachmentDescription> ColorAttachments;

		/// @brief The color attachment references
		List<VkAttachmentReference> ColorAttachmentReferences;

		/// @brief The depth-stencil attachment description
		Optional<AttachmentDescription> DepthStencilAttachment;

		/// @brief The depth-stencil attachment reference
		Optional<VkAttachmentReference> DepthStencilAttachmentReference;

		/// @brief Indices of attachments to preserve
		List<uint32_t> PreserveAttachments;

		/// @brief The subpass description
		VkSubpassDescription SubpassDescription = {};
	};

	/// @brief A cached Vulkan render pass
	class VulkanRenderPass final : public RenderingResource, public CachedResource
	{
	private:
		GraphicsDeviceVulkan* _device;
		List<SubpassInfo> _subpassInfos;
		VkRenderPass _renderPass = nullptr;
		Ref<RenderPipeline> _pipeline;

	public:
		VulkanRenderPass(ResourceID id, const string& name, uint64_t tickLifetime, GraphicsDeviceVulkan* device, const Ref<RenderPipeline>& pipeline);
		~VulkanRenderPass() final;

		DefineResourceType(VulkanRenderPass)

		bool IsValid() const noexcept final { return _pipeline.IsValid(); }
		bool NeedsUpdate() const noexcept final;

		void ReBind(const Ref<RenderPipeline>& pipeline);

		/// @brief Updates this render pass to reflect the current version of its pipeline
		void Update();

		/// @brief Gets the SubpassInfos for this render pass
		/// @return This render pass's SubpassInfos
		const List<SubpassInfo>& GetSubpassInfos() const noexcept { return _subpassInfos; }

		/// @brief Gets a SubpassInfo for the specific render pass index
		/// @param index The render pass index in the pipeline
		/// @return The SubpassInfo
		const SubpassInfo& GetSubpassInfo(uint index) const noexcept { return _subpassInfos[index]; }

		/// @brief Gets the underlying VkRenderPass
		/// @return The Vulkan render pass
		VkRenderPass GetRenderPass() const noexcept { return _renderPass; }

	private:
		/// @brief Creates the render pass object
		void CreateRenderPass();

		/// @brief Destroys the render pass object
		void DestroyRenderPass();
	};
}