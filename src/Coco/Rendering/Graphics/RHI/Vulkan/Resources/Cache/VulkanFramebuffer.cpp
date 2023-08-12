#include "VulkanFramebuffer.h"

#include "../../GraphicsDeviceVulkan.h"
#include <Coco/Rendering/Graphics/RenderView.h>
#include "VulkanRenderPass.h"
#include "../ImageVulkan.h"

namespace Coco::Rendering::Vulkan
{
	VulkanFramebuffer::VulkanFramebuffer(const ResourceID& id, const string& name, const Ref<RenderPipeline>& pipeline) :
		GraphicsResource<GraphicsDeviceVulkan, RenderingResource>(id, name), CachedResource(pipeline->ID, pipeline->GetVersion()),
		_pipeline(pipeline)
	{}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		DestroyFramebuffer();
	}

	bool VulkanFramebuffer::NeedsUpdate(const Ref<RenderView>& renderView) const noexcept
	{
		return _framebuffer == nullptr || 
			_size != renderView->ViewportRect.Size || 
			_renderTargets.Any([](const auto& rt) { return !rt.IsValid(); }) || 
			GetReferenceVersion() != _pipeline->GetVersion();
	}

	void VulkanFramebuffer::Update(Ref<RenderView>& renderView, VulkanRenderPass& renderPass)
	{
		DestroyFramebuffer();

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass.GetRenderPass();
		framebufferInfo.width = static_cast<uint32_t>(renderView->ViewportRect.Size.Width);
		framebufferInfo.height = static_cast<uint32_t>(renderView->ViewportRect.Size.Height);
		framebufferInfo.layers = 1;

		List<VkImageView> renderTargets;
		for (RenderTarget& renderTarget : renderView->RenderTargets)
		{
			if (!renderTarget.Image.IsValid())
				throw VulkanRenderingException("Render target resource was invalid");

			ImageVulkan* vulkanImage = static_cast<ImageVulkan*>(renderTarget.Image.Get());
			renderTargets.Add(vulkanImage->GetNativeView());
			_renderTargets.Add(renderTarget.Image);
		}

		framebufferInfo.attachmentCount = static_cast<uint32_t>(renderTargets.Count());
		framebufferInfo.pAttachments = renderTargets.Data();

		AssertVkResult(vkCreateFramebuffer(_device->GetDevice(), &framebufferInfo, nullptr, &_framebuffer));
		_size = renderView->ViewportRect.Size;

		UpdateReferenceVersion(_pipeline->GetVersion());
		IncrementVersion();
	}

	void VulkanFramebuffer::DestroyFramebuffer() noexcept
	{
		if (_framebuffer != nullptr)
		{
			_device->WaitForIdle();
			vkDestroyFramebuffer(_device->GetDevice(), _framebuffer, nullptr);
		}

		_framebuffer = nullptr;
		_size = SizeInt::Zero;
		_renderTargets.Clear();
	}
}
