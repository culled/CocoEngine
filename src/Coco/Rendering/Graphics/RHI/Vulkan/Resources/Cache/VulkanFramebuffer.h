#pragma once

#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>
#include <Coco/Core/Resources/CachedResource.h>

#include <Coco/Core/Types/Size.h>
#include "../../VulkanIncludes.h"

namespace Coco::Rendering
{
	struct RenderView;
	class Image;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class VulkanRenderPass;

	/// @brief A cached Vulkan framebuffer
	class VulkanFramebuffer final : public GraphicsResource<GraphicsDeviceVulkan, RenderingResource>, public CachedResource
	{
		Ref<RenderPipeline> _pipeline;
		List<Ref<Image>> _renderTargets;
		SizeInt _size = SizeInt::Zero;
		VkFramebuffer _framebuffer = nullptr;

	public:
		VulkanFramebuffer(ResourceID id, const string& name, uint64_t lifetime, const Ref<RenderPipeline>& pipeline);
		virtual ~VulkanFramebuffer();

		DefineResourceType(VulkanFramebuffer)

		void ReBind(const Ref<RenderPipeline>& pipeline);

		bool IsValid() const noexcept final { return _pipeline.IsValid(); }
		bool NeedsUpdate() const noexcept final { return false; }

		/// @brief Checks if this cached framebuffer needs to be updated
		/// @param framebufferSize The desired size of the framebuffer
		/// @return True if this resource should be updated
		bool NeedsUpdate(const Ref<RenderView>& renderView) const noexcept;

		void Update(const Ref<RenderView>& renderView, VulkanRenderPass* renderPass);

		VkFramebuffer GetFramebuffer() { return _framebuffer; }

		/// @brief Destroys the framebuffer
		void DestroyFramebuffer() noexcept;
	};
}