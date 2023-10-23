#include "Renderpch.h"
#include "VulkanCommandBuffer.h"
#include "VulkanGraphicsSemaphore.h"
#include "VulkanGraphicsFence.h"

namespace Coco::Rendering::Vulkan
{
	VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer buffer, VkQueue queue) :
		_commandBuffer(buffer),
		_queue(queue),
		_currentState(State::Ready)
	{}

	void VulkanCommandBuffer::Begin(bool isSingleUse, bool isSimultaneousUse)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (isSingleUse)
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (isSimultaneousUse)
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		AssertVkSuccess(vkBeginCommandBuffer(_commandBuffer, &beginInfo));
	}

	void VulkanCommandBuffer::End()
	{
		AssertVkSuccess(vkEndCommandBuffer(_commandBuffer));
	}

	void VulkanCommandBuffer::Submit(
		std::vector<Ref<VulkanGraphicsSemaphore>>* waitSemaphores,
		std::vector<Ref<VulkanGraphicsSemaphore>>* signalSemaphores,
		Ref<VulkanGraphicsFence> signalFence)
	{
		std::vector<VkSemaphore> vulkanWaitSemaphores;
		std::vector<VkPipelineStageFlags> waitStages;

		if (waitSemaphores)
		{
			for (Ref<VulkanGraphicsSemaphore>& s : *waitSemaphores)
			{
				if (!s.IsValid())
					continue;

				vulkanWaitSemaphores.push_back(s->GetSemaphore());

				// TODO: configurable wait stages?
				waitStages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
			}
		}

		std::vector<VkSemaphore> vulkanSignalSemaphores;

		if (signalSemaphores)
		{
			for (Ref<VulkanGraphicsSemaphore>& s : *signalSemaphores)
			{
				if (!s.IsValid())
					continue;

				vulkanSignalSemaphores.push_back(s->GetSemaphore());
			}
		}

		VkFence fence = signalFence.IsValid() ? signalFence->GetFence() : VK_NULL_HANDLE;

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_commandBuffer;
		submitInfo.pWaitDstStageMask = waitStages.data();
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(vulkanWaitSemaphores.size());
		submitInfo.pWaitSemaphores = vulkanWaitSemaphores.data();
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(vulkanSignalSemaphores.size());
		submitInfo.pSignalSemaphores = vulkanSignalSemaphores.data();

		AssertVkSuccess(vkQueueSubmit(_queue, 1, &submitInfo, fence));
	}

	void VulkanCommandBuffer::EndAndSubmit(
		std::vector<Ref<VulkanGraphicsSemaphore>>* waitSemaphores,
		std::vector<Ref<VulkanGraphicsSemaphore>>* signalSemaphores,
		Ref<VulkanGraphicsFence> signalFence)
	{
		End();
		Submit(waitSemaphores, signalSemaphores, signalFence);
	}

	void VulkanCommandBuffer::Reset()
	{
		AssertVkSuccess(vkResetCommandBuffer(_commandBuffer, 0));
	}
}