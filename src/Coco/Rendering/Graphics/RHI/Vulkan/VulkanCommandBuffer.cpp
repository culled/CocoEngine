#include "Renderpch.h"
#include "VulkanCommandBuffer.h"

#include "VulkanGraphicsFence.h"
#include "VulkanGraphicsSemaphore.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer buffer, VkQueue queue) :
		_commandBuffer(buffer),
		_queue(queue)
	{}

	void VulkanCommandBuffer::Begin(bool isSingleUse, bool isSimultaneousUse)
	{
		VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
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
		std::span<std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags>> waitSemaphores,
		std::span<Ref<VulkanGraphicsSemaphore>> signalSemaphores, 
		Ref<VulkanGraphicsFence> signalFence)
	{
		std::vector<VkSemaphore> waitVkSemaphores;
		std::transform(waitSemaphores.begin(), waitSemaphores.end(),
			std::back_inserter(waitVkSemaphores),
			[](const std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags>& pair)
			{
				const Ref<VulkanGraphicsSemaphore>& sem = pair.first;
				CocoAssert(sem, "Semaphore was invalid")
				return sem->GetSemaphore();
			});

		std::vector<VkPipelineStageFlags> waitPipelineStageFlags;
		std::transform(waitSemaphores.begin(), waitSemaphores.end(),
			std::back_inserter(waitPipelineStageFlags),
			[](const std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags>& pair)
			{
				return pair.second;
			});

		std::vector<VkSemaphore> signalVkSemaphores;
		std::transform(signalSemaphores.begin(), signalSemaphores.end(),
			std::back_inserter(signalVkSemaphores),
			[](const Ref<VulkanGraphicsSemaphore>& sem)
			{
				CocoAssert(sem, "Semaphore was invalid")
				return sem->GetSemaphore();
			});

		VkFence fence = signalFence.IsValid() ? signalFence->GetFence() : VK_NULL_HANDLE;

		VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_commandBuffer;
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitVkSemaphores.size());
		submitInfo.pWaitSemaphores = waitVkSemaphores.data();
		submitInfo.pWaitDstStageMask = waitPipelineStageFlags.data();
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalVkSemaphores.size());
		submitInfo.pSignalSemaphores = signalVkSemaphores.data();

		AssertVkSuccess(vkQueueSubmit(_queue, 1, &submitInfo, fence));
	}

	void VulkanCommandBuffer::EndAndSubmit(
		std::span<std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags>> waitSemaphores, 
		std::span<Ref<VulkanGraphicsSemaphore>> signalSemaphores, 
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