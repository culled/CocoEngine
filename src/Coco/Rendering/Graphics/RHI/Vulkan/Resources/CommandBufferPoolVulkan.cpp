#include "CommandBufferPoolVulkan.h"

#include <Coco/Core/Logging/Logger.h>
#include "../GraphicsDeviceVulkan.h"

namespace Coco::Rendering::Vulkan
{
	CommandBufferPoolVulkan::CommandBufferPoolVulkan(GraphicsDeviceVulkan* device, Ref<VulkanQueue> queue) :
		_device(device), _queue(queue)
	{
		VkCommandPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = static_cast<uint32_t>(queue->QueueFamily);
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		AssertVkResult(vkCreateCommandPool(_device->GetDevice(), &createInfo, nullptr, &_pool));
	}

	CommandBufferPoolVulkan::~CommandBufferPoolVulkan()
	{
		_device->WaitForIdle();

		_allocatedBuffers.Clear();

		if (_pool != nullptr)
		{
			vkDestroyCommandPool(_device->GetDevice(), _pool, nullptr);
			_pool = nullptr;
		}
	}

	CommandBuffer* CommandBufferPoolVulkan::Allocate(bool isPrimary)
	{
		_allocatedBuffers.Add(CreateManaged<CommandBufferVulkan>(isPrimary, _device, this));
		const Managed<CommandBufferVulkan>& buffer = _allocatedBuffers.Last();
		return buffer.get();
	}

	void CommandBufferPoolVulkan::Free(CommandBuffer* buffer) noexcept
	{
		auto it = _allocatedBuffers.Find([buffer](const Managed<CommandBufferVulkan>& other) {
			return buffer == other.get();
			});

		if (it != _allocatedBuffers.end())
			_allocatedBuffers.Remove(it);
	}

	void CommandBufferPoolVulkan::WaitForQueue() noexcept
	{
		try
		{
			AssertVkResult(vkQueueWaitIdle(_queue->Queue));
		}
		catch (const VulkanOperationException& ex)
		{
			LogWarning(_device->GetLogger(), FormattedString("Failed to wait for queue idle. Falling back to device idle: {}", ex.what()));
			_device->WaitForIdle();
		}
		catch (...)
		{
			LogWarning(_device->GetLogger(), "Failed to wait for queue idle. Falling back to device idle");
			_device->WaitForIdle();
		}
	}
}
