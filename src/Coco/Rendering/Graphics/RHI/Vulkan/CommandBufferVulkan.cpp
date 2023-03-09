#include "CommandBufferVulkan.h"

#include <Coco/Rendering/Graphics/RenderView.h>
#include "GraphicsDeviceVulkan.h"
#include "CommandBufferPoolVulkan.h"
#include "GraphicsSemaphoreVulkan.h"
#include "GraphicsFenceVulkan.h"

namespace Coco::Rendering
{
	CommandBufferVulkan::CommandBufferVulkan(bool isPrimary, GraphicsDeviceVulkan* device, CommandBufferPoolVulkan* pool) : CommandBuffer(isPrimary),
		_device(device), _pool(pool)
	{
		VkCommandBufferAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = _pool->GetPool();
		allocateInfo.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocateInfo.commandBufferCount = 1;

		AssertVkResult(vkAllocateCommandBuffers(_device->GetDevice(), &allocateInfo, &_commandBuffer));

		CurrentState = State::Ready;
	}

	CommandBufferVulkan::~CommandBufferVulkan()
	{
		if (_commandBuffer != nullptr)
		{
			vkFreeCommandBuffers(_device->GetDevice(), _pool->GetPool(), 1, &_commandBuffer);
			_commandBuffer = nullptr;
		}
	}

	void CommandBufferVulkan::Begin(bool isSingleUse, bool isSimultaneousUse)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (isSingleUse)
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (isSimultaneousUse)
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		AssertVkResult(vkBeginCommandBuffer(_commandBuffer, &beginInfo));
		CurrentState = State::Recording;
	}

	void CommandBufferVulkan::End()
	{
		if (_isInRenderPass)
			CmdEndRenderPass();

		AssertVkResult(vkEndCommandBuffer(_commandBuffer));
		CurrentState = State::RecordingEnded;
	}

	void CommandBufferVulkan::Submit(const List<GraphicsSemaphore*>& waitSemaphores, const List<GraphicsSemaphore*>& signalSemaphores, GraphicsFence* signalFence)
	{
		List<VkSemaphore> vulkanWaitSemaphores;

		for (int i = 0; i < waitSemaphores.Count(); i++)
		{
			if (GraphicsSemaphoreVulkan* vulkanSemaphore = dynamic_cast<GraphicsSemaphoreVulkan*>(waitSemaphores[i]))
				vulkanWaitSemaphores.Add(vulkanSemaphore->GetSemaphore());
		}

		List<VkSemaphore> vulkanSignalSemaphores;

		for (int i = 0; i < signalSemaphores.Count(); i++)
		{
			if (GraphicsSemaphoreVulkan* vulkanSemaphore = dynamic_cast<GraphicsSemaphoreVulkan*>(signalSemaphores[i]))
				vulkanSignalSemaphores.Add(vulkanSemaphore->GetSemaphore());
		}

		VkFence vulkanSignalFence = VK_NULL_HANDLE;

		if (GraphicsFenceVulkan* vulkanFence = dynamic_cast<GraphicsFenceVulkan*>(signalFence))
			vulkanSignalFence = vulkanFence->GetFence();

		// TODO: make this configurable?
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.pCommandBuffers = &_commandBuffer;
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(vulkanWaitSemaphores.Count());
		submitInfo.pWaitSemaphores = vulkanWaitSemaphores.Data();
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(vulkanSignalSemaphores.Count());
		submitInfo.pSignalSemaphores = vulkanSignalSemaphores.Data();

		AssertVkResult(vkQueueSubmit(_pool->GetQueue(), 1, &submitInfo, vulkanSignalFence));

		CurrentState = State::Submitted;
	}

	void CommandBufferVulkan::Reset()
	{
		AssertVkResult(vkResetCommandBuffer(_commandBuffer, 0));
		CurrentState = State::Ready;
	}

	void CommandBufferVulkan::CmdBeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, const Ref<RenderView>& renderView)
	{
		VkRenderPassBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.renderPass = renderPass;
		beginInfo.framebuffer = framebuffer;

		beginInfo.renderArea.offset.x = static_cast<uint32_t>(renderView->RenderOffset.X);
		beginInfo.renderArea.offset.y = static_cast<uint32_t>(renderView->RenderOffset.Y);
		beginInfo.renderArea.extent.width = static_cast<uint32_t>(renderView->RenderSize.Width);
		beginInfo.renderArea.extent.height = static_cast<uint32_t>(renderView->RenderSize.Height);

		VkClearValue clearValue = { {{
				static_cast<float>(renderView->ClearColor.R), 
				static_cast<float>(renderView->ClearColor.G), 
				static_cast<float>(renderView->ClearColor.B), 
				static_cast<float>(renderView->ClearColor.A)
			}} };

		beginInfo.clearValueCount = 1;
		beginInfo.pClearValues = &clearValue;

		vkCmdBeginRenderPass(_commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

		_isInRenderPass = true;
	}

	void CommandBufferVulkan::CmdEndRenderPass()
	{
		vkCmdEndRenderPass(_commandBuffer);
		_isInRenderPass = false;
	}
}
