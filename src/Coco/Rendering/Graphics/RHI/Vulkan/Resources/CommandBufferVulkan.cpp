#include "CommandBufferVulkan.h"

#include "../GraphicsDeviceVulkan.h"
#include "CommandBufferPoolVulkan.h"
#include "GraphicsSemaphoreVulkan.h"
#include "GraphicsFenceVulkan.h"

namespace Coco::Rendering::Vulkan
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
	}

	CommandBufferVulkan::~CommandBufferVulkan()
	{
		_device->WaitForIdle();

		if (_commandBuffer != nullptr)
		{
			vkFreeCommandBuffers(_device->GetDevice(), _pool->GetPool(), 1, &_commandBuffer);
			_commandBuffer = nullptr;
		}
	}

	void CommandBufferVulkan::BeginImpl(bool isSingleUse, bool isSimultaneousUse)
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
	}

	void CommandBufferVulkan::EndImpl()
	{
		AssertVkResult(vkEndCommandBuffer(_commandBuffer));
	}

	void CommandBufferVulkan::SubmitImpl(
		const List<IGraphicsSemaphore*>& waitSemaphores,
		const List<IGraphicsSemaphore*>& signalSemaphores,
		IGraphicsFence* signalFence)
	{
		List<VkSemaphore> vulkanWaitSemaphores;

		for (int i = 0; i < waitSemaphores.Count(); i++)
		{
			const GraphicsSemaphoreVulkan* vulkanSemaphore = static_cast<GraphicsSemaphoreVulkan*>(waitSemaphores[i]);
			vulkanWaitSemaphores.Add(vulkanSemaphore->GetSemaphore());
		}

		List<VkSemaphore> vulkanSignalSemaphores;

		for (int i = 0; i < signalSemaphores.Count(); i++)
		{
			const GraphicsSemaphoreVulkan* vulkanSemaphore = static_cast<GraphicsSemaphoreVulkan*>(signalSemaphores[i]);
			vulkanSignalSemaphores.Add(vulkanSemaphore->GetSemaphore());
		}

		VkFence vulkanSignalFence = VK_NULL_HANDLE;

		if (const GraphicsFenceVulkan* vulkanFence = dynamic_cast<GraphicsFenceVulkan*>(signalFence))
			vulkanSignalFence = vulkanFence->GetFence();

		// TODO: make this configurable?
		const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pWaitDstStageMask = &waitStages[0];
		submitInfo.pCommandBuffers = &_commandBuffer;
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(vulkanWaitSemaphores.Count());
		submitInfo.pWaitSemaphores = vulkanWaitSemaphores.Data();
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(vulkanSignalSemaphores.Count());
		submitInfo.pSignalSemaphores = vulkanSignalSemaphores.Data();

		AssertVkResult(vkQueueSubmit(_pool->GetQueue(), 1, &submitInfo, vulkanSignalFence));
	}

	void CommandBufferVulkan::ResetImpl()
	{
		AssertVkResult(vkResetCommandBuffer(_commandBuffer, 0));
	}
}
