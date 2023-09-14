#include "Renderpch.h"
#include "VulkanRenderContext.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanImage.h"
#include "VulkanUtils.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanContextRenderOperation::VulkanContextRenderOperation(VulkanFramebuffer& framebuffer, VulkanRenderPass& renderPass) :
		Framebuffer(framebuffer),
		RenderPass(renderPass),
		WaitOnSemaphores{},
		RenderCompletedSignalSemaphores{}
	{}

	VulkanRenderContext::VulkanRenderContext() :
		_backbufferIndex(-1),
		_vulkanRenderOperation{},
		_imageAvailableSemaphore(CreateUniqueRef<VulkanGraphicsSemaphore>()),
		_renderCompletedSemaphore(CreateUniqueRef<VulkanGraphicsSemaphore>()),
		_renderCompletedFence(CreateUniqueRef<VulkanGraphicsFence>(true)),
		_cache(CreateUniqueRef<VulkanRenderContextCache>())
	{
		DeviceQueue* graphicsQueue = _device->GetQueue(DeviceQueue::Type::Graphics);
		if (!graphicsQueue)
		{
			throw std::exception("A graphics queue needs to be created for rendering");
		}

		_commandBuffer = graphicsQueue->Pool.Allocate(true);
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		_imageAvailableSemaphore.reset();
		_renderCompletedSemaphore.reset();
		_renderCompletedFence.reset();

		_cache.reset();

		DeviceQueue* graphicsQueue = _device->GetQueue(DeviceQueue::Type::Graphics);
		if (graphicsQueue && _commandBuffer)
		{
			graphicsQueue->Pool.Free(*_commandBuffer);
		}

		_commandBuffer.reset();
	}

	void VulkanRenderContext::WaitForRenderingToComplete()
	{
		_renderCompletedFence->Wait(Math::MaxValue<uint64>());
	}

	void VulkanRenderContext::SetViewportRect(const RectInt& viewportRect)
	{
		_viewportRect = viewportRect;
	}

	void VulkanRenderContext::SetScissorRect(const RectInt& scissorRect)
	{
		_scissorRect = scissorRect;
	}

	void VulkanRenderContext::AddWaitOnSemaphore(GraphicsSemaphore& semaphore)
	{
		Assert(_vulkanRenderOperation.has_value())

		VulkanGraphicsSemaphore* vulkanSemaphore = static_cast<VulkanGraphicsSemaphore*>(&semaphore);
		_vulkanRenderOperation->WaitOnSemaphores.push_back(vulkanSemaphore);
	}

	void VulkanRenderContext::AddRenderCompletedSignalSemaphore(GraphicsSemaphore& semaphore)
	{
		Assert(_vulkanRenderOperation.has_value())

		VulkanGraphicsSemaphore* vulkanSemaphore = static_cast<VulkanGraphicsSemaphore*>(&semaphore);
		_vulkanRenderOperation->RenderCompletedSignalSemaphores.push_back(vulkanSemaphore);
	}

	void VulkanRenderContext::SetBackbufferIndex(uint32 index)
	{
		_backbufferIndex = static_cast<int>(index);
	}

	bool VulkanRenderContext::ResetImpl()
	{
		_backbufferIndex = -1;
		_vulkanRenderOperation.reset();
		_renderCompletedFence->Reset();

		return true;
	}

	bool VulkanRenderContext::BeginImpl()
	{

		try
		{
			const std::vector<RenderTarget>& rts = _renderOperation->RenderView.GetRenderTargets();
			std::vector<VulkanImage*> vulkanImages(rts.size());

			for (size_t i = 0; i < rts.size(); i++)
				vulkanImages.at(i) = static_cast<VulkanImage*>(rts.at(i).Image);

			VulkanRenderPass& renderPass = _device->GetCache()->GetOrCreateRenderPass(_renderOperation->Pipeline);
			VulkanFramebuffer& framebuffer = _cache->GetOrCreateFramebuffer(_renderOperation->RenderView.GetViewportRect().Size, renderPass, vulkanImages);

			// Setup the Vulkan-specific render operation
			_vulkanRenderOperation.emplace(VulkanContextRenderOperation(framebuffer, renderPass));
			_vulkanRenderOperation->WaitOnSemaphores.push_back(_imageAvailableSemaphore.get());
			_vulkanRenderOperation->RenderCompletedSignalSemaphores.push_back(_renderCompletedSemaphore.get());

			_commandBuffer->Begin(true, false);

			AddPreRenderPassImageTransitions();

			VkRenderPassBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			beginInfo.renderPass = renderPass.GetRenderPass();
			beginInfo.framebuffer = framebuffer.GetFramebuffer();

			const RectInt& viewportRect = _renderOperation->RenderView.GetViewportRect();
			beginInfo.renderArea.offset.x = static_cast<uint32_t>(viewportRect.Offset.X);
			beginInfo.renderArea.offset.y = static_cast<uint32_t>(-viewportRect.Offset.Y); // TODO: check if this is correct
			beginInfo.renderArea.extent.width = static_cast<uint32_t>(viewportRect.Size.Width);
			beginInfo.renderArea.extent.height = static_cast<uint32_t>(viewportRect.Size.Height);

			std::vector<VkClearValue> clearValues(rts.size());

			// Set clear clear color for each render target
			for (size_t i = 0; i < clearValues.size(); i++)
			{
				const RenderTarget& rt = rts.at(i);
				const AttachmentFormat& attachment = _renderOperation->Pipeline.InputAttachments.at(i);
				VkClearValue& clearValue = clearValues.at(i);

				if (IsDepthFormat(attachment.PixelFormat) || IsStencilFormat(attachment.PixelFormat))
				{
					clearValue.depthStencil.depth = static_cast<float>(rt.ClearValue.X);
					clearValue.depthStencil.stencil = static_cast<uint32>(Math::Round(rt.ClearValue.Y));
				}
				else
				{
					clearValues[i].color =
					{
						static_cast<float>(rt.ClearValue.X),
						static_cast<float>(rt.ClearValue.Y),
						static_cast<float>(rt.ClearValue.Z),
						static_cast<float>(rt.ClearValue.W)
					};
				}
			}

			beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			beginInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(_commandBuffer->GetCmdBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
		}
		catch (const std::exception& ex)
		{
			CocoError("Could not begin VulkanRenderContext: {}", ex.what())
			return false;
		}

		return true;
	}

	bool VulkanRenderContext::BeginNextPassImpl()
	{
		vkCmdNextSubpass(_commandBuffer->GetCmdBuffer(), VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		return true;
	}

	void VulkanRenderContext::EndImpl()
	{
		if (_currentState != State::InRender || !_vulkanRenderOperation.has_value())
		{
			CocoError("RenderContext End() called when it wasn't recording")
			return;
		}

		vkCmdEndRenderPass(_commandBuffer->GetCmdBuffer());

		AddPostRenderPassImageTransitions();

		// Submit the command buffer and free it
		_commandBuffer->EndAndSubmit(
			&_vulkanRenderOperation->WaitOnSemaphores,
			&_vulkanRenderOperation->RenderCompletedSignalSemaphores,
			_renderCompletedFence.get());
	}

	void VulkanRenderContext::AddPreRenderPassImageTransitions()
	{
		const auto& attachmentFormats = _renderOperation->Pipeline.InputAttachments;

		for (size_t i = 0; i < attachmentFormats.size(); i++)
		{
			const AttachmentFormat& attachment = attachmentFormats.at(i);

			// Don't bother transitioning layouts for attachments that aren't preserved between frames since Vulkan will initialize them for us
			if (!attachment.ShouldPreserve)
				continue;

			VkImageLayout layout = ToAttachmentLayout(attachment.PixelFormat);
			VulkanImage* image = static_cast<VulkanImage*>(_renderOperation->RenderView.GetRenderTarget(i).Image);

			// TODO: image->TransitionLayout(_vulkanRenderOperation->CommandBuffer, layout);
		}
	}

	void VulkanRenderContext::AddPostRenderPassImageTransitions()
	{
		const auto& attachmentFormats = _renderOperation->Pipeline.InputAttachments;

		for (size_t i = 0; i < attachmentFormats.size(); i++)
		{
			const AttachmentFormat& attachment = attachmentFormats.at(i);
			VkImageLayout layout = ToAttachmentLayout(attachment.PixelFormat);
			VulkanImage* image = static_cast<VulkanImage*>(_renderOperation->RenderView.GetRenderTarget(i).Image);

			// Since Vulkan automatically transitions layouts between passes, update the image's layout to match the layouts of the attachments
			image->_currentLayout = layout;

			// Transition any attachments that will be presented
			if ((image->GetDescription().UsageFlags & ImageUsageFlags::Presented) == ImageUsageFlags::Presented)
			{
				image->TransitionLayout(*_commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			}
		}
	}
}