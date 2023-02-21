#include "RenderContextVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>

namespace Coco::Rendering
{
	RenderContextVulkan::RenderContextVulkan(
		Rendering::RenderView* renderView, 
		GraphicsDeviceVulkan* device, 
		const Ref<RenderPipeline>& pipeline, 
		List<VkImageView> views) : 
		RenderContext(renderView),
		_device(device)
	{
		if (!_device->GetGraphicsCommandPool().has_value())
			throw Exception("A graphics command pool needs to be created for rendering");

		_renderPass = _device->GetRenderCache()->GetOrCreateRenderPass(pipeline);

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = _renderPass;
		framebufferInfo.width = static_cast<uint32_t>(renderView->RenderSize.Width);
		framebufferInfo.height = static_cast<uint32_t>(renderView->RenderSize.Height);
		framebufferInfo.attachmentCount = views.Count();
		framebufferInfo.pAttachments = views.Data();
		framebufferInfo.layers = 1;

		CheckVKResult(vkCreateFramebuffer(_device->GetDevice(), &framebufferInfo, nullptr, &_framebuffer));
		
		_commandBuffer = static_cast<CommandBufferVulkan*>(_device->GetGraphicsCommandPool().value()->Allocate(true));

		_commandBuffer->Begin(true, false);
		_commandBuffer->BeginRenderPass(_renderPass, _framebuffer, renderView);
	}

	RenderContextVulkan::~RenderContextVulkan()
	{
		if (_framebuffer != nullptr)
		{
			vkDestroyFramebuffer(_device->GetDevice(), _framebuffer, nullptr);
			_framebuffer = nullptr;
		}

		_renderPass = nullptr;
	}
}