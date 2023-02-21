#include "CommandBufferPoolVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include "CommandBufferVulkan.h"

namespace Coco::Rendering
{
	CommandBufferPoolVulkan::CommandBufferPoolVulkan(GraphicsDeviceVulkan* device, Ref<VulkanQueue> queue) :
		_device(device), _queue(queue)
	{
		VkCommandPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = static_cast<uint32_t>(queue->QueueFamily);
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		CheckVKResult(vkCreateCommandPool(_device->GetDevice(), &createInfo, nullptr, &_pool));
	}

	CommandBufferPoolVulkan::~CommandBufferPoolVulkan()
	{
		if (_pool != nullptr)
		{
			vkDestroyCommandPool(_device->GetDevice(), _pool, nullptr);
			_pool = nullptr;
		}
	}

	CommandBuffer* CommandBufferPoolVulkan::Allocate(bool isPrimary)
	{
		CommandBufferVulkan* buffer = new CommandBufferVulkan(isPrimary, _device, this);
		_allocatedBuffers.Add(Managed<CommandBufferVulkan>(buffer));
		return buffer;
	}

	void CommandBufferPoolVulkan::Free(CommandBuffer* buffer)
	{
		auto it = std::find_if(_allocatedBuffers.begin(), _allocatedBuffers.end(), [buffer](const Managed<CommandBufferVulkan>& other) {
			return buffer == other.get();
			});

		if (it != _allocatedBuffers.end())
			_allocatedBuffers.Erase(it);
	}
}
